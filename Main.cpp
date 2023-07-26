#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Shader.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Window settings
constexpr int WIDTH = 1024;
constexpr int HEIGHT = 768;

// Main window
Shader* shader;
constexpr float offset = 0.1f;
float currentOffset = 0;

bool wireframeToggle = false;
bool shaderToggle = false;

void wireframeCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_F && action == GLFW_RELEASE) {
		if (wireframeToggle) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			wireframeToggle = false;
		}
		else {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			wireframeToggle = true;
		}
	}
	else if (key == GLFW_KEY_LEFT) {
		currentOffset -= offset;
		shader->setVec1f("offset", currentOffset);
	}
	else if (key == GLFW_KEY_RIGHT) {
		currentOffset += offset;
		shader->setVec1f("offset", currentOffset);
	}
	else if (key == GLFW_KEY_ESCAPE) {
		exit(1);
	}
}

int main(void) {
	glfwInit();

	shader = nullptr;

	// Set OpenGL version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	float vertices[] = {
		// positions          // colors           // texture coords
		 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
		 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
		-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
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

	// Load glad so it configures OpenGL
	gladLoadGL();

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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// Colors
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);
	// Texture coordinates
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glfwSetKeyCallback(window, wireframeCallback);

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

	// Generate the texture in OpenGL
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

	// Generate its mipmap for long distance rendering
	glGenerateMipmap(GL_TEXTURE_2D);

	// Freeing the loaded image
	stbi_image_free(data);

	stbi_set_flip_vertically_on_load(true);

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

	// While loop so the window does not close.
	while (!glfwWindowShouldClose(window)) {
		// Changing the colors
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

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