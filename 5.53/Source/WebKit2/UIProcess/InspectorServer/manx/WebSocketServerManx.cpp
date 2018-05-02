/*
 * Copyright (C) 2012 Sony Network Entertainment Intl. All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY SONY NETWORK ENTERTAINMENT INTL. ``AS IS''
 * ANY ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SONY NETWORK ENTERTAINMENT INTL.
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include "config.h"
#include "WebSocketServer.h"

#include "SocketStreamHandle.h"
#include "WebCore/platform/Logging.h" // for LogNetwork, but hidden by WebKit2/platform/Logging.h
#include "WebSocketServerConnection.h"
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <wtf/Functional.h>
#include <wtf/NonCopyable.h>
#include <wtf/RunLoop.h>
#include <wtf/Threading.h>
#include <wtf/text/CString.h>

using namespace WebCore;

namespace WebKit {

class SocketConnectionHandler {
    WTF_MAKE_NONCOPYABLE(SocketConnectionHandler);
public:
    SocketConnectionHandler(WebSocketServer*);

    bool startListening(const String& bindAddress, unsigned short port);
    void stopListening();

private:
    void dispatchIncomingConnection(int connectionfd);

    static void connectionListenerThread(void* context);

    WebSocketServer* m_server;
    int m_socketfd;

    ThreadIdentifier m_serverThreadId;
    RunLoop& m_ownerRunLoop;
};


SocketConnectionHandler::SocketConnectionHandler(WebSocketServer *server)
    : m_server(server)
    , m_socketfd(-1)
    , m_serverThreadId(0)
    , m_ownerRunLoop(RunLoop::current())
{
}

bool SocketConnectionHandler::startListening(const String& bindAddress, unsigned short port)
{
    if (m_socketfd >= 0)
        stopListening();

    m_socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_socketfd < 0) {
        LOG_ERROR("Couldn't create Inspector Server socket! (%d)", errno);
        return false;
    }

    // Allow reusing the address for bind() if the socket is in TIME_WAIT state
    int sockoptVal = 1;
    if (setsockopt(m_socketfd, SOL_SOCKET, SO_REUSEADDR, (char*)&sockoptVal, sizeof(sockoptVal)) < 0) {
        LOG_ERROR("Couldn't set the SO_REUSEADDR flag on the Inspector Server socket! (%d)", errno);
        stopListening(); // will close the socket and reset m_socketfd
        return false;
    }

#if OS(ORBIS)
    if (setsockopt(m_socketfd, SOL_SOCKET, SO_KEEPSOCKET, &sockoptVal, sizeof(sockoptVal)) < 0) {
        LOG_ERROR("Couldn't set the SO_KEEPSOCKET flag on the Inspector Server socket! (%d)", errno);
        stopListening(); // will close the socket and reset m_socketfd
        return false;
    }
    if (setsockopt(m_socketfd, SOL_SOCKET, SO_USE_DEVLAN, &sockoptVal, sizeof(sockoptVal)) < 0) {
        LOG_ERROR("Couldn't set the SO_USE_DEVLAN flag on the Inspector Server socket! (%d)", errno);
        stopListening(); // will close the socket and reset m_socketfd
        return false;
    }
#endif

    sockaddr_in serveraddress;
    memset(&serveraddress, 0, sizeof(sockaddr_in));
    serveraddress.sin_family = AF_INET;
    serveraddress.sin_port = htons(port);
    serveraddress.sin_addr.s_addr = htonl(INADDR_ANY);

    const CString ipAddress = bindAddress.latin1();

    if (ipAddress.length()) {
        if (inet_pton(AF_INET, ipAddress.data(), &serveraddress.sin_addr.s_addr) <= 0) {
            LOG_ERROR("Invalid Inspector Server IP address '%s'!", ipAddress.data());
            return false;
        }
    }

    if (bind(m_socketfd, (sockaddr*) &serveraddress, sizeof(serveraddress)) < 0) {
        LOG_ERROR("Couldn't bind Inspector Server socket to %s:%d! (%d)", ipAddress.length() ? ipAddress.data() : "*", port, errno);
        return false;
    }

    if (listen(m_socketfd, 10) < 0) {
        LOG_ERROR("Couldn't listen on Inspector Server socket! (%d)", errno);
        return false;
    }

    m_serverThreadId = createThread(&SocketConnectionHandler::connectionListenerThread, this, "InspectorServerThread");
    if (!m_serverThreadId) {
        LOG_ERROR("Couldn't create Inspector Server thread! (%d)", errno);
        return false;
    }

    LOG(Network, "Inspector Server: start listening on port %d ...", port);
    return true; // success!
}

void SocketConnectionHandler::stopListening()
{
    if (m_socketfd <0)
        return;

    LOG(Network, "Inspector Server: stop listening on socket %d .", m_socketfd);

    if (shutdown(m_socketfd, SHUT_RDWR) < 0)
        LOG_ERROR("Shutdown sequence for Inspector Server socket failed! (%d)", errno);
    if (close(m_socketfd) < 0)
        LOG_ERROR("Closing Inspector Server socket failed! (%d)", errno);

    m_socketfd = -1;

    if (m_serverThreadId)
        waitForThreadCompletion(m_serverThreadId);
}

void SocketConnectionHandler::dispatchIncomingConnection(int connectionfd)
{
#if USE(NTF)
    auto webSocketConnection = std::make_unique<WebSocketServerConnection>(m_server->client(), m_server);
    webSocketConnection->setSocketHandle(SocketStreamHandle::create(connectionfd, webSocketConnection.get()));
    m_server->didAcceptConnection(WTF::move(webSocketConnection));
#endif
}

void SocketConnectionHandler::connectionListenerThread(void* context)
{
    SocketConnectionHandler* thiz = reinterpret_cast<SocketConnectionHandler*>(context);
    while (true) {
        // Don't bother protecting m_socketfd with a mutex or even checking it before calling accept()
        // Yes, there is a possible race condition if we call stopListening() as we enter the loop,
        // but that's not a normal scenario and would actually be hard to achieve given the call sequence
        // for platformListen() and platformClose().
        // In the worst case, m_socket would be -1 when entering this loop, which would just error out below.
        int connectionfd = accept(thiz->m_socketfd, 0, 0);
        if (connectionfd >= 0) {
            LOG(Network, "Inspector Server: got new connection, socketfd=%d", connectionfd);
            thiz->m_ownerRunLoop.dispatch(bind(&SocketConnectionHandler::dispatchIncomingConnection, thiz, connectionfd));
        } else {
            if (errno != ECONNABORTED)
                LOG_ERROR("Error while checking Inspector Server socket for incoming connections! (%d)", errno);
            break;
        }
    }
}


void WebSocketServer::platformInitialize()
{
    m_connectionHandler = new SocketConnectionHandler(this);
}

bool WebSocketServer::platformListen(const String& bindAddress, unsigned short port)
{
    ASSERT(m_connectionHandler);
    return m_connectionHandler->startListening(bindAddress, port);
}

void WebSocketServer::platformClose()
{
    ASSERT(m_connectionHandler);
    m_connectionHandler->stopListening();
    m_state = Closed;
}

}
