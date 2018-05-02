add_definitions(-DBUILDING_WEBKIT)

if (SECURE_WEBCORE)
    set(WebKit2_OUTPUT_NAME SceWebKit2Secure)
else ()
    set(WebKit2_OUTPUT_NAME SceWebKit2)
endif ()

list(APPEND WebKit2_SOURCES
    Platform/IPC/unix/AttachmentUnix.cpp
    Platform/IPC/unix/ConnectionUnix.cpp
    Platform/efl/LoggingEfl.cpp
    Platform/manx/ModuleManx.cpp
    Platform/unix/SharedMemoryUnix.cpp
    Shared/API/c/cairo/WKImageCairo.cpp
    Shared/API/c/manx/WKErrorManx.cpp
    Shared/API/c/manx/WKURLResponseManx.cpp
    Shared/CoordinatedGraphics/CoordinatedBackingStore.cpp
    Shared/CoordinatedGraphics/CoordinatedGraphicsArgumentCoders.cpp
    Shared/CoordinatedGraphics/CoordinatedGraphicsScene.cpp
    Shared/CoordinatedGraphics/WebCoordinatedSurface.cpp
    Shared/CoordinatedGraphics/manx/TextureMapperInspector.cpp
    Shared/Downloads/manx/DownloadManx.cpp
    Shared/cairo/ShareableBitmapCairo.cpp
    Shared/manx/NativeContextMenuItemManx.cpp
    Shared/manx/NativeWebKeyboardEventManx.cpp
    Shared/manx/NativeWebMouseEventManx.cpp
    Shared/manx/NativeWebWheelEventManx.cpp
    Shared/manx/PlatformCertificateInfo.cpp
    Shared/manx/WebAccessibilityObject.cpp
    Shared/manx/WebAccessibilityObject.h
    Shared/manx/WebCoreArgumentCodersManx.cpp
    Shared/manx/WebEventFactory.cpp
    Shared/unix/ChildProcessMain.cpp
    UIProcess/API/C/manx/WKAXObject.cpp
    UIProcess/API/C/manx/WKAXObject.h
    UIProcess/API/C/manx/WKContextManx.cpp
    UIProcess/API/C/manx/WKCookieManagerManx.cpp
    UIProcess/API/C/manx/WKCookieManagerManx.h
    UIProcess/API/C/manx/WKPagePrivateManx.cpp
    UIProcess/API/C/manx/WKPagePrivateManx.h
    UIProcess/API/C/manx/WKPopupMenuItem.cpp
    UIProcess/API/C/manx/WKPopupMenuItem.h
    UIProcess/API/C/manx/WKView.cpp
    UIProcess/BackingStore.cpp
    UIProcess/CoordinatedGraphics/CoordinatedDrawingAreaProxy.cpp
    UIProcess/CoordinatedGraphics/CoordinatedLayerTreeHostProxy.cpp
    UIProcess/CoordinatedGraphics/PageViewportController.cpp
    UIProcess/InspectorServer/manx/WebInspectorServerManx.cpp
    UIProcess/InspectorServer/manx/WebSocketServerManx.cpp
    UIProcess/Launcher/manx/ProcessLauncherManx.cpp
    UIProcess/Network/CustomProtocols/manx/CustomProtocolManagerProxyNone.cpp
    UIProcess/Plugins/unix/PluginInfoStoreUnix.cpp
    UIProcess/Storage/StorageManager.cpp
    UIProcess/cairo/BackingStoreCairo.cpp
    UIProcess/manx/ManxWebView.cpp
    UIProcess/manx/ManxWebViewClient.cpp
    UIProcess/manx/ManxWebViewClient.h
    UIProcess/manx/PageViewportControllerClientManx.cpp
    UIProcess/manx/TextCheckerManx.cpp
    UIProcess/manx/WebCookieManagerProxyManx.cpp
    UIProcess/manx/WebInspectorProxyManx.cpp
    UIProcess/manx/WebPageProxyManx.cpp
    UIProcess/manx/WebPopupMenuClient.cpp
    UIProcess/manx/WebPopupMenuClient.h
    UIProcess/manx/WebPopupMenuItemManx.cpp
    UIProcess/manx/WebPopupMenuItemManx.h
    UIProcess/manx/WebPopupMenuProxyManx.cpp
    UIProcess/manx/WebPopupMenuProxyManx.h
    UIProcess/manx/WebPreferencesManx.cpp
    UIProcess/manx/WebProcessPoolManx.cpp
    UIProcess/manx/WebProcessProxyManx.cpp
    UIProcess/manx/WebViewAccessibilityClient.cpp
    UIProcess/manx/WebViewAccessibilityClient.h
    WebProcess/InjectedBundle/API/c/manx/WKBundleFrameManx.cpp
    WebProcess/InjectedBundle/API/c/manx/WKBundleFrameManx.h
    WebProcess/InjectedBundle/API/c/manx/WKBundleManx.cpp
    WebProcess/InjectedBundle/API/c/manx/WKBundleManx.h

    WebProcess/InjectedBundle/manx/InjectedBundleGCClient.cpp
    WebProcess/InjectedBundle/manx/InjectedBundleGCClient.h
    WebProcess/InjectedBundle/manx/InjectedBundleManx.cpp
    WebProcess/WebCoreSupport/manx/WebContextMenuClientManx.cpp
    WebProcess/WebCoreSupport/manx/WebDiskCacheManager.cpp
    WebProcess/WebCoreSupport/manx/WebErrorsManx.cpp
    WebProcess/WebCoreSupport/manx/WebInspectorClientManx.cpp
    WebProcess/WebCoreSupport/manx/WebPopupMenuManx.cpp
    WebProcess/WebPage/CoordinatedGraphics/CoordinatedDrawingArea.cpp
    WebProcess/WebPage/CoordinatedGraphics/CoordinatedLayerTreeHost.cpp
    WebProcess/WebPage/CoordinatedGraphics/WebPageCoordinatedGraphics.cpp
    WebProcess/WebPage/manx/WebInspectorUIManx.cpp
    WebProcess/WebPage/manx/WebPageManx.cpp
    WebProcess/manx/WebProcessMainManx.cpp
    WebProcess/manx/WebProcessManx.cpp
)

