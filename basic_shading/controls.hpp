#ifndef CONTROLS_HPP
#define CONTROLS_HPP

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

glm::mat4 ViewMatrix;
glm::mat4 ProjectionMatrix;

glm::mat4 getViewMatrix(){
	return ViewMatrix;
}
glm::mat4 getProjectionMatrix(){
	return ProjectionMatrix;
}

glm::vec3 position = glm::vec3(0, 0, 5);

// horizontal angle: toward -Z
float horizontal_angle = 3.14f;

// vertical angle; 0; look toward horizon
float vertical_angle = 0.0f;

float initialFoV = 45.0f;

float speed = 3.0f;
float mouseSpeed = 0.00005f;

void computeMatricesFromInputs() {
  uint32_t width = 1024;
  uint32_t height = 768;
	// glfwGetTime is called only once, the first time this function is called
	static double lastTime = glfwGetTime();

	// Compute time difference between current and last frame
	double currentTime = glfwGetTime();
	float deltaTime = float(currentTime - lastTime);

	// Get mouse position
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	// Reset mouse position for next frame
	//glfwSetCursorPos(window, width/2, height/2);

	// Compute new orientation
	horizontal_angle += mouseSpeed * float(width/2 - xpos );
	vertical_angle   += mouseSpeed * float(height/2 - ypos );

	// Direction : Spherical coordinates to Cartesian coordinates conversion
	glm::vec3 direction(
		cos(vertical_angle) * sin(horizontal_angle), 
		sin(vertical_angle),
		cos(vertical_angle) * cos(horizontal_angle)
	);
	
	// Right vector
	glm::vec3 right = glm::vec3(
		sin(horizontal_angle - 3.14f/2.0f), 
		0,
		cos(horizontal_angle - 3.14f/2.0f)
	);
	
	// Up vector
	glm::vec3 up = glm::cross( right, direction );

	// Move forward
	if (glfwGetKey( window, GLFW_KEY_UP ) == GLFW_PRESS){
		position += direction * deltaTime * speed;
	}
	// Move backward
	if (glfwGetKey( window, GLFW_KEY_DOWN ) == GLFW_PRESS){
		position -= direction * deltaTime * speed;
	}
	// Strafe right
	if (glfwGetKey( window, GLFW_KEY_RIGHT ) == GLFW_PRESS){
		position += right * deltaTime * speed;
	}
	// Strafe left
	if (glfwGetKey( window, GLFW_KEY_LEFT ) == GLFW_PRESS){
		position -= right * deltaTime * speed;
	}

	float FoV = initialFoV;// - 5 * glfwGetMouseWheel(); // Now GLFW 3 requires setting up a callback for this. It's a bit too complicated for this beginner's tutorial, so it's disabled instead.

	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
  ProjectionMatrix = glm::perspective(FoV, 4.0f / 3.0f, 0.1f, 100.0f);
  // Camera matrix
  ViewMatrix = glm::lookAt(
    position,
    position + direction,
    up
  );
	// For the next frame, the "last time" will be "now"
	lastTime = currentTime;
}

#endif
