if (ORBIS)
    # Set some variables to find packages
    list(APPEND CMAKE_INCLUDE_PATH
        ${WEBKIT_MANX_LIBRARIES}/include
        ${WEBKIT_MANX_LIBRARIES}/include/cairo
        ${WEBKIT_MANX_LIBRARIES}/include/curl
        ${WEBKIT_MANX_LIBRARIES}/include/freetype2
        ${WEBKIT_MANX_LIBRARIES}/include/harfbuzz
        ${WEBKIT_MANX_LIBRARIES}/include/libpng15
        ${WEBKIT_MANX_LIBRARIES}/include/libxml2
        ${WEBKIT_MANX_LIBRARIES}/include/pixman-1
        ${PIGLET_PROJECT_INCLUDE_PATH}
        ${sce_orbis_sdk_dir}/../internal/include
    )
    # Piglet path is not necessary because
    # it is manually specified in manx/cmake/ImportPigletMSProject.cmake
    list(APPEND CMAKE_LIBRARY_PATH
        ${WEBKIT_MANX_LIBRARIES}/lib
        ${sce_orbis_sdk_dir}/../internal/target/lib
    )
endif ()
