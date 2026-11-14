# CMakeList.txt : CMake project for rigidbody2d-ccd-multirate, include source and define
# project specific logic here.

cmake_minimum_required(VERSION 3.20)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()


project ("rigidbody2d-ccd-multirate")

add_executable (rigidbody2d-ccd-multirate "rigidbody2d-ccd-multirate.cpp"  "Window.h" "PhysObject.h" "Vec2.h" "Window.cpp" "PhysObject.cpp" "Game.h" "Game.cpp" "Simulation.h" "Simulation.cpp")

find_package(SDL2 CONFIG REQUIRED)
find_package(sdl2-gfx CONFIG REQUIRED)

target_link_libraries("rigidbody2d-ccd-multirate" PRIVATE SDL2::SDL2 SDL2::SDL2main SDL2::SDL2_gfx)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET rigidbody2d-ccd-multirate PROPERTY CXX_STANDARD 20)
endif()



