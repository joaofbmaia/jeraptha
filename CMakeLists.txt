cmake_minimum_required (VERSION 3.6)
project(example)

add_executable(jeraptha main.cpp jeraptha.cpp configuration.cpp engine.cpp economy.cpp bettor.cpp wager.cpp)

add_subdirectory(sleepy-discord)
target_link_libraries(jeraptha sleepy-discord)