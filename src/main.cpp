#if defined(USE_GLAD)
#include <glad/glad.h>
#else // defined(USE_GLAD)
#include <GLES3/gl3.h>
#endif // defined(USE_GLAD)
#if defined(__EMSCRIPTEN__)
#include <emscripten.h>
#else  // defined(__EMSCRIPTEN__)
#endif // defined(__EMSCRIPTEN__)

#include <cstdio>
#include <array>
#include <span>
#include <string>
#include <memory>
#include <source_location>
#include <filesystem>
#include <utility>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <stb_image.h>

auto read_file_all(char const *filepath) -> std::string
{
  auto res = std::string{};
  if (auto const file = std::fopen(filepath, "r"))
  {
    res.resize((std::fseek(file, 0, SEEK_END), (size_t)::ftell(file)));
    res.resize((std::fseek(file, 0, SEEK_SET), (size_t)::fread(res.data(), sizeof(res.at(0)), res.size(), file)));
    std::fclose(file);
  }
  return res;
}

#define glCheckError(...) ::gl_check_error(__VA_ARGS__)
auto static gl_check_error(std::source_location location = std::source_location::current()) -> void
{
  GLenum err;
  while ((err = glGetError()) not_eq GL_NO_ERROR)
  {
    auto name = "UNKNOWN_ERROR";
    for (auto const &[err_num, err_name] : {std::pair{(GLenum)GL_INVALID_ENUM /*                  */, "INVALID_ENUM" /*                  */},
                                            std::pair{(GLenum)GL_INVALID_VALUE /*                 */, "INVALID_VALUE" /*                 */},
                                            std::pair{(GLenum)GL_INVALID_OPERATION /*             */, "INVALID_OPERATION" /*             */},
                                            std::pair{(GLenum)GL_OUT_OF_MEMORY /*                 */, "OUT_OF_MEMORY" /*                 */},
                                            std::pair{(GLenum)GL_INVALID_FRAMEBUFFER_OPERATION /* */, "INVALID_FRAMEBUFFER_OPERATION" /* */}})
      if (err == err_num)
        name = err_name;
    std::fprintf(stderr, "GLES Error 0x%03x %-18s on line %u from function %s\n", err, name, location.line(), location.function_name());
  }
}

auto compile_shader(GLuint sid, char const *glsl) -> GLuint
{
  glShaderSource(sid, 1, &glsl, nullptr);
  glCompileShader(sid);
  if (auto status = 0, len = 0; glGetShaderiv(sid, GL_COMPILE_STATUS, &status), not status)
  {
    glGetShaderiv(sid, GL_INFO_LOG_LENGTH, &len);
    auto const log = std::make_unique<char[]>((size_t)len);
    glGetShaderInfoLog(sid, len, &len, log.get());
    std::fprintf(stderr, "Shader Error: %s", log.get());
    glCheckError();
    sid = 0;
  }
  glCheckError();
  return sid;
}

auto make_program(char const *vert_glsl, const char *frag_glsl) -> GLuint
{
  auto pid = glCreateProgram();
  auto vid = glCreateShader(GL_VERTEX_SHADER);
  auto fid = glCreateShader(GL_FRAGMENT_SHADER);
  compile_shader(vid, vert_glsl);
  compile_shader(fid, frag_glsl);
  glAttachShader(pid, vid);
  glAttachShader(pid, fid);
  glLinkProgram(pid);
  glDeleteShader(vid);
  glDeleteShader(fid);
  if (auto status = 0, len = 0; glGetProgramiv(pid, GL_LINK_STATUS, &status), not status)
  {
    glGetProgramiv(pid, GL_INFO_LOG_LENGTH, &len);
    auto const log = std::make_unique<char[]>((size_t)len);
    glGetProgramInfoLog(pid, len, &len, log.get());
    std::fprintf(stderr, "Program Error: %s", log.get());
    glDeleteProgram(pid), pid = 0;
    glCheckError();
  }
  glCheckError();
  return pid;
}

