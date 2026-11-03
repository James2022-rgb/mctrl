cmake_minimum_required(VERSION 3.22)

set(TARGET_NAME "mctrl")
set(SRC_DIR "src")

project(${TARGET_NAME}
    VERSION 0.1.0.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror=thread-safety")
endif()

# ------------------------------------------------------------
# dependencies
#

# The consuming project may already provide mbase.
if(NOT TARGET mbase)
    add_subdirectory("mbase")
endif()

# ------------------------------------------------------------
# project targets
#

# Not BUILD_SHARED_LIBS, so that dependencies (fmt, ...) stay static.
option(MCTRL_BUILD_SHARED "Build mctrl as a shared library" OFF)

if(MCTRL_BUILD_SHARED)
    add_library(${TARGET_NAME} SHARED)
    target_compile_definitions(${TARGET_NAME}
        PUBLIC MCTRL_SHARED
        PRIVATE MCTRL_BUILDING
    )
    # Export only what `MCTRL_API` marks.
    set_target_properties(${TARGET_NAME} PROPERTIES
        CXX_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN ON
    )
else()
    add_library(${TARGET_NAME} STATIC)
endif()
target_compile_features(${TARGET_NAME} PRIVATE cxx_std_23)

if(MSVC)
    target_compile_options(${TARGET_NAME} PRIVATE /W4 /utf-8)
else()
    target_compile_options(${TARGET_NAME} PRIVATE -Wall -Wextra)
endif()

set(SOURCES_PRIVATE_ROOT_DIR "${SRC_DIR}/${TARGET_NAME}/private")
set(SOURCES_PUBLIC_ROOT_DIR "${SRC_DIR}/${TARGET_NAME}/public")

set(SOURCES_PRIVATE_ROOT
    ${SOURCES_PRIVATE_ROOT_DIR}/backend_iface.h
    ${SOURCES_PRIVATE_ROOT_DIR}/mctrl.cpp
)
source_group("Private" FILES ${SOURCES_PRIVATE_ROOT})

# Exactly one backend per target.
if(EMSCRIPTEN)
    set(SOURCES_PRIVATE_BACKEND
        ${SOURCES_PRIVATE_ROOT_DIR}/backend_web.cpp
        ${SOURCES_PRIVATE_ROOT_DIR}/backend_web.h
    )
elseif(PSP)
    set(SOURCES_PRIVATE_BACKEND
        ${SOURCES_PRIVATE_ROOT_DIR}/backend_psp.cpp
        ${SOURCES_PRIVATE_ROOT_DIR}/backend_psp.h
    )
elseif(WIN32)
    set(SOURCES_PRIVATE_BACKEND
        ${SOURCES_PRIVATE_ROOT_DIR}/backend_win32.cpp
        ${SOURCES_PRIVATE_ROOT_DIR}/backend_win32.h
    )
else()
    message(FATAL_ERROR "mctrl: unsupported platform")
endif()
source_group("Private/Backend" FILES ${SOURCES_PRIVATE_BACKEND})

set(SOURCES_PUBLIC_ROOT
    ${SOURCES_PUBLIC_ROOT_DIR}/mctrl.h
    ${SOURCES_PUBLIC_ROOT_DIR}/mctrl_types.h
)
source_group("Public" FILES ${SOURCES_PUBLIC_ROOT})

set(SOURCES
    ${SOURCES_PRIVATE_ROOT}
    ${SOURCES_PRIVATE_BACKEND}
    ${SOURCES_PUBLIC_ROOT}
)

target_sources(${TARGET_NAME} PRIVATE ${SOURCES})

target_link_libraries(${TARGET_NAME} PUBLIC
    mbase
)

target_include_directories(${TARGET_NAME} PUBLIC
    ${SRC_DIR}
)
