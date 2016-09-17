#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "deps/linmath.h"
#include "timer.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

static const char* vertex_shader_text =
"#version 330 core \n"
"\n"
"// Input vertex data, different for all executions of this shader.\n"
"layout(location = 0) in vec3 vertexPosition_modelspace;\n"
"// Values that stay uniform for the whole mesh\n"
"uniform mat4 MVP;\n"
"\n"
"void main(){\n"
"\n"
" gl_Position = MVP * vec4(vertexPosition_modelspace, 1);\n"
"\n"
"}\n";

static const char* fragment_shader_text =
"#version 330 core \n"
" \n"
"// Ouput data \n"
"out vec3 color; \n"
" \n"
"void main() \n"
"{ \n"
" \n"
"  // Output color = red  \n"
"  color = vec3(1,0,0); \n"
" \n"
"} \n";

/*
 * Returns a compiled glsl program id.
 */
GLuint load_shaders(const char* vertex_shader_text, const char* fragment_shader_text) {
  /*
   * Load shaders.
   */
  GLuint vtx_shader;
  vtx_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vtx_shader, 1, &vertex_shader_text, NULL);
  glCompileShader(vtx_shader);

  GLuint frg_shader;
  frg_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(frg_shader, 1, &fragment_shader_text, NULL);
  glCompileShader(frg_shader);

  GLuint program;
  program = glCreateProgram();
  glAttachShader(program, vtx_shader);
  glAttachShader(program, frg_shader);
  glLinkProgram(program);
  return program;
}

static void error_callback(int error, const char* description) {
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

int main(int argc, char* args[]) {
  // Set error callback
  glfwSetErrorCallback(error_callback);

  /*
   * Initialize GLFW
   */
  if (!glfwInit()) {
    return -1;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //We don't want the old OpenGL 

  /*
   * Initialize OpenGL window
   */
  GLFWwindow* window;
  window = glfwCreateWindow(1024, 768, "Tutorial 1", NULL, NULL);

  if (NULL == window) {
    fprintf(stderr, "Failed to initialize OpenGL window.");
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);
  glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

  /* 
   * Initialize GLEW
   */
  if (glewInit() != GLEW_OK) {
    fprintf(stderr, "Failed to initialize GLEW.");
    return -1;
  }

  // Black background
  glClearColor(0.0, 0.0, 0.0, 0.0);

  /* 
   * Initialize vertex array
   */
  GLuint vtx_ary_id;
  glGenVertexArrays(1, &vtx_ary_id);
  glBindVertexArray(vtx_ary_id);

  // Data for vertex buffer
  static const GLfloat vtx_buf_data[] = {
    -1.0, -1.0, 0.0,
    1.0, -1.0, 0.0,
    0.0, 1.0, 0.0
  };

  // The vertex buffer.
  GLuint vtx_buf;

  // Generate 1 buffer.  Put resulting identifier in vtx_buf.
  glGenBuffers(1, &vtx_buf);
  glBindBuffer(GL_ARRAY_BUFFER, vtx_buf);

  // Pass vertices to OpenGL
  glBufferData(GL_ARRAY_BUFFER, sizeof(vtx_buf_data), vtx_buf_data, GL_STATIC_DRAW);

  GLuint program = load_shaders(vertex_shader_text, fragment_shader_text);

  // Get handle of MVP variable from shader
  GLuint mtx_id = glGetUniformLocation(program,  "MVP");

  /*
   * Event loop
   */
  do {
    glm::mat4 model = glm::mat4(
      glm::vec4(1.0, 0.0, 0.0, 0.0),
      glm::vec4(0.0, 1.0, 0.0, 0.0),
      glm::vec4(0.0, 0.0, 1.0, 0.0),
      glm::vec4(0.0, 0.0, 0.0, 1.0)
    );
    /*
     * Model, view and projection matrices
     */
    glm::mat4 view = glm::lookAt(
      glm::vec3(4.0, 3.0, 3.0), // (4,3,3) in world space (eye)
      glm::vec3(0.0, 0.0, 0.0), // (0,0,0) is origin (center)
      glm::vec3(0.0, 1.0, 0.0)  // (0,1,0) is up vector
    );
    glm::mat4 proj = glm::perspective(
      glm::radians(45.0), // field of vision
      1024.0 / 768.0,
      0.1,
      100.0
    );

    unsigned long ms = get_msec();
    float r = cos(ms / 1000.0);

    glm::mat4 rotated_model = glm::rotate(model, r, glm::vec3(0.0, 1.0, 0.0));
    glm::mat4 mvp = proj * view * rotated_model;

    glClear(GL_COLOR_BUFFER_BIT);
    
    glUseProgram(program);
    glUniformMatrix4fv(mtx_id, 1, GL_FALSE, &mvp[0][0]);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vtx_buf);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    glDisableVertexAttribArray(0);

    glfwSwapBuffers(window);
    glfwPollEvents();
  } while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);

  // Cleanup VBO
  glDeleteVertexArrays(1, &vtx_ary_id);

  glfwTerminate();
  return 0;
}
