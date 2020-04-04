cmake_minimum_required (VERSION 3.14)

project ("CommandManager")

include(FetchContent)

FetchContent_Declare(
  Logger
  GIT_REPOSITORY https://github.com/krobotis/Logger.git
  GIT_TAG        master
)

FetchContent_MakeAvailable(Logger)

# Include sub-projects.
add_subdirectory ("CommandManager")
add_subdirectory ("sampleApp")