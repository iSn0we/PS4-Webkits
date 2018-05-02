add_custom_target(TestWebKitAPI-forwarding-headers
    COMMAND ${PERL_EXECUTABLE} ${WEBKIT2_DIR}/Scripts/generate-forwarding-headers.pl
        --include-path ${TESTWEBKITAPI_DIR}
        --output ${DERIVED_SOURCES_WEBKIT2_DIR}/include
        --platform manx
    DEPENDS WebKit2-forwarding-headers
)

set(ForwardingHeadersForTestWebKitAPI_NAME TestWebKitAPI-forwarding-headers)

include_directories(
    ${DERIVED_SOURCES_WEBKIT2_DIR}/include
)

set(test_main_SOURCES
    ${TESTWEBKITAPI_DIR}/manx/main.cpp
)

set(bundle_harness_SOURCES
    manx/InjectedBundleControllerManx.cpp
    manx/PlatformUtilitiesManx.cpp
)

list(APPEND bundle_harness_SOURCES
    ${TESTWEBKITAPI_DIR}/Tests/WebKit2/manx/WKBundleFrameConvertMonotonicTimeToDocumentTime_Bundle.cpp
    ${TESTWEBKITAPI_DIR}/Tests/WebKit2/manx/WKBundleGetJavaScriptHeapStatistics_Bundle.cpp
    ${TESTWEBKITAPI_DIR}/Tests/WebKit2/manx/WKBundleGetFastMallocStatistics_Bundle.cpp
    ${TESTWEBKITAPI_DIR}/Tests/WebKit2/manx/WKBundleGetMemoryCacheStatistics_Bundle.cpp
    ${TESTWEBKITAPI_DIR}/Tests/WebKit2/manx/WKBundleSetGarbageCollectClient_Bundle.cpp
)

link_libraries(
    ${LIBMANX_LIBRARIES}
)

include_directories(
    ${DERIVED_SOURCES_DIR}/ForwardingHeaders
    ${DERIVED_SOURCES_DIR}/ForwardingHeaders/JavaScriptCore
    ${LIBMANX_INCLUDE_DIRS}
    ${CAIRO_INCLUDE_DIRS}
)

webkit_add_executable(TestWebCore
    ${test_main_SOURCES}
    ${TESTWEBKITAPI_DIR}/TestsController.cpp
    ${TESTWEBKITAPI_DIR}/Tests/WebCore/CalculationValue
    #${TESTWEBKITAPI_DIR}/Tests/WebCore/ContentExtensions
    ${TESTWEBKITAPI_DIR}/Tests/WebCore/CSSParser
    #${TESTWEBKITAPI_DIR}/Tests/WebCore/DFACombiner
    #${TESTWEBKITAPI_DIR}/Tests/WebCore/DFAMinimizer
    #${TESTWEBKITAPI_DIR}/Tests/WebCore/FileSystem.cpp
    ${TESTWEBKITAPI_DIR}/Tests/WebCore/LayoutUnit.cpp
    ${TESTWEBKITAPI_DIR}/Tests/WebCore/PublicSuffix.cpp
    #${TESTWEBKITAPI_DIR}/Tests/WebCore/SharedBuffer.cpp
    ${TESTWEBKITAPI_DIR}/Tests/WebCore/TimeRanges.cpp
    ${TESTWEBKITAPI_DIR}/Tests/WebCore/TransformationMatrix.cpp
    ${TESTWEBKITAPI_DIR}/Tests/WebCore/URL.cpp
)

if (MANX_ENABLE_SPLITPROC_JIT)
    add_definitions(-DENABLE_SPLITPROC_JIT=1)
    add_definitions(-DBUILDING_SPLITPROC_VM=1)

    list(APPEND test_wtf_LIBRARIES JscBridge_vm SceJitBridge JavaScriptCore)

    list(APPEND TestJavaScriptCore_LIBRARIES JscBridge_vm SceJitBridge)

    list(APPEND test_webcore_LIBRARIES JscBridge_vm SceJitBridge)

    list(APPEND test_webkit2_api_LIBRARIES SceJitBridge)

    list(APPEND TestWebKitAPI_LIBRARIES SceJitBridge)
endif ()

