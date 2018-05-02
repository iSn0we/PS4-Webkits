list(APPEND WebCore_INCLUDE_DIRECTORIES
    ${WEBCORE_DIR}/page/manx
    ${WEBCORE_DIR}/page/scrolling/coordinatedgraphics
    ${WEBCORE_DIR}/platform/cairo
    ${WEBCORE_DIR}/platform/graphics/cairo
    ${WEBCORE_DIR}/platform/graphics/egl
    ${WEBCORE_DIR}/platform/graphics/manx
    ${WEBCORE_DIR}/platform/graphics/opengl
    ${WEBCORE_DIR}/platform/graphics/surfaces
    ${WEBCORE_DIR}/platform/graphics/surfaces/egl
    ${WEBCORE_DIR}/platform/graphics/texmap/coordinated
    ${WEBCORE_DIR}/platform/manx
    ${LIBJSCBRIDGE_INCLUDE_DIRS}
)

list(APPEND WebCore_SOURCES
    accessibility/manx/AXObjectCacheManx.cpp
    accessibility/manx/AccessibilityObjectManx.cpp
    editing/manx/EditorManx.cpp

    inspector/InspectorInputAgent.cpp
    inspector/InspectorPlayStationAgent.cpp
    page/manx/CursorNavigation.cpp
    page/manx/DragControllerManx.cpp
    page/manx/EventHandlerManx.cpp
    page/manx/FocusControllerManx.cpp
    page/scrolling/coordinatedgraphics/ScrollingCoordinatorCoordinatedGraphics.cpp
    page/scrolling/coordinatedgraphics/ScrollingStateNodeCoordinatedGraphics.cpp
    page/scrolling/coordinatedgraphics/ScrollingStateScrollingNodeCoordinatedGraphics.cpp
    platform/LocalizedStrings.cpp
    platform/graphics/GLContext.cpp
    platform/graphics/ImageSource.cpp
    platform/graphics/PlatformDisplay.cpp
    platform/graphics/WOFFFileFormat.cpp
    platform/graphics/cairo/BackingStoreBackendCairoImpl.cpp
    platform/graphics/cairo/BitmapImageCairo.cpp
    platform/graphics/cairo/CairoUtilities.cpp
    platform/graphics/cairo/GradientCairo.cpp
    platform/graphics/cairo/GraphicsContextCairo.cpp
    platform/graphics/cairo/ImageBufferCairo.cpp
    platform/graphics/cairo/ImageCairo.cpp
    platform/graphics/cairo/PathCairo.cpp
    platform/graphics/cairo/PatternCairo.cpp
    platform/graphics/cairo/PlatformContextCairo.cpp
    platform/graphics/cairo/PlatformContextCairo.h
    platform/graphics/cairo/PlatformPathCairo.cpp
    platform/graphics/cairo/PlatformPathCairo.h
    platform/graphics/cairo/RefPtrCairo.cpp
    platform/graphics/cairo/TransformationMatrixCairo.cpp
    platform/graphics/egl/GLContextEGL.cpp
    platform/graphics/harfbuzz/HarfBuzzFace.cpp
    platform/graphics/harfbuzz/HarfBuzzFaceCairo.cpp
    platform/graphics/harfbuzz/HarfBuzzShaper.cpp
    platform/graphics/manx/AudioTrackPrivateManx.cpp
    platform/graphics/manx/AudioTrackPrivateManx.h
    platform/graphics/manx/FontCacheManx.cpp
    platform/graphics/manx/FontCustomPlatformDataManx.cpp
    platform/graphics/manx/FontManx.cpp
    platform/graphics/manx/FontPlatformData.h
    platform/graphics/manx/FontPlatformDataManx.cpp
    platform/graphics/manx/GlyphPageTreeNodeManx.cpp
    platform/graphics/manx/GraphicsContext3DManx.cpp
    platform/graphics/manx/GraphicsContext3DPrivate.cpp
    platform/graphics/manx/IconManx.cpp
    platform/graphics/manx/ImageManx.cpp
    platform/graphics/manx/InbandTextTrackPrivateManx.h
    platform/graphics/manx/InbandTextTrackPrivateManx.cpp
    platform/graphics/manx/MediaDescriptionManx.cpp
    platform/graphics/manx/MediaDescriptionManx.h
    platform/graphics/manx/MediaPlayerPrivateManx.cpp
    platform/graphics/manx/MediaPlayerPrivateManx.h
    platform/graphics/manx/MediaSamplePrivateManx.cpp
    platform/graphics/manx/MediaSamplePrivateManx.h
    platform/graphics/manx/MediaSourcePrivateManx.cpp
    platform/graphics/manx/MediaSourcePrivateManx.h
    platform/graphics/manx/OpenGLManx.cpp
    platform/graphics/manx/OpenGLManx.h
    platform/graphics/manx/PlatformDisplayManx.cpp
    platform/graphics/manx/SourceBufferPrivateManx.cpp
    platform/graphics/manx/SourceBufferPrivateManx.h
    platform/graphics/manx/TileUpdateThrottle.cpp
    platform/graphics/manx/TileUpdateThrottle.h
    platform/graphics/manx/VideoLayerManx.cpp
    platform/graphics/manx/VideoLayerManx.h
    platform/graphics/manx/VideoTrackPrivateManx.cpp
    platform/graphics/manx/VideoTrackPrivateManx.h
    platform/graphics/opengl/Extensions3DOpenGLCommon.cpp
    platform/graphics/opengl/Extensions3DOpenGLES.cpp
    platform/graphics/opengl/GLPlatformContext.cpp
    platform/graphics/opengl/GLPlatformSurface.cpp
    platform/graphics/opengl/GraphicsContext3DOpenGLCommon.cpp
    platform/graphics/opengl/GraphicsContext3DOpenGLES.cpp
    platform/graphics/opengl/TemporaryOpenGLSetting.cpp
    platform/graphics/surfaces/GLTransportSurface.cpp
    platform/graphics/surfaces/GraphicsSurface.cpp
    platform/graphics/surfaces/egl/EGLConfigSelector.cpp
    platform/graphics/surfaces/egl/EGLContext.cpp
    platform/graphics/surfaces/egl/EGLHelper.cpp
    platform/graphics/surfaces/egl/EGLPbufferSurface.cpp
    platform/graphics/surfaces/egl/EGLSurface.cpp
    platform/graphics/surfaces/egl/EGLXSurface.cpp
    platform/graphics/surfaces/manx/GraphicsSurfaceOrbis.cpp
    platform/graphics/texmap/coordinated/AreaAllocator.cpp
    platform/graphics/texmap/coordinated/CompositingCoordinator.cpp
    platform/graphics/texmap/coordinated/CoordinatedGraphicsLayer.cpp
    platform/graphics/texmap/coordinated/CoordinatedImageBacking.cpp
    platform/graphics/texmap/coordinated/CoordinatedSurface.cpp
    platform/graphics/texmap/coordinated/Tile.cpp
    platform/graphics/texmap/coordinated/TiledBackingStore.cpp
    platform/graphics/texmap/coordinated/UpdateAtlas.cpp
    platform/image-decoders/ImageDecoder.cpp
    platform/image-decoders/bmp/BMPImageDecoder.cpp
    platform/image-decoders/bmp/BMPImageReader.cpp
    platform/image-decoders/cairo/ImageDecoderCairo.cpp
    platform/image-decoders/gif/GIFImageDecoder.cpp
    platform/image-decoders/gif/GIFImageReader.cpp
    platform/image-decoders/ico/ICOImageDecoder.cpp
    platform/image-decoders/jpeg/JPEGImageDecoder.cpp
    platform/image-decoders/png/PNGImageDecoder.cpp
    platform/image-decoders/webp/WEBPImageDecoder.cpp
    platform/manx/ContextMenuItemManx.cpp
    platform/manx/ContextMenuManx.cpp
    platform/manx/CursorManx.cpp
    platform/manx/DragDataManx.cpp
    platform/manx/DragImageManx.cpp
    platform/manx/EventLoopManx.cpp
    platform/manx/FileSystemManx.cpp
    platform/manx/LanguageManx.cpp
    platform/manx/LocalizedStringsManx.cpp
    platform/manx/LoggingManx.cpp
    platform/manx/MIMETypeRegistryManx.cpp
    platform/manx/PasteboardManx.cpp
    platform/manx/PlatformKeyboardEventManx.cpp
    platform/manx/PlatformMouseEventManx.cpp
    platform/manx/PlatformScreenManx.cpp
    platform/manx/PlatformTouchEventManx.cpp
    platform/manx/PlatformTouchPointManx.cpp
    platform/manx/RenderThemeManx.cpp
    platform/manx/RenderThemeManx.h
    platform/manx/ScrollbarThemeManx.cpp
    platform/manx/ScrollbarThemeManx.h
    platform/manx/SharedBufferManx.cpp
    platform/manx/SharedTimerManx.cpp
    platform/manx/SoundManx.cpp
    platform/manx/TemporaryLinkStubs.cpp
    platform/manx/WidgetManx.cpp
    platform/text/LocaleICU.cpp
    platform/text/manx/TextBreakIteratorInternalICUManx.cpp
)

