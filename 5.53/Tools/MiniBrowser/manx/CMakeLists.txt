webkit_add_executable(MiniBrowser
    Browser.cpp
    Browser.h
    WKRetainPtr.h
    WebContext.cpp
    WebContext.h
    WebView.cpp
    WebView.h
    main.cpp
    utils.cpp
    utils.h
    )

target_link_libraries(
    MiniBrowser
    WebKit2
    ${SOUTHWEST_LIBRARIES}
    )

target_include_directories(
    MiniBrowser PRIVATE
    ${PROJECT_SOURCE_DIR}/Source
    ${DERIVED_SOURCES_WEBKIT2_DIR}/include
    ${SOUTHWEST_INCLUDE_DIRS}
    .
    )
