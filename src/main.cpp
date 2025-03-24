#include <cstdio>
#include <array>
#include <span>
#include <string>
#include <memory>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <stb_image.h>

auto read_file_all(char const *filepath) -> std::string
{
  auto res = std::string{};
  if (auto const file = std::fopen(filepath, "r"))
  {
    res.resize((std::fseek(file, 0, SEEK_END), std::ftell(file)));
    res.resize((std::fseek(file, 0, SEEK_SET), std::fread(res.data(), sizeof(res.at(0)), res.size(), file)));
    std::fclose(file);
  }
  return res;
}

auto compile_shader(GLuint sid, char const *glsl) -> GLuint
{
  glShaderSource(sid, 1, &glsl, nullptr);
  glCompileShader(sid);
  auto status = 0;
  glGetShaderiv(sid, GL_COMPILE_STATUS, &status);
  if (status)
    return sid;
  auto len = 0;
  glGetShaderiv(sid, GL_INFO_LOG_LENGTH, &len);
  auto log = std::make_unique<char[]>(len);
  glGetShaderInfoLog(sid, len, &len, log.get());
  std::fprintf(stderr, "Shader Error: %s", log.get());
  return 0;
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
  auto status = 0;
  glGetProgramiv(pid, GL_LINK_STATUS, &status);
  if (status)
    return pid;
  auto len = 0;
  glGetProgramiv(pid, GL_INFO_LOG_LENGTH, &len);
  auto log = std::make_unique<char[]>(len);
  glGetProgramInfoLog(pid, len, &len, log.get());
  std::fprintf(stderr, "Program Error: %s", log.get());
  glDeleteProgram(pid);
  return 0;
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
  if (not gladLoadGLES2Loader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
  {
    std::fprintf(stderr, "%s init fail\n", "glad");
    glfwTerminate();
    return EXIT_FAILURE;
  }

  auto vertices = std::array{
      glm::vec2{-1, -1} * 0.5f,
      glm::vec2{-1, +1} * 0.5f,
      glm::vec2{+1, -1} * 0.5f,
      glm::vec2{+1, +1} * 0.5f,
  };

  GLuint vbo;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, std::span{vertices}.size_bytes(), vertices.data(), GL_STATIC_DRAW);

  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  glVertexAttribPointer(0, 2, GL_FLOAT, false, sizeof(vertices.at(0)), 0);
  glEnableVertexAttribArray(0);

  auto pid = make_program(read_file_all("shader/vert.glsl").c_str(),
                          read_file_all("shader/frag.glsl").c_str());

  while (not glfwWindowShouldClose(window))
  {
    glfwPollEvents();

    if (auto const new_pid = make_program(read_file_all("shader/vert.glsl").c_str(),
                                          read_file_all("shader/frag.glsl").c_str()))
      glDeleteProgram(pid), pid = new_pid;

    auto const size = glm::min(1.0f, 1.3f - 1.0f / ((float)glfwGetTime() * 1.0f));
    vertices = std::array{
        glm::vec2{-1, -1} * size,
        glm::vec2{-1, +1} * size,
        glm::vec2{+1, -1} * size,
        glm::vec2{+1, +1} * size,
    };
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, std::span{vertices}.size_bytes(), vertices.data());

    GLsizei window_width, window_height;
    glfwGetWindowSize(window, &window_width, &window_height);
    glViewport(0, 0, window_width, window_height);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(pid);
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glfwSwapInterval(1);
    glfwSwapBuffers(window);
  }

  glDeleteBuffers(1, &vbo);
  glDeleteVertexArrays(1, &vao);
  glfwDestroyWindow(window);
  glfwTerminate();
}