if (MANX_ENABLE_SPLITPROC_JIT)
    add_definitions(-DENABLE_SPLITPROC_JIT=1)
    add_definitions(-DBUILDING_SPLITPROC_VM=1)
endif ()

if (USE_TEXTURE_MAPPER)
    list(APPEND WebCore_INCLUDE_DIRECTORIES
        platform/graphics/texmap
    )
    list(APPEND WebCore_SOURCES
        platform/graphics/texmap/BitmapTexture.cpp
        platform/graphics/texmap/BitmapTextureGL.cpp
        platform/graphics/texmap/BitmapTexturePool.cpp
        platform/graphics/texmap/GraphicsLayerTextureMapper.cpp
        platform/graphics/texmap/TextureMapperGL.cpp
        platform/graphics/texmap/TextureMapperShaderProgram.cpp
    )
endif ()

if (USE_REQUEST_ANIMATION_FRAME_DISPLAY_MONITOR)
    list(APPEND WebCore_SOURCES
        platform/graphics/DisplayRefreshMonitor.cpp
        platform/graphics/DisplayRefreshMonitorManager.cpp

        platform/graphics/manx/DisplayRefreshMonitorManx.cpp
    )
endif ()

if (ENABLE_WEBGL)
    list(APPEND WebCore_SOURCES
        html/canvas/EXTTextureFormatBGRA8888.cpp
    )
    list(APPEND WebCore_IDL_FILES
        html/canvas/EXTTextureFormatBGRA8888.idl
    )
