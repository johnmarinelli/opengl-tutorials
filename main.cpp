#include <GL/glew.h>
#include <GLFW/glfw3.h>
GLFWwindow* window;

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "timer.h"

#include <stdlib.h>
#include <stdio.h>

static float x_offset = 4.0;
static float y_offset = 3.0;
static float z_offset = 3.0;

static const GLfloat vtx_buf_data[] = {
    -1.0f,-1.0f,-1.0f, // triangle 1 : begin
    -1.0f,-1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f, // triangle 1 : end
    1.0f, 1.0f,-1.0f, // triangle 2 : begin
    -1.0f,-1.0f,-1.0f,
    -1.0f, 1.0f,-1.0f, // triangle 2 : end
    1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f,-1.0f,
    1.0f,-1.0f,-1.0f,
    1.0f, 1.0f,-1.0f,
    1.0f,-1.0f,-1.0f,
    -1.0f,-1.0f,-1.0f,
    -1.0f,-1.0f,-1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f,-1.0f,
    1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f,-1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f,-1.0f, 1.0f,
    1.0f,-1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f,-1.0f,-1.0f,
    1.0f, 1.0f,-1.0f,
    1.0f,-1.0f,-1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f,-1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f,-1.0f,
    -1.0f, 1.0f,-1.0f,
    1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f,-1.0f,
    -1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,
    1.0f,-1.0f, 1.0f
};

// One color for each vertex. They were generated randomly.
static const GLfloat color_buf_data[] = { 
  0.583f,  0.771f,  0.014f,
  0.609f,  0.115f,  0.436f,
  0.327f,  0.483f,  0.844f,
  0.822f,  0.569f,  0.201f,
  0.435f,  0.602f,  0.223f,
  0.310f,  0.747f,  0.185f,
  0.597f,  0.770f,  0.761f,
  0.559f,  0.436f,  0.730f,
  0.359f,  0.583f,  0.152f,
  0.483f,  0.596f,  0.789f,
  0.559f,  0.861f,  0.639f,
  0.195f,  0.548f,  0.859f,
  0.014f,  0.184f,  0.576f,
  0.771f,  0.328f,  0.970f,
  0.406f,  0.615f,  0.116f,
  0.676f,  0.977f,  0.133f,
  0.971f,  0.572f,  0.833f,
  0.140f,  0.616f,  0.489f,
  0.997f,  0.513f,  0.064f,
  0.945f,  0.719f,  0.592f,
  0.543f,  0.021f,  0.978f,
  0.279f,  0.317f,  0.505f,
  0.167f,  0.620f,  0.077f,
  0.347f,  0.857f,  0.137f,
  0.055f,  0.953f,  0.042f,
  0.714f,  0.505f,  0.345f,
  0.783f,  0.290f,  0.734f,
  0.722f,  0.645f,  0.174f,
  0.302f,  0.455f,  0.848f,
  0.225f,  0.587f,  0.040f,
  0.517f,  0.713f,  0.338f,
  0.053f,  0.959f,  0.120f,
  0.393f,  0.621f,  0.362f,
  0.673f,  0.211f,  0.457f,
  0.820f,  0.883f,  0.371f,
  0.982f,  0.099f,  0.879f
};

static const char* vertex_shader_text =
"#version 330 core \n"
" \n"
"// Input vertex data, different for all executions of this shader. \n"
"layout(location = 0) in vec3 vertexPosition_modelspace; \n"
"layout(location = 1) in vec3 vertexColor; \n"
" \n"
"// Output data ; will be interpolated for each fragment. \n"
"out vec3 fragmentColor; \n"
"// Values that stay constant for the whole mesh. \n"
"uniform mat4 MVP; \n"
" \n"
"void main(){	 \n"
" \n"
"	// Output position of the vertex, in clip space : MVP * position \n"
"	gl_Position =  MVP * vec4(vertexPosition_modelspace,1); \n"
" \n"
"	// The color of each vertex will be interpolated \n"
"	// to produce the color of each fragment \n"
"	fragmentColor = vertexColor; \n"
"}";

static const char* fragment_shader_text =
"#version 330 core \n"
" \n"
"// Interpolated values from the vertex shaders \n"
"in vec3 fragmentColor; \n"
" \n"
"// Ouput data \n"
"out vec3 color; \n"
" \n"
"void main(){ \n"
" \n"
"	// Output color = color specified in the vertex shader,  \n"
"	// interpolated between all 3 surrounding vertices \n"
"	color = fragmentColor; \n"
" \n"
"}";

