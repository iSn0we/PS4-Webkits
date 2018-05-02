if (NOT ORBIS)
    message(FATAL_ERROR "Unknown OS '${CMAKE_SYSTEM_NAME}'")
endif ()

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set_and_expose_to_build(USE_NTF ON)

find_package(Cairo 1.10.2 REQUIRED)
find_package(Pixman REQUIRED)
find_package(Freetype2 2.4.2 REQUIRED)
find_package(HarfBuzz 0.9.2 REQUIRED)
find_package(ICU REQUIRED)
find_package(JPEG REQUIRED)
find_package(LibXml2 2.7.8 REQUIRED)
find_package(PNG REQUIRED)
find_package(Sqlite REQUIRED)
find_package(ZLIB REQUIRED)
find_package(Libmanx REQUIRED)
find_package(PrecompiledShaders REQUIRED)

if (USE_CURL)
    find_package(curl REQUIRED)
    find_package(OpenSSL REQUIRED)
endif ()

find_library(HARFBUZZ_ICU_LIBRARIES NAMES harfbuzz-icu)
if (HARFBUZZ_ICU_LIBRARIES)
    list(APPEND HARFBUZZ_LIBRARIES ${HARFBUZZ_ICU_LIBRARIES})
endif ()

if (ORBIS)
    if (NOT MANX_JSCORE_ONLY)
        set(EGL_NAMES ScePigletv2VSH_stub_weak)
        find_package(EGL REQUIRED)
    endif ()

    find_library(ICU_DATA_LIBRARIES NAMES icudata)
    mark_as_advanced(ICU_DATA_LIBRARIES)
    list(APPEND ICU_LIBRARIES ${ICU_DATA_LIBRARIES})

    # The command should be quoted in MSWin32. See WebCore/bindings/scripts/preprocessor.pm.
    set(CODE_GENERATOR_PREPROCESSOR "\"${orbis_clang_executable}\" -E -P -x c++")
    set(CODE_GENERATOR_PREPROCESSOR_WITH_LINEMARKERS "\"${orbis_clang_executable}\" -E -x c++")

    if (MANX_ORBIS_USE_PUBLIC_SDK)
        set(sce_sdk_libraries -lScePosix_stub_weak)
    else ()
        set(sce_sdk_include_dirs
            ${sce_orbis_sdk_dir}/../internal/include
        )
        set(sce_sdk_libraries
            -lc_stub_weak
            -lkernel_stub_05_weak
        )
        set(sce_sdk_library_dirs
            ${sce_orbis_sdk_dir}/../internal/target/lib
        )
    endif ()
endif ()

add_definitions(-DWTF_PLATFORM_MANX)
if (MANX_ENABLE_JITBRIDGE_DEBUG_ARENA)
    add_definitions(-DENABLE_JITBRIDGE_DEBUG_ARENA=1)
endif ()

set_and_expose_to_build(USE_COORDINATED_GRAPHICS ON)
set_and_expose_to_build(USE_COORDINATED_GRAPHICS_MULTIPROCESS ON)
set_and_expose_to_build(ENABLE_GRAPHICS_CONTEXT_3D ON)
if (NOT MANX_JSCORE_ONLY)
    set_and_expose_to_build(USE_EGL ON)
endif ()
set_and_expose_to_build(USE_GRAPHICS_SURFACE ON)
set_and_expose_to_build(USE_OPENGL ON)
set_and_expose_to_build(USE_OPENGL_ES_2 ON)
set_and_expose_to_build(USE_TEXMAP_OPENGL_ES_2 ON)
set_and_expose_to_build(USE_TEXTURE_MAPPER ON)
set_and_expose_to_build(USE_TEXTURE_MAPPER_CAIRO ON)
set_and_expose_to_build(USE_TEXTURE_MAPPER_GL ON)

set_and_expose_to_build(USE_REQUEST_ANIMATION_FRAME_DISPLAY_MONITOR ON)

set_and_expose_to_build(ENABLE_MANX_PLAYSTATION_INSPECTOR_DOMAIN ON)

set_and_expose_to_build(ENABLE_MANX_INPUT_INSPECTOR_DOMAIN ON)

add_compile_options(
    -fPIC
    -fstack-protector-strong
)

include_directories(
    ${sce_sdk_include_dirs}
    ${LIBMANX_INCLUDE_DIRS}
)

link_directories(
    ${sce_sdk_library_dirs}
)

if (SHARED_CORE)
    set_and_expose_to_build(USE_EXPORT_MACROS ON)
