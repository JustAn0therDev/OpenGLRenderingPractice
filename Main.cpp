#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

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
"layout (location = 1) in vec3 aColor;\n"
"out vec3 ourColor;"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos, 1.0);\n"
"   ourColor = aColor;\n"
"}\0";

//Fragment Shader
const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"in vec3 ourColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(ourColor, 1.0f);\n"
"}\n\0";

int main(void) {
	glfwInit();

	// Set OpenGL version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	float vertices[] = {
		// positions         // colors
		 0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // bottom right
		-0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // bottom left
		 0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // top 
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

	// Now create the shader program
	GLuint shaderProgram = glCreateProgram();

	// Attach the previously created shaders
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);

	// Links the shaders to the shader program object
	// to generate a working shader program.
	glLinkProgram(shaderProgram);

	// Then delete them, since the program has already been created,
	// and we want to always free the resources that are not being used anymore.
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

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

	// Calling this function only once, since its the only shader program in this project.
	glUseProgram(shaderProgram);

	// While loop so the window does not close.
	while (!glfwWindowShouldClose(window)) {
		// Changing the colors
		float timeValue = glfwGetTime();
		float greenValue = (sin(timeValue) / 2.0f) + 0.5f;
		int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
		
		// glUseProgram(shaderProgram) does not need to be called multiple times.
		// In this case, we would want to call it multiple times if the shaders changed
		// in different places. So, when modifying an uniform value, the best thing to do
		// is make sure the correct shader program is being used.
		// If not, the program could fail.
		glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);

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
	glDeleteProgram(shaderProgram);

	// Destroy the window when the program is about to exit.
	glfwDestroyWindow(window);

	// And free resources from GLFW.
	glfwTerminate();
	
	return 0;
}