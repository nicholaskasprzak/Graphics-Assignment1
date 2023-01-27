#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include <stdio.h>

//void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void resizeFrameBufferCallback(GLFWwindow* window, int width, int height);

// Source code for vertex shader
const char* vertexShaderSource =
"#version 450						 \n"
"layout (location = 0) in vec3 vPos; \n"
"void main(){						 \n"
"	gl_Position = vec4(vPos, 1.0);   \n"
"}									 \0";

// Source code for fragment shader
const char* fragmentShaderSource =
"#version 450								\n"
"out vec4 FragColor;						\n"
"void main(){								\n"
"	FragColor = vec4(1.0, 1.0, 1.0, 1.0);	\n"
"}											\0";

/*
* An array of floats representing vertex data
* Positions supplied are read counterclockwise
* and must be supplied as such
*/
const float vertexData[] = {
	//x    y     z
	-0.5, -0.5, +0.0,
	+0.5, -0.5, +0.0,
	+0.0, +0.5, +0.0
};

int main() {
	if (!glfwInit()) {
		printf("glfw failed to init");
		return 1;
	}

	GLFWwindow* window = glfwCreateWindow(640, 480, "OpenGLExample", 0, 0);
	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK) {
		printf("glew failed to init");
		return 1;
	}

	glfwSetFramebufferSizeCallback(window, resizeFrameBufferCallback);


	/*
	* Creates a vertex shader instance
	* Replaces the source code in that instance with vertexShaderSource
	* Recompiles the object with the new source code
	*/
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	/*
	* Returns the compile status from the vertex shader after it's compilation,
	* returned in the form of a bool, either GL_TRUE or GL_FALSE
	* If compilation was unsuccessful, gets and displays the shader's info log,
	* which is then displayed on screen.
	*/
	GLint success;

	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		// 512 length is arbitrary
		GLchar infoLog[512];
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		printf("Failed to compile vertex shader: %s", infoLog);
	}
	
	/*
	* Creates a fragment shader instance
	* Replaces the source code in that instance with fragmentShaderSource
	* Recompiles the object with the new source code
	*/
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	/*
	* Returns the compile status from the fragment shader after it's compilation,
	* returned in the form of a bool, either GL_TRUE or GL_FALSE
	* If compilation was unsuccessful, gets and displays the shader's info log,
	* which is then displayed on screen.
	*/
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		// 512 length is arbitrary
		GLchar infoLog[512];
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		printf("Failed to compile fragment shader: %s", infoLog);
	}

	/*
	* Creates a new shader program instance which then has the
	* previously created and compiled vertex and fragment shaders
	* attached to it.
	* 
	* The program is then linked to create an executable program
	* with the attached shaders.
	*/
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	
	/*
	* Returns the link status from the shader program after being linked,
	* returned in the form of a bool, either GL_TRUE or GL_FALSE
	* If linking was unsuccessful, gets and displays the program's info log,
	* which is then displayed on screen.
	*/
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success)
	{
		// 512 length is arbitrary
		GLchar infoLog[512];
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		printf("Failed to link shader program: %s", infoLog);
	}

	//TODO: Delete vertex + fragment shader objects
	//glDeleteShader(...)

	/*
	* Creates a new vertex array object, which is used to
	* supply data to the vertex shader and contains pointers
	* to vertex attributes.
	* 
	* The object is bound, which allows for future function calls to modify it
	*/
	unsigned int vertexArrayObject;
	glGenVertexArrays(1, &vertexArrayObject);
	glBindVertexArray(vertexArrayObject);

	/*
	* Creates a buffer to supply vertex data, supplying it with an id in the process
	* The buffer object is the bound to the GL_ARRAY_BUFFER binding point type
	* GL_ARRAY_BUFFER is the binding target for vertex attributes
	* A buffer data store is then created at GL_ARRAY_BUFFER, allocated with
	* the size and data of the vertices and purpose of GL_STATIC_DRAW for drawing
	*/
	unsigned int vertexBufferObject;
	glGenBuffers(1, &vertexBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);

	/*
	* Defines how the vertex data provided will be interpreted by the verte shader by 
	* defining the index of the attribute, the number of components, the component type,
	* whether its normalized, the total size of in bytes of each vertex (known as the stride),
	* and the offset in bytes from the start of the vertex
	* 
	* The attribute is then enabled by reference of its id
	*/
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (const void*)0);
	glEnableVertexAttribArray(0);

	while (!glfwWindowShouldClose(window)) {
		glClearColor(0.2f, 0.3f, 0.6f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Installs the program object as part of the rendering state
		glUseProgram(shaderProgram);
		
		// Draws the triangle using the data given to the program
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

void resizeFrameBufferCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

