cmake_minimum_required(VERSION 3.10)
project(LearnOpenGL VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
  set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /MTd")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
  set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /MT")
endif()

include(FetchContent)
set(SHARED_DEPENDENCY_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/vendor/FetchContent")
FetchContent_Declare(glm  GIT_REPOSITORY https://github.com/g-truc/glm    GIT_TAG 1.0.1 SOURCE_DIR ${SHARED_DEPENDENCY_SOURCE_DIR}/glm )
FetchContent_Declare(glfw GIT_REPOSITORY https://github.com/glfw/glfw.git GIT_TAG 3.4   SOURCE_DIR ${SHARED_DEPENDENCY_SOURCE_DIR}/glfw)
FetchContent_MakeAvailable(glm)
add_subdirectory(vendor/stb)
add_subdirectory(vendor/imgui)

add_executable(${PROJECT_NAME}
  src/main.cpp
  vendor/imgui/imgui/backends/imgui_impl_glfw.cpp
  vendor/imgui/imgui/backends/imgui_impl_opengl3.cpp
)
target_link_libraries(${PROJECT_NAME} PRIVATE glm::glm stb imgui)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  target_compile_options(${PROJECT_NAME} PRIVATE /W4)
  target_compile_definitions(${PROJECT_NAME} PRIVATE _CRT_SECURE_NO_WARNINGS)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR
        CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR
        CMAKE_CXX_COMPILER_ID STREQUAL "Emscripten")
  target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion)
endif()

if (CMAKE_SYSTEM_NAME MATCHES "Emscripten")
  set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_NAME ${PROJECT_NAME}
    SUFFIX .html)
  target_link_options(${PROJECT_NAME} PRIVATE
    -sUSE_GLFW=3
    -sUSE_WEBGL2=1
    -sFULL_ES3=1
    --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/shader@shader
  )
  if (DEFINED GITHUB_PAGES_DIR)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD 
      COMMAND ${CMAKE_COMMAND} -E make_directory ${GITHUB_PAGES_DIR}
      COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG> ${GITHUB_PAGES_DIR}
    )
  endif()

else()
  set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
  set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
  set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(glfw)
  target_link_libraries(${PROJECT_NAME} PRIVATE glfw)
  if (WIN32)
    add_definitions(-DUSE_GLAD)
    add_subdirectory(vendor/glad)
    target_link_libraries(${PROJECT_NAME} PRIVATE glad)
  else()
    target_link_libraries(${PROJECT_NAME} PRIVATE GLESv2 EGL)
  endif()
endif()
