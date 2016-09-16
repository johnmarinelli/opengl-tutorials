#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "deps/linmath.h"

#include <stdlib.h>
#include <stdio.h>

static const char* vertex_shader_text =
"#version 330 core \n"
"\n"
"// Input vertex data, different for all executions of this shader.\n"
"layout(location = 0) in vec3 vertexPosition_modelspace;\n"
"\n"
"void main(){\n"
"\n"
"    gl_Position.xyz = vertexPosition_modelspace;\n"
"    gl_Position.w = 1.0;\n"
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

  GLuint vtx_ary_id_2;
  glGenVertexArrays(1, &vtx_ary_id_2);
  glBindVertexArray(vtx_ary_id_2);

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

  /*
   * Initialize shaders.
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

  /*
   * Event loop
   */
  do {
    glClear(GL_COLOR_BUFFER_BIT);
    
    glUseProgram(program);

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
