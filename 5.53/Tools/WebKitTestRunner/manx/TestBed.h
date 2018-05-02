#ifndef TestBed_h
#define TestBed_h

#include <WebKit/WebKit2_C.h>
#include <manx/RunLoop.h>
#include <sw/sw.h>
#include <wtf/CurrentTime.h>

class TestBed : public SW::WindowManagerClient {
public:
    class WaitDialog : public SW::DialogWindow {
    public:
        WaitDialog(SW::WindowManager* windowManager, WKViewRef webView)
            : SW::DialogWindow(windowManager)
            , m_webView(webView)
            {}

        virtual void paintToCurrentGLContext(const SW::Rect& clipRect) override {
            WKViewPaintFlags flags = 0;
            WKViewPaintToCurrentGLContext(m_webView, WKRectMake(clipRect.m_pos.m_x, clipRect.m_pos.m_y, clipRect.m_size.m_width, clipRect.m_size.m_height), flags);
        }
        virtual SW::Rect clientRect() override { return SW::Rect(SW::Point(0, 0), SW::Size(DISPLAY_WIDTH, DISPLAY_HEIGHT)); }

        WKViewRef m_webView;
    };

    static const int DISPLAY_WIDTH = 1920;
    static const int DISPLAY_HEIGHT = 1080;

    TestBed() {
        m_windowManager = SW::WindowManager::create(this, DISPLAY_WIDTH, DISPLAY_HEIGHT);
    }

    virtual void updateClient() override {
        if (m_done && *m_done) {
            m_windowManager->quit();
            return;
        }
        if (m_timeout > 0 && currentTime() >= m_timeout) {
            m_windowManager->quit();
            return;
        }
        Manx::RunLoop::poll();
    }

    void notifyDone() {
        m_windowManager->quit();
    }

    void runUntil(WKViewRef webView, bool* done, double timeout) {
        if (timeout > 0)
            m_timeout = currentTime() + timeout;
        else
            m_timeout = -1;
        m_done = done;
        WaitDialog dialog(m_windowManager, webView);
        dialog.show();
    }

    SW::WindowManager* m_windowManager;
    double m_timeout;
    bool* m_done;
};

#endif // TestBed_h
