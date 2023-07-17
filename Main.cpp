#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

bool wireframeToggle = false;
bool shaderToggle = false;

void wireframe_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
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
	else if (key == GLFW_KEY_S && action == GLFW_RELEASE) {
		if (shaderToggle) {
			shaderToggle = false;
		}
		else {
			shaderToggle = true;
		}
	}
	else if (key == GLFW_KEY_ESCAPE) {
		exit(1);
	}
}

// Window settings
constexpr int WIDTH = 1024;
constexpr int HEIGHT = 768;

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

//Second Fragment Shader
const char* secondFragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);\n"
"}\n\0";

int main(void) {
	glfwInit();

	// Set OpenGL version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	float vertices[] =
	{
		 0.5f,  0.5f, 0.0f,  // top right
		 0.5f, -0.5f, 0.0f,  // bottom right
		-0.5f, -0.5f, 0.0f,  // bottom left
		-0.5f,  0.5f, 0.0f   // top left 
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

	// Create the vertex shader, set the source and compile it
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	// Create the fragment shader, set the source and compile it
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	// Now a second fragment shader, which sets a different color
	GLuint secondFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(secondFragmentShader, 1, &secondFragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	// Now create the shader program
	GLuint firstTriangleShaderProgram = glCreateProgram();
	GLuint secondTriangleShaderProgram = glCreateProgram();

	// Attach the previously created shaders
	glAttachShader(firstTriangleShaderProgram, vertexShader);
	glAttachShader(secondTriangleShaderProgram, vertexShader);

	glAttachShader(firstTriangleShaderProgram, fragmentShader);
	glAttachShader(secondTriangleShaderProgram, secondFragmentShader);

	// Links the shaders to the shader program object
	// to generate a working shader program.
	glLinkProgram(firstTriangleShaderProgram);
	glLinkProgram(secondTriangleShaderProgram);

	// Then delete them, since the program has already been created,
	// and we want to always free the resources that are not being used anymore.

	// The vertex array and vertex buffer objects.
	GLuint VAO, VBO, EBO;

	// Always generate the VAO before the VBO (and EBO apparently).
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	// Bind the created vertex array object.
	glBindVertexArray(VAO);
	
	// Bind the buffer object
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// Set the buffer data
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Bind the buffer object
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	// Set the buffer data
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Call vertex attribute pointer
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	// Enable it
	glEnableVertexAttribArray(0);

	// Bind to 0
	glBindBuffer(VBO, 0);
	
	// Same for array
	glBindVertexArray(0);

	glfwSetKeyCallback(window, wireframe_callback);

	// While loop so the window does not close.
	while (!glfwWindowShouldClose(window)) {
		// Clear the color.
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);

		// Set the buffer to be the back buffer.
		glClear(GL_COLOR_BUFFER_BIT);

		if (shaderToggle) {
			glUseProgram(secondTriangleShaderProgram);
		}
		else {
			glUseProgram(firstTriangleShaderProgram);
		}
		
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		glfwSwapBuffers(window);

		// Make sure to poll events so the window is not frozen.
		glfwPollEvents();
	}

	// Shader and vertex data cleanup.
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	glDeleteShader(secondFragmentShader);

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteProgram(firstTriangleShaderProgram);
	glDeleteProgram(secondTriangleShaderProgram);

	// Destroy the window when the program is about to exit.
	glfwDestroyWindow(window);

	// And free resources from GLFW.
	glfwTerminate();
	
	return 0;
}