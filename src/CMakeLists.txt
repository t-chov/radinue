add_library(
    radinue_core STATIC
    core/DirectoryPlaylist.cpp
    core/DirectoryPlaylist.h
    core/PlaybackSettings.cpp
    core/PlaybackSettings.h
)
target_include_directories(radinue_core PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(radinue_core PUBLIC Qt6::Core)
radinue_set_warnings(radinue_core)

qt_add_executable(
    radinue
    main.cpp
    player/PlayerController.cpp
    player/PlayerController.h
    ui/MainWindow.cpp
    ui/MainWindow.h
)

target_link_libraries(radinue PRIVATE radinue_core Qt6::Widgets Mpv::Mpv)
radinue_set_warnings(radinue)

if(WIN32)
    add_custom_command(
        TARGET radinue
        POST_BUILD
        COMMAND
            "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:Mpv::Mpv>"
            "$<TARGET_FILE_DIR:radinue>"
        VERBATIM
    )
endif()

set_target_properties(
    radinue
    PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_BUNDLE_NAME "Radinue"
        MACOSX_BUNDLE_GUI_IDENTIFIER "org.radinue.Radinue"
        WIN32_EXECUTABLE TRUE
)
