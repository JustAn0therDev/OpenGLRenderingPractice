#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Shader.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Window settings
constexpr int WIDTH = 1024;
constexpr int HEIGHT = 768;

// Main window
Shader* shader;

// Transparency settings
constexpr float transparency = 0.1f;
float currentTransparency = 0.0f;

// Delta time
float lastFrame = 0.0f, deltaTime = 0.0f;

// Camera settings
bool firstMouse = true;
float pitch = 0.0f;
float yaw = 0.0f;
float lastX = 0.0f;
float lastY = 0.0f;
float fov = 45.0f;

bool wireframeToggle = false;
bool shaderToggle = false;

glm::vec3 viewVector = glm::vec3(0.0f, 0.0f, -3.0f);
float modelRotation = -55.0f;
const glm::vec3 rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);

glm::mat4 model = glm::mat4(1.0f);
glm::mat4 view = glm::mat4(1.0f);
glm::mat4 projection;

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

void printMatrix(const glm::mat4& matrix) {
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++) {
			std::cout << matrix[i][j] << " ";
		}

		std::cout << std::endl;
	}
}

void wireframeCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_RELEASE) {
		if (key == GLFW_KEY_F) {
			if (wireframeToggle) {
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				wireframeToggle = false;
			}
			else {
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				wireframeToggle = true;
			}
		}
		else if (key == GLFW_KEY_UP) {
			currentTransparency += transparency;
			shader->setFloat("transparency", currentTransparency);
		}
		else if (key == GLFW_KEY_DOWN) {
			currentTransparency -= transparency;
			shader->setFloat("transparency", currentTransparency);
		}
		else if (key == GLFW_KEY_ESCAPE) {
			exit(1);
		}
	}

	const float cameraSpeed = 7.5f * deltaTime;

	// Camera movement controls
	if (key == GLFW_KEY_W) {
		cameraPos += cameraSpeed * cameraFront;
	}
	
	if (key == GLFW_KEY_S) {
		cameraPos -= cameraSpeed * cameraFront;
	}
	
	if (key == GLFW_KEY_D) {
		cameraPos += cameraSpeed * glm::normalize(glm::cross(cameraFront, cameraUp));
	}
	
	if (key == GLFW_KEY_A) {
		cameraPos -= cameraSpeed * glm::normalize(glm::cross(cameraFront, cameraUp));
	}
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	// Mouse controls (camera yaw and pitch)
	
	if (firstMouse) {
		lastX = (float)xpos;
		lastY = (float)ypos;
		firstMouse = false;
	}

	float xoffset = (float)xpos - lastX;
	float yoffset = lastY - (float)ypos;
	lastX = (float)xpos;
	lastY = (float)ypos;

	float sensitivity = 0.1f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f)
		pitch = 89.0f;
	
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(direction);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	fov -= (float)yoffset;
	if (fov < 1.0f)
		fov = 1.0f;
	if (fov > 45.0f)
		fov = 45.0f;
}