if (NOT SHARED_CORE)
    list(APPEND WebKit2_SOURCES ${CMAKE_CURRENT_BINARY_DIR}/manx/export.pemd)
    list(INSERT WebKit2_LIBRARIES 0 PUBLIC)
    list(APPEND WebKit2_LIBRARIES PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/manx/export.pemd)
endif ()

list(APPEND WebProcess_SOURCES
    WebProcess/manx/WebProcessMainDummy.cpp
)
set(WebKit2_WebProcess_OUTPUT_NAME web_process_dummy)

list(APPEND WebKit2_MESSAGES_IN_FILES
    UIProcess/CoordinatedGraphics/CoordinatedLayerTreeHostProxy.messages.in
    WebProcess/WebPage/CoordinatedGraphics/CoordinatedLayerTreeHost.messages.in
)

list(APPEND WebKit2_INCLUDE_DIRECTORIES
    ${WEBCORE_DIR}/page/manx
    ${WEBCORE_DIR}/platform/cairo
    ${WEBCORE_DIR}/platform/graphics/cairo
    ${WEBCORE_DIR}/platform/graphics/manx
    ${WEBCORE_DIR}/platform/graphics/opentype
    ${WEBCORE_DIR}/platform/texmap
    ${WEBKIT2_DIR}/Shared/unix
    
    Shared/CoordinatedGraphics
    Shared/manx
    UIProcess/API/C/manx
    UIProcess/CoordinatedGraphics
    UIProcess/manx
    WebProcess/Downloads/manx
    WebProcess/InjectedBundle/API/c/manx
    WebProcess/InjectedBundle/manx
    WebProcess/WebCoreSupport/manx
    WebProcess/WebPage/CoordinatedGraphics
    WebProcess/WebPage/manx
    WebProcess/manx
    WebProcess/unix

    ${LIBJSCBRIDGE_INCLUDE_DIRS}
)

list(APPEND WebKit2_SYSTEM_INCLUDE_DIRECTORIES
    ${ICU_INCLUDE_DIRS}
    ${CAIRO_INCLUDE_DIRS}
    ${FREETYPE2_INCLUDE_DIRS}
    ${HARFBUZZ_INCLUDE_DIRS}
    ${EGL_INCLUDE_DIRS}
)

if (ORBIS AND MANX_ENABLE_SPLITPROC_JIT)
    list(APPEND WebKit2_LIBRARIES JscBridge_vm SceJitBridge)
    add_definitions(-DENABLE_SPLITPROC_JIT=1)
    add_definitions(-DBUILDING_SPLITPROC_VM=1)
endif ()

if (USE_CURL)
    list(APPEND WebKit2_INCLUDE_DIRECTORIES
        ${WEBKIT2_DIR}/Shared/Network/CustomProtocols/none
        ${WEBCORE_DIR}/platform/network/curl
    )
    list(APPEND WebKit2_SOURCES
        Shared/Network/CustomProtocols/none/CustomProtocolManagerNone.cpp
        WebProcess/Cookies/curl/WebCookieManagerCurl.cpp
    )
    set(manx_gfh_opt --platform curl)
elseif (USE_NTF)
    list(APPEND WebKit2_INCLUDE_DIRECTORIES
        ${WEBKIT2_DIR}/Shared/Network/CustomProtocols/none
        ${WEBCORE_DIR}/platform/network/ntf
    )
    list(APPEND WebKit2_SOURCES
        Shared/Network/CustomProtocols/none/CustomProtocolManagerNone.cpp
        WebProcess/Cookies/manx/WebCookieManagerManx.cpp
    )
    set(manx_gfh_opt --platform ntf)
endif ()

if (ENABLE_MANX_INDIRECT_COMPOSITING)
    list(APPEND WebKit2_SOURCES
        Shared/Manx/WebCanvasHole.cpp
    )
endif ()

add_custom_target(WebKit2-forwarding-headers
    COMMAND ${PERL_EXECUTABLE} ${WEBKIT2_DIR}/Scripts/generate-forwarding-headers.pl
        --include-path ${WEBKIT2_DIR}
        --output ${DERIVED_SOURCES_WEBKIT2_DIR}/include
        --platform manx
        ${manx_gfh_opt}
)

set(WEBKIT2_EXTRA_DEPENDENCIES
     WebKit2-forwarding-headers
)

webkit_wrap_sourcelist(${WebKit2_SOURCES} ${WebKit2_MESSAGES_IN_FILES})
