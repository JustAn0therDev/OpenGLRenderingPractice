#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

// Vertex Shader
const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";
//Fragment Shader
const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(0.8f, 0.3f, 0.02f, 1.0f);\n"
"}\n\0";

int main(void) {
	glfwInit();

	// Set OpenGL version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLfloat vertices[] =
	{
		-0.5f, -0.5f * float(sqrt(3)) / 3, 0.0f,
		0.5f, -0.5f * float(sqrt(3)) / 3, 0.0f,
		0.0f, 0.5f * float(sqrt(3)) * 2 / 3, 0.0f
	};

	// Create window
	GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGLRenderingPractice", NULL, NULL);

	// Error handling
	if (window == nullptr) {
		std::cout << "Unable to create window with GLFW.\n";
		return 1;
	}

	// Make context current (not really sure what this means yet...)
	glfwMakeContextCurrent(window);

	// Load glad so it configures OpenGL
	gladLoadGL();

	// Set the viewport (limits to where to draw the content)
	glViewport(0, 0, 800, 600);

	// Create the vertex shader, set the source and compile it
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	// Create the fragment shader, set the source and compile it
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	// Now create the shader program
	GLuint shaderProgram = glCreateProgram();

	// Attach the previously created shaders
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	// Then delete them, since the program has already been created,
	// and we want to always free the resources that are not being used anymore.
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	// The vertex array and vertex buffer objects.
	GLuint VAO, VBO;

	// Always generate the VAO before the VBO.
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	// Bind the created vertex array.
	glBindVertexArray(VAO);
	
	// Bind the buffer object
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// Set the buffer data
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Call vertex attribute pointer
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	// Enable it
	glEnableVertexAttribArray(0);

	// Bind to 0
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	// Same for array
	glBindVertexArray(0);

	// Clear the color.
	glClearColor(0.0f, 0.0f, 255.0f, 1.0f);

	// Set the buffer to be the back buffer.
	glClear(GL_COLOR_BUFFER_BIT);

	// And tell glfw to swap the buffers.
	glfwSwapBuffers(window);

	// While loop so the window does not close.
	while (!glfwWindowShouldClose(window)) {
		// Clear the color.
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);

		// Set the buffer to be the back buffer.
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glfwSwapBuffers(window);
		// Make sure to poll events so the window is not frozen.
		glfwPollEvents();
	}

	// Shader and vertex data cleanup.
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(shaderProgram);

	// Destroy the window when the program is about to exit.
	glfwDestroyWindow(window);

	// And free resources from GLFW.
	glfwTerminate();
	
	return 0;
}