static void error_callback(int error, const char* description) {
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  if (action == GLFW_PRESS || action == GLFW_REPEAT) {
    switch(key) {
      case GLFW_KEY_A: x_offset += 0.1; break;
      case GLFW_KEY_D: x_offset -= 0.1; break;
      case GLFW_KEY_S: y_offset -= 0.1; break;
      case GLFW_KEY_W: y_offset += 0.1; break;
      case GLFW_KEY_ESCAPE: 
        glfwSetWindowShouldClose(window, GLFW_TRUE);
        break;
      default: break;
    };
  }
}

int main(int argc, char* args[]) {
  /*
   * Initialize GLFW
   */
  if (!glfwInit()) {
    return -1;
  }
  // Set error callback
  glfwSetErrorCallback(error_callback);

	glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //We don't want the old OpenGL 

  /*
   * Initialize OpenGL window
   */
  window = glfwCreateWindow(1024, 768, "Tutorial 1", NULL, NULL);

  if (NULL == window) {
    fprintf(stderr, "Failed to initialize OpenGL window.");
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);
  glfwSetKeyCallback(window, key_callback);

  /*
   * Initialize GLEW
   */
  if (glewInit() != GLEW_OK) {
    fprintf(stderr, "Failed to initialize GLEW.");
    return -1;
  }

  glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

  glClearColor(0.0, 0.0, 0.15, 0.5);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS); 

  /*
   * Initialize vertex array.
   */
  GLuint vtx_ary_id;
  glGenVertexArrays(1, &vtx_ary_id);
  glBindVertexArray(vtx_ary_id);

  // Vertex buffer.
  GLuint vtx_buf;
  glGenBuffers(1, &vtx_buf);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vtx_buf_data), vtx_buf_data, GL_STATIC_DRAW);

  GLuint color_buf;
  glGenBuffers(1, &color_buf);
  glBindBuffer(GL_ARRAY_BUFFER, color_buf);
  glBufferData(GL_ARRAY_BUFFER, sizeof(color_buf_data), color_buf_data, GL_STATIC_DRAW);

  /*
   * Load shaders
   */
  GLuint vtx_shader, frg_shader, program;
  vtx_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vtx_shader, 1,&vertex_shader_text, NULL);
  glCompileShader(vtx_shader);

  frg_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(frg_shader, 1, &fragment_shader_text, NULL);
  glCompileShader(frg_shader);

  program = glCreateProgram();
  glAttachShader(program, vtx_shader);
  glAttachShader(program, frg_shader);
  glLinkProgram(program);

  // Get handle of MVP variable from shader
  GLuint mtx_id = glGetUniformLocation(program,  "MVP");

  /*
   * Model, view and projection matrices
   */
  glm::mat4 model = glm::mat4(1.0);
  glm::mat4 view = glm::lookAt(
    glm::vec3(x_offset, y_offset, z_offset), // (4,3,3) in world space (eye)
    glm::vec3(0.0, 0.0, 0.0), // (0,0,0) is origin (center)
    glm::vec3(0.0, 1.0, 0.0)  // (0,1,0) is up vector
  );

  glm::mat4 proj = glm::perspective(
      glm::radians(45.0), // field of vision
      4.0 / 3.0,
      0.1,
      100.0
  );

  /*
   * Event loop
   */
  do {
    glClear(GL_COLOR_BUFFER_BIT);

    unsigned long ms = get_msec();
    float r = cos(ms / 1000.0);
    glm::mat4 transformed_model = proj * view * model;
    glm::mat4 mvp = transformed_model;

    glUseProgram(program);

		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
    glUniformMatrix4fv(mtx_id, 1, GL_FALSE, &mvp[0][0]);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vtx_buf);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		// 2nd attribute buffer : colors
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, color_buf);
		glVertexAttribPointer(
			1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			3,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

    glDrawArrays(GL_TRIANGLES, 0, 12 * 3);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    glfwSwapBuffers(window);
    glfwPollEvents();
  } while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);

  // Cleanup VBO
  glDeleteVertexArrays(1, &vtx_ary_id);

  glfwTerminate();

  return 0;
}
