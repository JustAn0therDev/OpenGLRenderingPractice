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
const char* orangeFragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(0.8f, 0.3f, 0.02f, 1.0f);\n"
"}\n\0";

//Second Fragment Shader
const char* yellowFragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(1.0f, 1.0f, 0.0f, 1.0f);\n"
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
		-0.5f,  0.5f, 0.0f   // top left 
	};

	float vertices2[] =
	{
		 0.5f, -0.5f, 0.0f,  // bottom right
		-0.5f, -0.5f, 0.0f,  // bottom left
		-0.5f,  0.5f, 0.0f   // top left
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
	glShaderSource(fragmentShader, 1, &orangeFragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	// Now a second fragment shader, which sets a different color
	GLuint secondFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(secondFragmentShader, 1, &yellowFragmentShaderSource, NULL);
	glCompileShader(secondFragmentShader);

	// Now create the shader program
	GLuint orangeTriangleShaderProgram = glCreateProgram();
	GLuint yellowTriangleShaderProgram = glCreateProgram();

	// Attach the previously created shaders
	glAttachShader(orangeTriangleShaderProgram, vertexShader);
	glAttachShader(orangeTriangleShaderProgram, fragmentShader);

	glAttachShader(yellowTriangleShaderProgram, vertexShader);
	glAttachShader(yellowTriangleShaderProgram, secondFragmentShader);

	// Links the shaders to the shader program object
	// to generate a working shader program.
	glLinkProgram(orangeTriangleShaderProgram);
	glLinkProgram(yellowTriangleShaderProgram);

	// Then delete them, since the program has already been created,
	// and we want to always free the resources that are not being used anymore.

	// The vertex array and vertex buffer objects.
	GLuint firstVAO, firstVBO, secondVAO, secondVBO;

	// Always generate the VAO before the VBO (and EBO apparently).
	glGenVertexArrays(1, &firstVAO);
	glGenBuffers(1, &firstVBO);

	glGenVertexArrays(1, &secondVAO);
	glGenBuffers(1, &secondVBO);

	glBindVertexArray(firstVAO);
	glBindBuffer(GL_ARRAY_BUFFER, firstVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	// Bind the created vertex array object.
	glBindVertexArray(secondVAO);
	glBindBuffer(GL_ARRAY_BUFFER, secondVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	glfwSetKeyCallback(window, wireframe_callback);

	// While loop so the window does not close.
	while (!glfwWindowShouldClose(window)) {
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(yellowTriangleShaderProgram);

		glBindVertexArray(firstVAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glUseProgram(orangeTriangleShaderProgram);

		glBindVertexArray(secondVAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glfwSwapBuffers(window);

		// Make sure to poll events so the window is not frozen.
		glfwPollEvents();
	}

	// Shader and vertex data cleanup.
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	glDeleteShader(secondFragmentShader);

	glDeleteVertexArrays(1, &firstVAO);
	glDeleteBuffers(1, &firstVBO);
	glDeleteVertexArrays(1, &secondVAO);
	glDeleteBuffers(1, &secondVBO);
	glDeleteProgram(orangeTriangleShaderProgram);
	glDeleteProgram(yellowTriangleShaderProgram);

	// Destroy the window when the program is about to exit.
	glfwDestroyWindow(window);

	// And free resources from GLFW.
	glfwTerminate();
	
	return 0;
}