endif ()

if (USE_CURL)
    list(APPEND WebCore_INCLUDE_DIRECTORIES
        ${WEBCORE_DIR}/platform/network/curl
    )
    list(APPEND WebCore_SOURCES
        platform/network/NetworkStorageSessionStub.cpp
        platform/network/curl/AuthenticationChallenge.h
        platform/network/curl/CookieJarCurl.cpp
        platform/network/curl/CredentialStorageCurl.cpp
        platform/network/curl/CurlCacheEntry.cpp
        platform/network/curl/CurlCacheEntry.h
        platform/network/curl/CurlCacheManager.cpp
        platform/network/curl/CurlCacheManager.h
        platform/network/curl/DNSCurl.cpp
        platform/network/curl/DownloadBundle.h
        platform/network/curl/FormDataStreamCurl.cpp
        platform/network/curl/FormDataStreamCurl.h
        platform/network/curl/MultipartHandle.cpp
        platform/network/curl/MultipartHandle.h
        platform/network/curl/ProxyServerCurl.cpp
        platform/network/curl/ResourceError.h
        platform/network/curl/ResourceHandleCurl.cpp
        platform/network/curl/ResourceHandleManager.cpp
        platform/network/curl/ResourceHandleManager.h
        platform/network/curl/ResourceRequest.h
        platform/network/curl/ResourceResponse.h
        platform/network/curl/SSLHandle.cpp
        platform/network/curl/SSLHandle.h
        platform/network/curl/SocketStreamError.h
        platform/network/curl/SocketStreamHandle.h
        platform/network/curl/SocketStreamHandleCurl.cpp
        platform/network/curl/SynchronousLoaderClientCurl.cpp
    )
