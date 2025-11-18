cmake_minimum_required(VERSION 3.21)
project(Tiny2DEngine LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_PREFIX_PATH "C:/DEV/SFML-3.0.2")

find_package(SFML 3 REQUIRED COMPONENTS Graphics Window System)

add_library(engine
    src/Engine.cpp
    src/Logger.cpp
)
target_include_directories(engine PUBLIC include)
target_link_libraries(engine PUBLIC SFML::Graphics SFML::Window SFML::System)

add_executable(demo src/main.cpp)
target_include_directories(demo PRIVATE include)
target_link_libraries(demo PRIVATE engine)

add_custom_command(TARGET demo POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
          "C:/DEV/SFML-3.0.2/bin"
          "$<TARGET_FILE_DIR:demo>")
