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
FetchContent_Declare(glm GIT_REPOSITORY https://github.com/g-truc/glm GIT_TAG 1.0.1)
FetchContent_MakeAvailable(glm)
add_subdirectory(vendor/stb)
add_subdirectory(vendor/imgui)

add_executable(${PROJECT_NAME}
  src/main.cpp
  vendor/imgui/imgui/backends/imgui_impl_glfw.cpp
  vendor/imgui/imgui/backends/imgui_impl_opengl3.cpp
)
target_link_libraries(${PROJECT_NAME} PRIVATE glm::glm stb imgui)

if (CMAKE_SYSTEM_NAME MATCHES "Emscripten")
  set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/pages
    OUTPUT_NAME ${PROJECT_NAME}
    SUFFIX .html)
  target_link_options(${PROJECT_NAME} PRIVATE
    -sUSE_GLFW=3
    -sUSE_WEBGL2=1
    -sFULL_ES3=1
    --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/shader@shader
  )
  
else()
  FetchContent_Declare(glfw GIT_REPOSITORY https://github.com/glfw/glfw.git GIT_TAG 3.4)
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