int main(void) {
	glfwInit();
    
	shader = nullptr;
    
	// Set OpenGL version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
	float vertices[] = {
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};

	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};
    
	unsigned int indices[] = {  // note that we start from 0!
		0, 1, 3,   // first triangle
		1, 2, 3    // second triangle
	};
    
	// Create window
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "OpenGLRenderingPractice", NULL, NULL);
    
	// Error handling
	if (window == nullptr) {
		std::cout << "Unable to create window with GLFW.\n";
		return 1;
	}
    
	// Get the primary monitor's current resolution
	const GLFWvidmode* vidMode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    
	// And set the window position to always be centered on the screen.
	glfwSetWindowPos(window, (vidMode->width / 2) - WIDTH / 2, (vidMode->height / 2) - HEIGHT / 2);
    
	// Make context current. Every that happens in OpenGL will affect this window (or this "context").
	glfwMakeContextCurrent(window);
    
	// Make glad load OpenGL.
	gladLoadGL();

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	
	glfwSetKeyCallback(window, wireframeCallback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
    
	// Set the viewport (limits to where to draw the content. The positions in OpenGL are all normalized,
	// so it needs to know the screen size beforehand.)
	glViewport(0, 0, WIDTH, HEIGHT);
    
	// The vertex array and vertex buffer objects.
	GLuint VAO, VBO, EBO;
    
	// Always generate the VAO before the VBO (and EBO apparently).
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
    
	glBindVertexArray(VAO);
    
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	
	// Vertices
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// Texture coordinates
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
   
	// nrChannels -> Number of color channels
	int width, height, nrChannels;
	unsigned char* data = stbi_load("Assets\\Images\\container.jpg", &width, &height, &nrChannels, 0);
    
	GLuint texture;
	// Generate a texture
	glGenTextures(1, &texture);
    
	// Bind the texture to the current context, meaning that every action
	// from now on will be done for this texture.
	// Another thing to note is that its only bound for GL_TEXTURE_2D: no
	// other type of texture (GL_TEXTURE_1D or GL_TEXTURE_3D) will be modified 
	// in the following actions.
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
    
	// Specifying how OpenGL should deal with the image if it
	// happens to be smaller than the rendered object.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
	// Generate the texture in OpenGL.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    
	// Generate its mipmap for long distance rendering.
	glGenerateMipmap(GL_TEXTURE_2D);
    
	// Freeing the loaded image.
	stbi_image_free(data);
    
    // Flip the image on load.
    // NOTE(Ruan): Why do I need to flip the image before loading?
    // It's a PNG image and it looks fine in Windows' image viewer.
	// Maybe it's because it's a PNG and it should always be interpreted
	// differently? Maybe... but I'm not entirely sure why.
	//stbi_set_flip_vertically_on_load(true);
    
	// Loading the second image.
	data = stbi_load("Assets\\Images\\awesomeface.png", &width, &height, &nrChannels, 0);
    
	// Now generate the second texture
	GLuint texture2;
	glGenTextures(1, &texture2);
    
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture2);
    
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
	// In this case, the image is the .png format instead of .jpg, meaning that it has a new
	// channel (Alpha, for transparency). So the value for the number of channels should be
	// RGBA instead of RGB.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    
	glGenerateMipmap(GL_TEXTURE_2D);
    
	// Freeing the loaded image
	stbi_image_free(data);
    
	shader = new Shader("Assets\\Shaders\\shader.vs", "Assets\\Shaders\\shader.fs");
	shader->use();
    
	// This call is kinda optional when dealing with a single texture.
	// We can assign a location to get the texture and render it in
	// the fragment shader. The currently loaded texture has a default position of 0,
	// but not every graphics card manufacturer has this default "texture unit" set,
	// so it's better to set it manually.
	// OpenGL has at least 16 positions to set textures at a time (from GL_TEXTURE0 to GL_TEXTURE16).
    
    shader->setInt("texture1", 0);
    shader->setInt("texture2", 1);
    
    // Another important detail to highlight is that while GL_TEXTURE0 expands to 0x0,
    // GL_TEXTURE1 **DOES NOT** expand to 0x1. So when setting a value for a fragment shader to read,
    // always pass in the actual integers that correspond to their unit values.
    
    // Set the transparency uniform value so we can change it later.
	shader->setFloat("transparency", transparency);

	model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));

	shader->setMatrix4fv("model", model);
	shader->setMatrix4fv("view", view);
	shader->setMatrix4fv("projection", projection);

	glEnable(GL_DEPTH_TEST);

	// MAIN LOOP
	// While loop so the window does not close.
	while (!glfwWindowShouldClose(window)) {
		
		// Update deltaTime
		float currentFrame = (float)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

        // Changing the colors
        glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
		
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);
       
        glBindVertexArray(VAO);

		for (unsigned i = 0; i < 10; i++) {
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, cubePositions[i]);

			if (i % 3 == 0) {
				float angle = i * (float)glfwGetTime() * 3;
				model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			}

			shader->setMatrix4fv("model", model);

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		// Since the camera is moving, we have to always update the view matrix
		// in the fragment shader.
		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		shader->setMatrix4fv("view", view);
		projection = glm::perspective(glm::radians(fov), (float)WIDTH / (float)HEIGHT, 0.1f, 1000.0f);
		shader->setMatrix4fv("projection", projection);
        
		glDrawArrays(GL_TRIANGLES, 0, 36);
        // This call swaps the back and front buffers, so it needs to be here.
        glfwSwapBuffers(window);
        
        // Make sure to poll events so the window is not frozen.
        glfwPollEvents();
	}
    
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(2, &EBO);
    
	// Destroy the window when the program is about to exit.
	glfwDestroyWindow(window);
    
	// And free resources from GLFW.
	glfwTerminate();
    
	return 0;
}