int main()
{
  std::puts("Hello World");

  if (not glfwInit())
  {
    std::fprintf(stderr, "%s init fail\n", "glfw");
    return EXIT_FAILURE;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
  auto const window = glfwCreateWindow(720, 720, "LearnOpenGL", 0, 0);
  if (not window)
  {
    std::fprintf(stderr, "%s init fail\n", "window");
    glfwTerminate();
    return EXIT_FAILURE;
  }
  glfwMakeContextCurrent(window);

#if defined(USE_GLAD)
  if (not gladLoadGLES2Loader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
  {
    std::fprintf(stderr, "%s init fail\n", "glad");
    glfwTerminate();
    return EXIT_FAILURE;
  }
#else  // defined(USE_GLAD)
#endif // defined(USE_GLAD)

  auto static click_pos = glm::f64vec2{};
  glfwSetMouseButtonCallback(window, [](GLFWwindow *window, int /* button */, int /* action */, int /* mods */)
                             { glfwGetCursorPos(window, &click_pos.x, &click_pos.y); });

  auto static constexpr vertices = std::array{
      glm::vec2{-1, -1},
      glm::vec2{-1, +1},
      glm::vec2{+1, -1},
      glm::vec2{+1, +1},
  };

  GLuint vbo;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr) std::span{vertices}.size_bytes(), vertices.data(), GL_STATIC_DRAW);
  glCheckError();

  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  glVertexAttribPointer(0, 2, GL_FLOAT, false, sizeof(vertices.at(0)), 0);
  glEnableVertexAttribArray(0);
  glCheckError();

  auto vert_path = "shader/vert.glsl",
       frag_path = "shader/frag.glsl";
  auto last_write = std::max(std::filesystem::last_write_time(vert_path),
                             std::filesystem::last_write_time(frag_path));
  auto pid = make_program(read_file_all(vert_path).c_str(),
                          read_file_all(frag_path).c_str());

  auto frame = 0u - 1u;
  auto prev_time = glfwGetTime();
  auto mouse_pos = glm::f64vec2{};
  auto window_size = glm::i32vec2{};
  auto const render_loop = [&]
  {
    if (glfwWindowShouldClose(window))
      return false;

    glfwPollEvents();

    frame++;
    auto const time = glfwGetTime();
    auto const dt = time - std::exchange(prev_time, time);
    auto const t = std::time(nullptr);
    auto const tm = *std::localtime(&t);

    glfwGetCursorPos(window, &mouse_pos.x, &mouse_pos.y);
    glfwGetWindowSize(window, &window_size.x, &window_size.y);
    glViewport(0, 0, window_size.x, window_size.y);

    if (auto next_last_write = std::max(std::filesystem::last_write_time(vert_path),
                                        std::filesystem::last_write_time(frag_path));
        last_write not_eq next_last_write)
      if (auto next_pid = make_program(read_file_all(vert_path).c_str(),
                                       read_file_all(frag_path).c_str()))
      {
        glDeleteProgram(pid), pid = next_pid;
        last_write = next_last_write;
      }

    glCheckError();
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glCheckError();

    glUseProgram(pid);
    glUniform3f(glGetUniformLocation(pid, "iResolution" /* */), (float)(window_size.x), (float)(window_size.y), (1)); // viewport resolution (in pixels)
    glUniform1f(glGetUniformLocation(pid, "iTime" /*       */), (float)(time));                                       // shader playback time (in seconds)
    glUniform1f(glGetUniformLocation(pid, "iTimeDelta" /*  */), (float)(dt));                                         // render time (in seconds)
    glUniform1f(glGetUniformLocation(pid, "iFrameRate" /*  */), (float)(1.0 / dt));                                   // shader frame rate
    glUniform1i(glGetUniformLocation(pid, "iFrame" /*      */), (int)(frame));                                        // shader playback frame
    glUniform4f(glGetUniformLocation(pid, "iMouse" /*      */),                                                       // mouse pixel coords. xy: current (if MLB down), zw: click
                (float)mouse_pos.x, (float)mouse_pos.y,                                                               //   mouse coords
                (float)click_pos.x, (float)click_pos.y);                                                              //   click coords
    glUniform4f(glGetUniformLocation(pid, "iDate" /*       */),                                                       // (year, month, day, time in seconds)
                (float)(tm.tm_year + 1900),                                                                           //   year
                (float)(tm.tm_mon + 1),                                                                               //   month
                (float)(tm.tm_mday),                                                                                  //   day
                (float)(tm.tm_hour * 3600 + tm.tm_min * 60 + tm.tm_sec));                                             //   seconds
    glCheckError();
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glCheckError();

    glfwSwapInterval(1);
    glfwSwapBuffers(window);
    glCheckError();

    return true;
  };
#if defined(__EMSCRIPTEN__)
  auto static const &render_loop_static = render_loop;
  auto static constexpr emscripten_main_loop = []
  {
    if (not render_loop_static())
      emscripten_cancel_main_loop();
  };
  emscripten_set_main_loop(emscripten_main_loop, 0, 1);
#else  // defined(__EMSCRIPTEN__)
  while (render_loop())
    ;
#endif // defined(__EMSCRIPTEN__)

  glDeleteProgram(pid);
  glDeleteBuffers(1, &vbo);
  glDeleteVertexArrays(1, &vao);
  glfwDestroyWindow(window);
  glfwTerminate();
}