elseif (USE_NTF)
    list(APPEND WebCore_INCLUDE_DIRECTORIES
        ${WEBCORE_DIR}/platform/network/ntf
    )
    list(APPEND WebCore_SOURCES
        platform/network/NetworkStorageSessionStub.cpp
        platform/network/ntf/AuthenticationChallenge.h
        platform/network/ntf/CertificateInfo.h
        platform/network/ntf/CookieJarNtf.cpp
        platform/network/ntf/CookieStorageNtf.cpp
        platform/network/ntf/CredentialBackingStoreNtf.cpp
        platform/network/ntf/CredentialBackingStoreNtf.h
        platform/network/ntf/CredentialStorageNtf.cpp
        platform/network/ntf/CredentialTransformDataNtf.cpp
        platform/network/ntf/CredentialTransformDataNtf.h
        platform/network/ntf/DNSNtf.cpp
        platform/network/ntf/NetworkStateNotifierNtf.cpp
        platform/network/ntf/ProxyServerNtf.cpp
        platform/network/ntf/ResourceError.h
        platform/network/ntf/ResourceHandleNtf.cpp
        platform/network/ntf/ResourceRequest.h
        platform/network/ntf/ResourceRequestNtf.cpp
        platform/network/ntf/ResourceResponse.h
        platform/network/ntf/SocketStreamError.h
        platform/network/ntf/SocketStreamHandle.h
        platform/network/ntf/SocketStreamHandleNtf.cpp
        platform/network/ntf/SocketStreamHandleNtf.h
        platform/network/ntf/SynchronousLoaderClientNtf.cpp
        platform/network/ntf/URLRequestData.cpp
        platform/network/ntf/URLRequestData.h
        platform/network/ntf/URLRequestFile.cpp
        platform/network/ntf/URLRequestFile.h
        platform/network/ntf/URLRequestHttp.cpp
        platform/network/ntf/URLRequestHttp.h
        platform/network/ntf/URLRequestJobFactory.cpp
        platform/network/ntf/URLRequestJobFactory.h
        platform/network/ntf/URLRequestListenerNtf.cpp
        platform/network/ntf/URLRequestListenerNtf.h
        platform/network/ntf/URLRequestUndefined.cpp
        platform/network/ntf/URLRequestUndefined.h
    )
endif ()

list(APPEND WebCore_USER_AGENT_STYLE_SHEETS
    ${WEBCORE_DIR}/Modules/mediacontrols/mediaControlsManx.css
)

set(WebCore_USER_AGENT_SCRIPTS
    ${WEBCORE_DIR}/English.lproj/mediaControlsLocalizedStrings.js
    ${WEBCORE_DIR}/Modules/mediacontrols/mediaControlsManx.js
)

list(APPEND WebCore_LIBRARIES
    ${CAIRO_LIBRARIES}
    ${EGL_LIBRARIES}
    ${FREETYPE2_LIBRARIES}
    ${HARFBUZZ_LIBRARIES}
    ${JPEG_LIBRARIES}
    ${LIBXML2_LIBRARIES}
    ${PIXMAN_LIBRARIES}
    ${PNG_LIBRARIES}
    ${SQLITE_LIBRARIES}
    ${ZLIB_LIBRARIES}
    orbis_sdk::md5
    orbis_sdk::shader_binary
)

list(APPEND WebCore_SYSTEM_INCLUDE_DIRECTORIES
    ${CAIRO_INCLUDE_DIRS}
    ${EGL_INCLUDE_DIRS}
    ${FREETYPE2_INCLUDE_DIRS}
    ${HARFBUZZ_INCLUDE_DIRS}
    ${JPEG_INCLUDE_DIRS}
    ${LIBXML2_INCLUDE_DIR}
    ${PNG_INCLUDE_DIR}
    ${SQLITE_INCLUDE_DIR}
    ${ZLIB_INCLUDE_DIRS}
)

if (USE_CURL)
    list(APPEND WebCore_LIBRARIES
        ${CURL_LIBRARIES}
        ${OPENSSL_LIBRARIES}
    )
    list(APPEND WebCore_SYSTEM_INCLUDE_DIRECTORIES
        ${CURL_INCLUDE_DIRS}
    )
endif ()

list(APPEND WebCore_LIBRARIES
    ${PRECOMPILED_SHADERS_LIBRARIES}
)

set(WebCore_USER_AGENT_SCRIPTS_DEPENDENCIES ${WEBCORE_DIR}/platform/manx/RenderThemeManx.cpp)