target_link_libraries(TestWebCore ${test_webcore_LIBRARIES})
add_dependencies(TestWebCore ${ForwardingHeadersForTestWebKitAPI_NAME})

set(TestWebKit2_tests
    AboutBlankLoad
    CanHandleRequest
    #CloseFromWithinCreatePage
    CloseThenTerminate
    CookieManager
    DOMWindowExtensionBasic
    DOMWindowExtensionNoCache
    DidAssociateFormControls
    #DidNotHandleKeyDown
    DocumentStartUserScriptAlertCrash
    #DownloadDecideDestinationCrash
    #EphemeralSessionPushStateNoHistoryCallback
    EvaluateJavaScript
    FailedLoad
    Find
    #ForceRepaint
    FrameMIMETypeHTML
    FrameMIMETypePNG
    #Geolocation
    GetInjectedBundleInitializationUserDataCallback
    HitTestResultNodeHandle
    InjectedBundleBasic
    InjectedBundleFrameHitTest
    InjectedBundleInitializationUserDataCallbackWins
    LayoutMilestonesWithAllContentInFrame
    LoadAlternateHTMLStringWithNonDirectoryURL
    LoadCanceledNoServerRedirectCallback
    LoadPageOnCrash
    #MenuTypesForMouseEvents
    #ModalAlertsSPI
    MouseMoveAfterCrash
    NewFirstVisuallyNonEmptyLayout
    #NewFirstVisuallyNonEmptyLayoutFails
    NewFirstVisuallyNonEmptyLayoutForImages
    #NewFirstVisuallyNonEmptyLayoutFrames
    PageLoadBasic
    PageLoadDidChangeLocationWithinPageForFrame
    ParentFrame
    PreventEmptyUserAgent
    PrivateBrowsingPushStateNoHistoryCallback
    ReloadPageAfterCrash
    ResizeReversePaginatedWebView
    ResizeWindowAfterCrash
    ResponsivenessTimerDoesntFireEarly
    #RestoreSessionStateContainingFormData
    ScrollPinningBehaviors
    #SeccompFilters
    ShouldGoToBackForwardListItem
    ShouldKeepCurrentBackForwardListItemInList
    #SpacebarScrolling
    #StopLoadingDuringDidFailProvisionalLoad
    TerminateTwice
    TextFieldDidBeginAndEndEditing
    UserMedia
    UserMessage
    #WKBundleFileHandle
    #WKImageCreateCGImageCrash
    #WKPageConfiguration
    WKPageCopySessionStateWithFiltering
    WKPageGetScaleFactorNotZero
    #WKPageIsPlayingAudio
    WKPreferences
    WKString
    WKStringJSString
    WKURL
    #WebArchive
    WebCoreStatisticsWithNoWebProcess
    #WillLoad
    WillSendSubmitEvent
    manx/ContextMenu
    manx/Cursor
    manx/FullScreen
    manx/JavascriptAlert
    manx/JavascriptConfirm
    manx/JavascriptPrompt
    manx/PopupMenu
    manx/ScalePageKeepingScrollOffset
    manx/TitleFormat
    manx/WKBundleFrameConvertMonotonicTimeToDocumentTime
    manx/WKBundleGetFastMallocStatistics
    manx/WKBundleGetJavaScriptHeapStatistics
    manx/WKBundleGetMemoryCacheStatistics
    manx/WKBundleSetGarbageCollectClient
    manx/WKKeyValueStorageManagerDeleteAllEntries
    manx/WKPageRunJavaScriptInMainFrameAndReturnString
    )

foreach(i IN LISTS TestWebKit2_tests)
    list(APPEND TestWebKit2_src Tests/WebKit2/${i}.cpp)
endforeach()

webkit_add_executable(TestWebKit2 ${TestWebKit2_src})
target_link_libraries(TestWebKit2 ${test_webkit2_api_LIBRARIES})
target_compile_definitions(TestWebKit2 PRIVATE "TEST_WEBKIT2_RESOURCES_DIR=\"${TESTWEBKITAPI_DIR}/Tests/WebKit2\"")

set(webkit2_api_harness_SOURCES
    manx/PlatformUtilitiesManx.cpp
    manx/PlatformWebView.cpp
)
