if (USE_CURL)
    set(manx_gfh_opt --platform curl)
elseif (USE_NTF)
    set(manx_gfh_opt --platform ntf)
endif ()

add_custom_target(forwarding-headersForWebKitTestRunner
    COMMAND ${PERL_EXECUTABLE} ${WEBKIT2_DIR}/Scripts/generate-forwarding-headers.pl
        --include-path ${WEBKIT_TESTRUNNER_DIR} --output ${DERIVED_SOURCES_WEBKIT2_DIR}/include
        --platform manx ${manx_gfh_opt}
)
set(ForwardingHeadersForWebKitTestRunner_NAME forwarding-headersForWebKitTestRunner)

list(APPEND WebKitTestRunner_SOURCES
    ${WEBKIT_TESTRUNNER_DIR}/cairo/TestInvocationCairo.cpp

    ${WEBKIT_TESTRUNNER_DIR}/manx/EventSenderProxyManx.cpp
    ${WEBKIT_TESTRUNNER_DIR}/manx/PlatformWebViewManx.cpp
    ${WEBKIT_TESTRUNNER_DIR}/manx/TestControllerManx.cpp
    ${WEBKIT_TESTRUNNER_DIR}/manx/main.cpp
)

list(APPEND WebKitTestRunner_INCLUDE_DIRECTORIES
    ${DERIVED_SOURCES_WEBKIT2_DIR}/include
    ${SOUTHWEST_INCLUDE_DIRS}
)

list(APPEND WebKitTestRunner_SYSTEM_INCLUDE_DIRECTORIES
    ${CAIRO_INCLUDE_DIRS}
)

list(APPEND WebKitTestRunner_LIBRARIES
    ${CAIRO_LIBRARIES}
    ${SOUTHWEST_LIBRARIES}
)

list(APPEND WebKitTestRunnerInjectedBundle_SOURCES
    ${WEBKIT_TESTRUNNER_INJECTEDBUNDLE_DIR}/manx/ActivateFontsManx.cpp
    ${WEBKIT_TESTRUNNER_INJECTEDBUNDLE_DIR}/manx/InjectedBundleManx.cpp
    ${WEBKIT_TESTRUNNER_INJECTEDBUNDLE_DIR}/manx/TestRunnerManx.cpp
)
