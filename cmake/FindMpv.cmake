find_package(PkgConfig QUIET)

if(PkgConfig_FOUND)
    pkg_check_modules(PC_MPV QUIET mpv)
endif()

find_path(
    Mpv_INCLUDE_DIR
    NAMES mpv/client.h
    HINTS ${PC_MPV_INCLUDE_DIRS}
    PATH_SUFFIXES include
)

find_library(
    Mpv_LIBRARY
    NAMES mpv libmpv
    HINTS ${PC_MPV_LIBRARY_DIRS}
    PATH_SUFFIXES lib
)

if(WIN32)
    find_file(
        Mpv_RUNTIME_LIBRARY
        NAMES libmpv-2.dll
        HINTS ${CMAKE_PREFIX_PATH}
        PATH_SUFFIXES bin .
    )
endif()

include(FindPackageHandleStandardArgs)
if(WIN32)
    find_package_handle_standard_args(
        Mpv
        REQUIRED_VARS Mpv_LIBRARY Mpv_RUNTIME_LIBRARY Mpv_INCLUDE_DIR
        VERSION_VAR PC_MPV_VERSION
    )
else()
    find_package_handle_standard_args(
        Mpv
        REQUIRED_VARS Mpv_LIBRARY Mpv_INCLUDE_DIR
        VERSION_VAR PC_MPV_VERSION
    )
endif()

if(Mpv_FOUND AND NOT TARGET Mpv::Mpv)
    if(WIN32)
        add_library(Mpv::Mpv SHARED IMPORTED)
        set_target_properties(
            Mpv::Mpv
            PROPERTIES
                IMPORTED_IMPLIB "${Mpv_LIBRARY}"
                IMPORTED_LOCATION "${Mpv_RUNTIME_LIBRARY}"
                INTERFACE_INCLUDE_DIRECTORIES "${Mpv_INCLUDE_DIR}"
        )
    else()
        add_library(Mpv::Mpv UNKNOWN IMPORTED)
        set_target_properties(
            Mpv::Mpv
            PROPERTIES
                IMPORTED_LOCATION "${Mpv_LIBRARY}"
                INTERFACE_INCLUDE_DIRECTORIES "${Mpv_INCLUDE_DIR}"
        )
    endif()
endif()

mark_as_advanced(Mpv_INCLUDE_DIR Mpv_LIBRARY Mpv_RUNTIME_LIBRARY)
