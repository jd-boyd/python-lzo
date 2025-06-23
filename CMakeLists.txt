cmake_minimum_required(VERSION 3.15)

project(python_lzo)

# Set C/C++ standards
set(CMAKE_C_STANDARD 11)

# Option to use system liblzo instead of building from source
option(USE_SYSTEM_LZO "Use system-installed liblzo instead of building from source" OFF)

# Platform-specific settings
if(WIN32)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

# Find Python for the extension
find_package(Python COMPONENTS Interpreter Development REQUIRED)

if(USE_SYSTEM_LZO)
    # Use system-installed liblzo
    find_library(LZO_LIBRARY
        NAMES lzo2 lzo
        PATHS /usr/lib /usr/local/lib /opt/local/lib
        DOC "liblzo library"
    )

    find_path(LZO_INCLUDE_DIR
        NAMES lzo/lzo1x.h
        PATHS /usr/include /usr/local/include /opt/local/include
        DOC "liblzo include directory"
    )

    if(NOT LZO_LIBRARY OR NOT LZO_INCLUDE_DIR)
        message(FATAL_ERROR "System liblzo not found. Install liblzo-dev or disable USE_SYSTEM_LZO")
    endif()

    message(STATUS "Using system liblzo: ${LZO_LIBRARY}")
    message(STATUS "liblzo headers: ${LZO_INCLUDE_DIR}")

    # Create imported target for system liblzo
    add_library(lzo_lib SHARED IMPORTED)
    set_target_properties(lzo_lib PROPERTIES
        IMPORTED_LOCATION ${LZO_LIBRARY}
        INTERFACE_INCLUDE_DIRECTORIES ${LZO_INCLUDE_DIR}
    )

else()
    # Build liblzo from source in lzo-2.10 subdirectory
    set(LZO_PROJECT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lzo-2.10")
    set(LZO_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}/lzo_build")

    # Determine build command based on platform
    if(WIN32)
        # Windows: Use MSBuild
        set(LZO_BUILD_COMMAND
            msbuild lzo_static_lib.vcxproj
            -p:Configuration=Release$<SEMICOLON>Platform=x64$<SEMICOLON>OutDir=..\\
        )
        set(LZO_STATIC_LIB "${LZO_BUILD_DIR}/lzo2.lib")
        set(LZO_CONFIGURE_COMMAND
            ${CMAKE_COMMAND} -S ${LZO_PROJECT_DIR} -B ${LZO_BUILD_DIR}
            -DCMAKE_BUILD_TYPE=Release
            -A x64
        )
    else()
        # Linux/macOS: Use make
        set(LZO_BUILD_COMMAND make -j${CMAKE_BUILD_PARALLEL_LEVEL})
        if(APPLE)
            set(LZO_STATIC_LIB "${LZO_BUILD_DIR}/src/.libs/liblzo2.a")
        else()
            set(LZO_STATIC_LIB "${LZO_BUILD_DIR}/liblzo2.a")
        endif()
        set(LZO_CONFIGURE_COMMAND
            ${CMAKE_COMMAND} -S ${LZO_PROJECT_DIR} -B ${LZO_BUILD_DIR}
            -DCMAKE_BUILD_TYPE=Release
            -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
            -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
        )
    endif()

    # Configure and build liblzo using ExternalProject
    include(ExternalProject)
    ExternalProject_Add(lzo_project
        SOURCE_DIR ${LZO_PROJECT_DIR}
        BINARY_DIR ${LZO_BUILD_DIR}
        CONFIGURE_COMMAND ${LZO_CONFIGURE_COMMAND}
        BUILD_COMMAND ${LZO_BUILD_COMMAND}
        INSTALL_COMMAND ""  # No install step needed
        BUILD_BYPRODUCTS ${LZO_STATIC_LIB}
        BUILD_IN_SOURCE 0
    )

    # Create an imported target for the built liblzo static library
    add_library(lzo_lib STATIC IMPORTED)
    set_target_properties(lzo_lib PROPERTIES
        IMPORTED_LOCATION ${LZO_STATIC_LIB}
        INTERFACE_INCLUDE_DIRECTORIES ${LZO_PROJECT_DIR}/include
    )

    # Make sure liblzo is built before we try to use it
    add_dependencies(lzo_lib lzo_project)
endif()

# Compile the lzomodule.c file into an object library
add_library(lzo_module SHARED
    lzomodule.c
)

# Set properties for the object library
target_include_directories(lzo_module PRIVATE
    ${Python_INCLUDE_DIRS}
)

if(USE_SYSTEM_LZO)
    target_include_directories(lzo_module PRIVATE ${LZO_INCLUDE_DIR})
else()
    target_include_directories(lzo_module PRIVATE ${LZO_PROJECT_DIR}/include)
endif()


# Link the lzo library (either system or built from source)
target_link_libraries(lzo_module PRIVATE
    lzo_lib
    ${Python_LIBRARIES}
)

# Set target properties
set_target_properties(lzo_module PROPERTIES
  OUTPUT_NAME "lzo"  # This will create lzo.so/.dll/.pyd
  PREFIX ""
    INTERPROCEDURAL_OPTIMIZATION TRUE
)

# Include directories for the final module
if(USE_SYSTEM_LZO)
    target_include_directories(lzo_module PRIVATE ${LZO_INCLUDE_DIR})
else()
    target_include_directories(lzo_module PRIVATE ${LZO_PROJECT_DIR}/include)
    # Ensure liblzo is built before our module
    add_dependencies(lzo_module lzo_project)
endif()

# Platform-specific linking options
if(WIN32)
    target_compile_definitions(lzo_module PRIVATE _WIN32_WINNT=0x0601)
    # On Windows, you might need additional libraries
    target_link_libraries(lzo_module PRIVATE ws2_32)
elseif(APPLE)
    # macOS specific settings
    set_target_properties(lzo_module PROPERTIES
        INSTALL_RPATH "@loader_path"
    )
elseif(UNIX)
    # Linux specific settings
    set_target_properties(lzo_module PROPERTIES
        INSTALL_RPATH "$ORIGIN"
    )
    target_link_libraries(lzo_module PRIVATE pthread dl)
endif()

# Compiler-specific optimizations
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    if(MSVC)
        target_compile_options(lzo_module PRIVATE /O2)
        target_compile_options(lzo_modules PRIVATE /O2)
    else()
        target_compile_options(lzo_module PRIVATE -O3 -march=native)
        target_compile_options(lzo_modules PRIVATE -O3 -march=native)
    endif()
endif()

# Print configuration summary
message(STATUS "")
message(STATUS "python-lzo Configuration Summary:")
message(STATUS "  Use system liblzo: ${USE_SYSTEM_LZO}")
if(USE_SYSTEM_LZO)
    message(STATUS "  liblzo library: ${LZO_LIBRARY}")
    message(STATUS "  liblzo headers: ${LZO_INCLUDE_DIR}")
else()
    message(STATUS "  liblzo source: ${LZO_PROJECT_DIR}")
    message(STATUS "  liblzo build: ${LZO_BUILD_DIR}")
    message(STATUS "  Platform: ${CMAKE_SYSTEM_NAME}")
    if(WIN32)
        message(STATUS "  Build tool: MSBuild")
    else()
        message(STATUS "  Build tool: make")
    endif()
endif()
message(STATUS "")
