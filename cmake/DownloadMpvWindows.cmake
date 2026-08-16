cmake_minimum_required(VERSION 3.25)

if(NOT DEFINED OUTPUT_DIR)
    message(FATAL_ERROR "OUTPUT_DIR must point to the libmpv SDK destination")
endif()

set(mpv_version "2026-08-15-e167836802")
set(mpv_archive "mpv-dev-x86_64-20260815-git-e167836802.7z")
set(mpv_sha256 "e98e9138fe180f15ddc868966d593e6b1f8c5a6cf16c4ca9a91aa937056275de")
set(mpv_url "https://github.com/zhongfly/mpv-winbuild/releases/download/${mpv_version}/${mpv_archive}")
set(download_dir "${OUTPUT_DIR}/.download")
set(archive_path "${download_dir}/${mpv_archive}")

file(MAKE_DIRECTORY "${download_dir}")
file(MAKE_DIRECTORY "${OUTPUT_DIR}")

message(STATUS "Downloading pinned libmpv SDK ${mpv_version}")
file(
    DOWNLOAD "${mpv_url}" "${archive_path}"
    EXPECTED_HASH "SHA256=${mpv_sha256}"
    SHOW_PROGRESS
    TLS_VERIFY ON
)

file(ARCHIVE_EXTRACT INPUT "${archive_path}" DESTINATION "${OUTPUT_DIR}")

if(NOT EXISTS "${OUTPUT_DIR}/include/mpv/client.h" OR NOT EXISTS "${OUTPUT_DIR}/libmpv-2.dll")
    message(FATAL_ERROR "The downloaded libmpv SDK has an unexpected layout")
endif()
