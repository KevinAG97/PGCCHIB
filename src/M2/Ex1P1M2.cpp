#include <iostream>
#include <string>
#include <assert.h>
#include <cmath>

using namespace std;

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
int setupShader();
GLuint createTriangle(float x0, float y0, float x1, float y1, float x2, float y2);

const GLuint WIDTH = 800, HEIGHT = 600;

const GLchar* vertexShaderSource = "#version 400\n"
"layout (location = 0) in vec3 position;\n"
"uniform mat4 projection;\n"
"uniform mat4 model;\n"
"void main()\n"
"{\n"
"gl_Position = projection * model * vec4(position, 1.0);\n"
"}\0";

const GLchar* fragmentShaderSource = "#version 400\n"
"uniform vec4 inputColor;\n"
"out vec4 color;\n"
"void main()\n"
"{\n"
"color = inputColor;\n"
"}\n\0";

int main()
{
	glfwInit();
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Ola Triangulo! -- Rossana", nullptr, nullptr);
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* version = glGetString(GL_VERSION);
	cout << "Renderer: " << renderer << endl;
	cout << "OpenGL version supported: " << version << endl;

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	GLuint shaderID = setupShader();
	glUseProgram(shaderID);

	GLint colorLoc = glGetUniformLocation(shaderID, "inputColor");

	mat4 projection = ortho(0.0, 800.0, 0.0, 600.0, -1.0, 1.0);  
	glUniformMatrix4fv(glGetUniformLocation(shaderID, "projection"), 1, GL_FALSE, value_ptr(projection));

	GLuint VAO = createTriangle(-0.5f, -0.5f, 0.5f, -0.5f, 0.0f, 0.5f);

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		mat4 model = mat4(1.0f);
		model = translate(model, vec3(400.0f, 300.0f, 0.0f));
		model = rotate(model, (float)glfwGetTime(), vec3(0.0f, 0.0f, 1.0f));
		model = scale(model, vec3(abs(cos(glfwGetTime())) * 300.0f));

		glUniformMatrix4fv(glGetUniformLocation(shaderID, "model"), 1, GL_FALSE, value_ptr(model));

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glLineWidth(10);
		glPointSize(20);

		glBindVertexArray(VAO);
		float time = glfwGetTime();
		float red   = (sin(time * 2.0f) + 1.0f) / 2.0f;
		float green = (sin(time * 2.0f + 2.0f) + 1.0f) / 2.0f;
		float blue  = (sin(time * 2.0f + 4.0f) + 1.0f) / 2.0f;
		glUniform4f(colorLoc, red, green, blue, 1.0f);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);

		glfwSwapBuffers(window);
	}

	glDeleteVertexArrays(1, &VAO);
	glfwTerminate();
	return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

int setupShader()
{
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	GLint success;
	GLchar infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return shaderProgram;
}

GLuint createTriangle(float x0, float y0, float x1, float y1, float x2, float y2)
{
	GLfloat vertices[] = {
		x0, y0, 0.0f,
		x1, y1, 0.0f,
		x2, y2, 0.0f
	};

	GLuint VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return VAO;
}