else ()
    set(JavaScriptCore_LIBRARY_TYPE STATIC)
endif ()

set(ENABLE_WEBKIT OFF)
set(ENABLE_WEBKIT2 ON)
set(ENABLE_WEBCORE ON)

webkit_option_begin()

webkit_option_default_port_value(ENABLE_3D_TRANSFORMS PRIVATE ON)
webkit_option_default_port_value(ENABLE_ACCELERATED_2D_CANVAS PRIVATE OFF)
webkit_option_default_port_value(ENABLE_ACCELERATED_OVERFLOW_SCROLLING PRIVATE OFF)
webkit_option_default_port_value(ENABLE_API_TESTS PUBLIC ON)
webkit_option_default_port_value(ENABLE_ASSEMBLER_WX_EXCLUSIVE PRIVATE OFF)
webkit_option_default_port_value(ENABLE_ATTACHMENT_ELEMENT PRIVATE OFF)
webkit_option_default_port_value(ENABLE_AVF_CAPTIONS PRIVATE OFF)
webkit_option_default_port_value(ENABLE_BATTERY_STATUS PRIVATE OFF)
webkit_option_default_port_value(ENABLE_CACHE_PARTITIONING PRIVATE OFF)
webkit_option_default_port_value(ENABLE_CANVAS_PATH PRIVATE ON)
webkit_option_default_port_value(ENABLE_CANVAS_PROXY PRIVATE OFF)
webkit_option_default_port_value(ENABLE_CHANNEL_MESSAGING PRIVATE ON)
webkit_option_default_port_value(ENABLE_CONTENT_FILTERING PRIVATE OFF)
webkit_option_default_port_value(ENABLE_CONTEXT_MENUS PRIVATE ON)
webkit_option_default_port_value(ENABLE_CREDENTIAL_STORAGE PRIVATE OFF)
webkit_option_default_port_value(ENABLE_CSP_NEXT PRIVATE OFF)
webkit_option_default_port_value(ENABLE_CSS3_TEXT PRIVATE OFF)
webkit_option_default_port_value(ENABLE_CSS3_TEXT_LINE_BREAK PRIVATE OFF)
webkit_option_default_port_value(ENABLE_CSS_BOX_DECORATION_BREAK PRIVATE ON)
webkit_option_default_port_value(ENABLE_CSS_COMPOSITING PRIVATE OFF)
webkit_option_default_port_value(ENABLE_CSS_DEVICE_ADAPTATION PRIVATE OFF)
webkit_option_default_port_value(ENABLE_CSS_GRID_LAYOUT PRIVATE OFF)
webkit_option_default_port_value(ENABLE_CSS_IMAGE_ORIENTATION PRIVATE OFF)
webkit_option_default_port_value(ENABLE_CSS_IMAGE_RESOLUTION PRIVATE OFF)
webkit_option_default_port_value(ENABLE_CSS_IMAGE_SET PRIVATE OFF)
webkit_option_default_port_value(ENABLE_CSS_REGIONS PRIVATE OFF)
webkit_option_default_port_value(ENABLE_CSS_SCROLL_SNAP PRIVATE OFF)
webkit_option_default_port_value(ENABLE_CSS_SELECTORS_LEVEL4 PRIVATE OFF)
webkit_option_default_port_value(ENABLE_CSS_SHAPES PRIVATE OFF)
webkit_option_default_port_value(ENABLE_CURSOR_VISIBILITY PRIVATE OFF)
webkit_option_default_port_value(ENABLE_CUSTOM_SCHEME_HANDLER PRIVATE OFF)
webkit_option_default_port_value(ENABLE_DASHBOARD_SUPPORT PRIVATE OFF)
webkit_option_default_port_value(ENABLE_DATABASE_PROCESS PRIVATE OFF)
webkit_option_default_port_value(ENABLE_DATACUE_VALUE PRIVATE OFF)
webkit_option_default_port_value(ENABLE_DATALIST_ELEMENT PRIVATE OFF)
webkit_option_default_port_value(ENABLE_DATA_TRANSFER_ITEMS PRIVATE OFF)
webkit_option_default_port_value(ENABLE_DETAILS_ELEMENT PRIVATE ON)
webkit_option_default_port_value(ENABLE_DEVICE_ORIENTATION PRIVATE OFF)
webkit_option_default_port_value(ENABLE_DFG_JIT PRIVATE OFF)
webkit_option_default_port_value(ENABLE_DNT PRIVATE ON)
webkit_option_default_port_value(ENABLE_DOM4_EVENTS_CONSTRUCTOR PRIVATE OFF)
webkit_option_default_port_value(ENABLE_DOWNLOAD_ATTRIBUTE PRIVATE OFF)
webkit_option_default_port_value(ENABLE_DRAG_SUPPORT PRIVATE OFF)
webkit_option_default_port_value(ENABLE_ENCRYPTED_MEDIA PRIVATE OFF)
webkit_option_default_port_value(ENABLE_ENCRYPTED_MEDIA_V2 PRIVATE OFF)
webkit_option_default_port_value(ENABLE_ES6_ARROWFUNCTION_SYNTAX PRIVATE ON)
webkit_option_default_port_value(ENABLE_ES6_CLASS_SYNTAX PRIVATE ON)
webkit_option_default_port_value(ENABLE_ES6_TEMPLATE_LITERAL_SYNTAX PRIVATE ON)
webkit_option_default_port_value(ENABLE_FILTERS_LEVEL_2 PRIVATE OFF)
webkit_option_default_port_value(ENABLE_FONT_LOAD_EVENTS PRIVATE OFF)
webkit_option_default_port_value(ENABLE_FTL_JIT PRIVATE OFF)
webkit_option_default_port_value(ENABLE_FTL_NATIVE_CALL_INLINING PRIVATE OFF)
webkit_option_default_port_value(ENABLE_FTPDIR PRIVATE OFF)
webkit_option_default_port_value(ENABLE_FULLSCREEN_API PRIVATE ON)
webkit_option_default_port_value(ENABLE_GAMEPAD PRIVATE OFF)
webkit_option_default_port_value(ENABLE_GAMEPAD_DEPRECATED PRIVATE OFF)
webkit_option_default_port_value(ENABLE_GEOLOCATION PRIVATE OFF)
webkit_option_default_port_value(ENABLE_HIDDEN_PAGE_DOM_TIMER_THROTTLING PRIVATE ON)
webkit_option_default_port_value(ENABLE_ICONDATABASE PRIVATE OFF)
webkit_option_default_port_value(ENABLE_IMAGE_DECODER_DOWN_SAMPLING PRIVATE OFF)
webkit_option_default_port_value(ENABLE_INDEXED_DATABASE PRIVATE OFF)
webkit_option_default_port_value(ENABLE_INDEXED_DATABASE_IN_WORKERS PRIVATE OFF)
webkit_option_default_port_value(ENABLE_INDIE_UI PRIVATE OFF)
webkit_option_default_port_value(ENABLE_INPUT_TYPE_COLOR PRIVATE OFF)
webkit_option_default_port_value(ENABLE_INPUT_TYPE_COLOR_POPOVER PRIVATE OFF)
webkit_option_default_port_value(ENABLE_INPUT_TYPE_DATE PRIVATE OFF)
webkit_option_default_port_value(ENABLE_INPUT_TYPE_DATETIMELOCAL PRIVATE OFF)
webkit_option_default_port_value(ENABLE_INPUT_TYPE_DATETIME_INCOMPLETE PRIVATE OFF)
webkit_option_default_port_value(ENABLE_INPUT_TYPE_MONTH PRIVATE OFF)
webkit_option_default_port_value(ENABLE_INPUT_TYPE_TIME PRIVATE OFF)
webkit_option_default_port_value(ENABLE_INPUT_TYPE_WEEK PRIVATE OFF)
webkit_option_default_port_value(ENABLE_INTL PRIVATE OFF)
webkit_option_default_port_value(ENABLE_IOS_AIRPLAY PRIVATE OFF)
webkit_option_default_port_value(ENABLE_IOS_TEXT_AUTOSIZING PRIVATE OFF)
webkit_option_default_port_value(ENABLE_JIT PRIVATE OFF)
webkit_option_default_port_value(ENABLE_LEGACY_CSS_VENDOR_PREFIXES PRIVATE OFF)
webkit_option_default_port_value(ENABLE_LEGACY_NOTIFICATIONS PRIVATE OFF)
webkit_option_default_port_value(ENABLE_LEGACY_VENDOR_PREFIXES PRIVATE ON)
webkit_option_default_port_value(ENABLE_LEGACY_WEB_AUDIO PRIVATE OFF)
webkit_option_default_port_value(ENABLE_LETTERPRESS PRIVATE OFF)
webkit_option_default_port_value(ENABLE_LINK_PREFETCH PRIVATE OFF)
webkit_option_default_port_value(ENABLE_MAC_LONG_PRESS PRIVATE OFF)
webkit_option_default_port_value(ENABLE_MANX_APPCACHE_IGNORE_QUERY PRIVATE OFF)
webkit_option_default_port_value(ENABLE_MANX_ASYNC_IMAGE_DECODER PRIVATE OFF)
webkit_option_default_port_value(ENABLE_MANX_CURSOR_NAVIGATION PRIVATE ON)
webkit_option_default_port_value(ENABLE_MANX_HTMLTILE PRIVATE ON)
webkit_option_default_port_value(ENABLE_MATHML PRIVATE OFF)
webkit_option_default_port_value(ENABLE_MEDIA_CAPTURE PRIVATE OFF)
webkit_option_default_port_value(ENABLE_MEDIA_CONTROLS_SCRIPT PRIVATE ON)
webkit_option_default_port_value(ENABLE_MEDIA_SOURCE PRIVATE OFF)
webkit_option_default_port_value(ENABLE_MEDIA_STATISTICS PRIVATE OFF)
webkit_option_default_port_value(ENABLE_MEDIA_STREAM PRIVATE OFF)
webkit_option_default_port_value(ENABLE_MEMORY_SAMPLER PRIVATE OFF)
webkit_option_default_port_value(ENABLE_METER_ELEMENT PRIVATE OFF)
webkit_option_default_port_value(ENABLE_MHTML PRIVATE OFF)
webkit_option_default_port_value(ENABLE_MINIBROWSER PUBLIC ON)
webkit_option_default_port_value(ENABLE_MOUSE_CURSOR_SCALE PRIVATE OFF)
webkit_option_default_port_value(ENABLE_NAVIGATOR_CONTENT_UTILS PRIVATE OFF)
webkit_option_default_port_value(ENABLE_NAVIGATOR_HWCONCURRENCY PRIVATE OFF)
webkit_option_default_port_value(ENABLE_NETSCAPE_PLUGIN_API PRIVATE OFF)
webkit_option_default_port_value(ENABLE_NETWORK_PROCESS PRIVATE OFF)
webkit_option_default_port_value(ENABLE_NOSNIFF PRIVATE OFF)
webkit_option_default_port_value(ENABLE_NOTIFICATIONS PRIVATE OFF)
webkit_option_default_port_value(ENABLE_ORIENTATION_EVENTS PRIVATE OFF)
webkit_option_default_port_value(ENABLE_PDFKIT_PLUGIN PRIVATE OFF)
webkit_option_default_port_value(ENABLE_PERFORMANCE_TIMELINE PRIVATE ON)
webkit_option_default_port_value(ENABLE_PICTURE_SIZES PRIVATE OFF)
webkit_option_default_port_value(ENABLE_POINTER_LOCK PRIVATE OFF)
webkit_option_default_port_value(ENABLE_PROXIMITY_EVENTS PRIVATE OFF)
webkit_option_default_port_value(ENABLE_PUBLIC_SUFFIX_LIST PRIVATE OFF)
webkit_option_default_port_value(ENABLE_QUOTA PRIVATE OFF)
webkit_option_default_port_value(ENABLE_REMOTE_INSPECTOR PRIVATE OFF)
webkit_option_default_port_value(ENABLE_REQUEST_ANIMATION_FRAME PRIVATE ON)
webkit_option_default_port_value(ENABLE_RESOLUTION_MEDIA_QUERY PRIVATE OFF)
webkit_option_default_port_value(ENABLE_RESOURCE_TIMING PRIVATE ON)
webkit_option_default_port_value(ENABLE_SATURATED_LAYOUT_ARITHMETIC PRIVATE ON)
webkit_option_default_port_value(ENABLE_SECCOMP_FILTERS PRIVATE OFF)
webkit_option_default_port_value(ENABLE_SERVICE_CONTROLS PRIVATE OFF)
webkit_option_default_port_value(ENABLE_SMOOTH_SCROLLING PRIVATE OFF)
webkit_option_default_port_value(ENABLE_SPEECH_SYNTHESIS PRIVATE OFF)
webkit_option_default_port_value(ENABLE_SPELLCHECK PRIVATE OFF)
webkit_option_default_port_value(ENABLE_STREAMS_API PRIVATE OFF)
webkit_option_default_port_value(ENABLE_SUBTLE_CRYPTO PRIVATE OFF)
webkit_option_default_port_value(ENABLE_SVG_FONTS PRIVATE OFF)
webkit_option_default_port_value(ENABLE_TELEPHONE_NUMBER_DETECTION PRIVATE OFF)
webkit_option_default_port_value(ENABLE_TEMPLATE_ELEMENT PRIVATE ON)
webkit_option_default_port_value(ENABLE_TEXT_AUTOSIZING PRIVATE OFF)
webkit_option_default_port_value(ENABLE_THREADED_COMPOSITOR PRIVATE OFF)
webkit_option_default_port_value(ENABLE_TOUCH_EVENTS PRIVATE OFF)
webkit_option_default_port_value(ENABLE_TOUCH_ICON_LOADING PRIVATE OFF)
webkit_option_default_port_value(ENABLE_TOUCH_SLIDER PRIVATE OFF)
webkit_option_default_port_value(ENABLE_USERSELECT_ALL PRIVATE OFF)
webkit_option_default_port_value(ENABLE_USER_MESSAGE_HANDLERS PRIVATE OFF)
webkit_option_default_port_value(ENABLE_USER_TIMING PRIVATE ON)
webkit_option_default_port_value(ENABLE_VIBRATION PRIVATE OFF)
webkit_option_default_port_value(ENABLE_VIDEO PRIVATE ON)
webkit_option_default_port_value(ENABLE_VIDEO_TRACK PRIVATE ON)
webkit_option_default_port_value(ENABLE_VIEW_MODE_CSS_MEDIA PRIVATE ON)
webkit_option_default_port_value(ENABLE_WEBGL PRIVATE OFF)
webkit_option_default_port_value(ENABLE_WEBVTT_REGIONS PRIVATE OFF)
webkit_option_default_port_value(ENABLE_WEB_AUDIO PRIVATE OFF)
webkit_option_default_port_value(ENABLE_WEB_REPLAY PRIVATE OFF)
webkit_option_default_port_value(ENABLE_WEB_SOCKETS PRIVATE ON)
webkit_option_default_port_value(ENABLE_WEB_TIMING PRIVATE ON)
webkit_option_default_port_value(ENABLE_XHR_TIMEOUT PRIVATE ON)
webkit_option_default_port_value(ENABLE_XSLT PRIVATE OFF)

