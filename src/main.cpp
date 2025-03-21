#include <cstdio>
#include <array>
#include <span>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <stb_image.h>

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

  while (not glfwWindowShouldClose(window))
  {
    glfwPollEvents();

    auto const f = glfwGetTime() * glfwGetTime();
    auto const quad_pos = glm::vec2{glm::cos(f), glm::sin(f)} * glm::vec2{0.2f, 0.1f};
    vertices = std::array{
        glm::vec2{-1, -1} * 0.5f,
        glm::vec2{-1, +1} * 0.5f,
        glm::vec2{+1, -1} * 0.5f,
        glm::vec2{+1, +1} * 0.5f,
    };
    for (auto &vertex : vertices)
      vertex += quad_pos;
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, std::span{vertices}.size_bytes(), vertices.data());

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

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