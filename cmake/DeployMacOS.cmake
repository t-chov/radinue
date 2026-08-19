cmake_minimum_required(VERSION 3.25)

if(NOT DEFINED APP_BUNDLE OR NOT IS_DIRECTORY "${APP_BUNDLE}")
    message(FATAL_ERROR "APP_BUNDLE must point to the staged Radinue.app bundle")
endif()

if(NOT DEFINED SEARCH_DIRS)
    set(SEARCH_DIRS "")
endif()

# macdeployqt handles Qt frameworks and plugins before this script runs.
# BundleUtilities then copies libmpv and its non-system transitive dependencies
# and rewrites their install names to make the application self-contained.
include(BundleUtilities)
fixup_bundle("${APP_BUNDLE}" "" "${SEARCH_DIRS}")

verify_bundle_prerequisites("${APP_BUNDLE}" bundle_verified verification_output)
if(NOT bundle_verified)
    message(FATAL_ERROR "The staged application is not self-contained:\n${verification_output}")
endif()