# Manx-specific options
webkit_option_define(ENABLE_MANX_ADVANCED_ACCELERATED_COMPOSITING "Toggle Advanced Accelerated Compositing Support." PRIVATE ON)
webkit_option_define(ENABLE_MANX_INDIRECT_COMPOSITING "Toggle Indirect Compositing Support." PRIVATE ON)
webkit_option_define(ENABLE_MANX_TEXMAP_LAZY_TEXTURE_ALLOCATION "Toggle Lazy Texture Allocation Support." PRIVATE ON)
webkit_option_define(ENABLE_MANX_JSC_DEBUGGER "Toggle JSC Remote Debugger Support." PRIVATE OFF)
webkit_option_define(ENABLE_MANX_ADVANCED_TIMING_API "Toggle Advanced Timing API Support." PRIVATE ON)

webkit_option_end()

if (ORBIS)
    function(webkit_add_executable name)
        set(emd_file ${CMAKE_CURRENT_BINARY_DIR}/${name}.emd)
        set(pemd_file ${CMAKE_CURRENT_BINARY_DIR}/${name}.pemd)
        string(CONCAT emd_content
            "Module: ${name} major_version: 1 minor_version: 1\n"
            "Library: ${name} {\n"
            "    version: 1\n"
            "    attr: {\n"
            "            auto_export\n"
            "            loose_import\n"
            "    }\n"
            "    nidsuffix: \"NID Suffix of ${name}\"\n"
            "    function: {\n"
            "            main\n"
            "    }\n"
            "}\n"
        )
        add_library(${name} SHARED ${ARGN} ${pemd_file})
        set_target_properties(
            ${name} PROPERTIES
            PREFIX ""
        )
        file(GENERATE OUTPUT ${emd_file}
            CONTENT "${emd_content}"
        )
        add_custom_command(OUTPUT ${pemd_file}
            DEPENDS ${emd_file}
            COMMAND ${orbis_clang_executable} -x c++ -o ${pemd_file} -E ${emd_file}
        )
        target_link_libraries(${name} ${pemd_file})
    endfunction()
endif ()

if (MANX_JSCORE_ONLY)
    set(ENABLE_WEBCORE OFF)
    set(ENABLE_WEBKIT OFF)
    set(ENABLE_WEBKIT2 OFF)
    set(ENABLE_MINIBROWSER OFF)
    set(ENABLE_API_TESTS OFF)
    set(ENABLE_MANX_JSC_DEBUGGER ON)
endif ()
if (MANX_ENABLE_JIT)
    set(ENABLE_JIT ON)
endif ()

