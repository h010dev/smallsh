# Install CMocka testing framework
# Source: https://github.com/kylemanna/cmocka-init/blob/master/cmocka.cmake
include(ExternalProject)

ExternalProject_Add(
        cmocka_ep
        GIT_REPOSITORY https://gitlab.com/cmocka/cmocka.git
        GIT_TAG cmocka-1.1.1
        CMAKE_ARGS
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DCMAKE_SYSTEM_NAME=${CMAKE_SYSTEM_NAME}
        -DWITH_STATIC_LIB=ON
        -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG:PATH=Debug
        -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE:PATH=Release

        # Additional args
        -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
        -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}
        -DWITH_CMOCKERY_SUPPORT=OFF
        -DWITH_EXAMPLES=OFF
        -DUNIT_TESTING=OFF
        -DPICKY_DEVELOPER=OFF
        BUILD_COMMAND $(MAKE) cmocka_static
        INSTALL_COMMAND ""
)

add_library(cmocka STATIC IMPORTED GLOBAL)
ExternalProject_Get_Property(cmocka_ep binary_dir)

set_property(TARGET cmocka PROPERTY IMPORTED_LOCATION
        "${binary_dir}/src/libcmocka.a")
set_property(TARGET cmocka PROPERTY IMPORTED_LOCATION_DEBUG
        "${binary_dir}/src/Debug/libcmocka.a")
set_property(TARGET cmocka PROPERTY IMPORTED_LOCATION_RELEASE
        "${binary_dir}/src/Release/libcmocka.a")

add_dependencies(cmocka cmocka_ep)

ExternalProject_Get_Property(cmocka_ep source_dir)
set(CMOCKA_INCLUDE_DIR ${source_dir}/include GLOBAL)