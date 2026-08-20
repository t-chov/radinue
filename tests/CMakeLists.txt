qt_add_executable(playback_settings_test PlaybackSettingsTest.cpp)
target_link_libraries(playback_settings_test PRIVATE radinue_core Qt6::Test)
radinue_set_warnings(playback_settings_test)

add_test(NAME playback_settings COMMAND playback_settings_test)

qt_add_executable(directory_playlist_test DirectoryPlaylistTest.cpp)
target_link_libraries(directory_playlist_test PRIVATE radinue_core Qt6::Test)
radinue_set_warnings(directory_playlist_test)

add_test(NAME directory_playlist COMMAND directory_playlist_test)

qt_add_executable(playback_state_store_test PlaybackStateStoreTest.cpp)
target_link_libraries(playback_state_store_test PRIVATE radinue_core Qt6::Test)
radinue_set_warnings(playback_state_store_test)

add_test(NAME playback_state_store COMMAND playback_state_store_test)

qt_add_executable(player_controller_test PlayerControllerTest.cpp)
target_link_libraries(player_controller_test PRIVATE radinue_app Qt6::Test)
radinue_set_warnings(player_controller_test)

add_test(NAME player_controller COMMAND player_controller_test)

qt_add_executable(main_window_test MainWindowTest.cpp)
target_link_libraries(main_window_test PRIVATE radinue_app Qt6::Test)
radinue_set_warnings(main_window_test)

add_test(NAME main_window COMMAND main_window_test)

if(WIN32)
    add_library(radinue_test_vulkan_stub SHARED windows/VulkanStub.cpp)
    set_target_properties(
        radinue_test_vulkan_stub
        PROPERTIES OUTPUT_NAME "vulkan-1" WINDOWS_EXPORT_ALL_SYMBOLS ON
    )
    radinue_set_warnings(radinue_test_vulkan_stub)

    foreach(test_target IN ITEMS player_controller_test main_window_test)
        add_dependencies(${test_target} radinue_test_vulkan_stub)
        add_custom_command(
            TARGET ${test_target}
            POST_BUILD
            COMMAND
                "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:Mpv::Mpv>"
                "$<TARGET_FILE_DIR:${test_target}>"
            COMMAND
                "${CMAKE_COMMAND}" -E copy_if_different
                "$<TARGET_FILE:radinue_test_vulkan_stub>" "$<TARGET_FILE_DIR:${test_target}>"
            VERBATIM
        )
    endforeach()
endif()
