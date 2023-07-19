#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Shader.hpp"

// Window settings
constexpr int WIDTH = 1024;
constexpr int HEIGHT = 768;

// Main window
Shader* shader;

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
		shader->setVec1f("offset", -0.5);
	}
	else if (key == GLFW_KEY_RIGHT) {
		shader->setVec1f("offset", 0.5);
	}
	else if (key == GLFW_KEY_ESCAPE) {
		exit(1);
	}
}

/*
0. Create a shader managing class (a class to read shader code) - DONE
1. Adjust the vertex shader so that the triangle is upside down. - DONE
2. Specify a horizontal offset via a uniform and move the triangle to the right side of the screen in the vertex shader using this offset value. - DONE
3. Output the vertex position to the fragment shader using the out keyword and set the fragment's color equal to this vertex position 
(see how even the vertex position values are interpolated across the triangle). Once you managed to do this; try to answer the following question: why is the bottom-left side of our triangle black?. - DONE

*/
int main(void) {
	glfwInit();

	shader = nullptr;

	// Set OpenGL version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	float vertices[] = {
		// positions         // colors
		 -0.5f,  0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // bottom right
		 0.5f, 0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // bottom left
		 0.0f,  -0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // top 
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
	GLuint firstVAO, firstVBO;

	// Always generate the VAO before the VBO (and EBO apparently).
	glGenVertexArrays(1, &firstVAO);
	glGenBuffers(1, &firstVBO);

	glBindVertexArray(firstVAO);
	glBindBuffer(GL_ARRAY_BUFFER, firstVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);

	glfwSetKeyCallback(window, wireframeCallback);

	shader = new Shader("Assets\\Shaders\\shader.vs", "Assets\\Shaders\\shader.fs");

	shader->use();

	// While loop so the window does not close.
	while (!glfwWindowShouldClose(window)) {
		// Changing the colors
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glBindVertexArray(firstVAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glfwSwapBuffers(window);

		// Make sure to poll events so the window is not frozen.
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &firstVAO);
	glDeleteBuffers(1, &firstVBO);

	// Destroy the window when the program is about to exit.
	glfwDestroyWindow(window);

	// And free resources from GLFW.
	glfwTerminate();
	
	return 0;
}