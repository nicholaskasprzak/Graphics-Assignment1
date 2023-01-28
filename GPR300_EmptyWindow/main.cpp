#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include <stdio.h>

//void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void resizeFrameBufferCallback(GLFWwindow* window, int width, int height);

/*
* Source code for vertex shader
* 
* Variables from the currently bound
* VBO pass in values from their ids
* 
* Takes in vPos at location 1 and vCol
* at location 2
* 
* vColor is passed out to the fragment
* shader at Color
* 
* vPos is used to define the vertices
* positions through gl_Position
* 
* The time uniform is used to modify the positions
* of the vertices on the X and Y axes.
* Multiplying the x position by the absolute value
* of sin time creates the effect of the vertices
* shrinking and growing on the x axis. Adding half
* of sin time to the y position creates the effect
* of the vertices moving up and down on the y axis.
*/
const char* vertexShaderSource =
"#version 450						 \n"
"layout (location = 0) in vec3 vPos; \n"
"layout (location = 1) in vec3 vCol; \n"
"out vec3 Color;					 \n"
"uniform float _Time;				 \n"
"void main(){						 \n"
"	Color = vCol;					 \n"
"	gl_Position = vec4(abs(sin(_Time)) * vPos.x, (0.5 * sin(_Time)) + vPos.y, vPos.z, 1.0);\n"
"}									 \0";

/*
* Source code for the fragment shader
* 
* Takes in Color from the vertex shader
* and passes out FragColor
* 
* Using time, the fragments
* pulse between their original color
* and black. That value is multiplied
* by 2 to amplify the glowing effect.
*/
const char* fragmentShaderSource =
"#version 450									  \n"
"in vec3 Color;									  \n"
"out vec4 FragColor;							  \n"
"uniform float _Time;							  \n"
"void main(){									  \n"
"	FragColor = vec4(abs(sin(_Time) * 2)*Color, 1.0); \n"
"}												  \0";

/*
* An array of floats representing vertex data
* Positions supplied are read counterclockwise
* and must be supplied as such
* 
* Colors are supplied in an RGBA format and each
* set corresponds to a vertex's color
*/
const float vertexData[] = {
	//x    y     z
	-0.5, -0.5, +0.0,	1.0, 0.0, 0.0, 1.0,
	+0.5, -0.5, +0.0,	0.0, 1.0, 0.0, 1.0,
	+0.0, +0.5, +0.0,	0.0, 0.0, 1.0, 1.0
};

/*
* A second array of vertex data that creates
* two triangles. The two triangles are reflective
* of each other on the y axis, creating a form
* similar to that of a butterfly. Its colored
* white at its center and black at its outer
* vertices.
* 
* glDrawArrays() has to contain 6 to account for
* having to draw 6 vertices.
*/
const float altVertexData[] = {
	//x		y	   z
	-0.0, -0.5, +0.0, 1.0, 1.0, 1.0, 1.0,
	+1.0, -1.0, +0.0, 0.0, 0.0, 0.0, 1.0,
	+0.5, +0.5, +0.0, 0.0, 0.0, 0.0, 1.0,

	-1.0, -1.0, +0.0, 0.0, 0.0, 0.0, 1.0,
	+0.0, -0.5, +0.0, 1.0, 1.0, 1.0, 1.0,
	-0.5, +0.5, +0.0, 0.0, 0.0, 0.0, 1.0
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
	glBufferData(GL_ARRAY_BUFFER, sizeof(altVertexData), altVertexData, GL_STATIC_DRAW);

	/*
	* Defines how the vertex data provided will be interpreted by the verte shader by 
	* defining the index of the attribute, the number of components, the component type,
	* whether its normalized, the total size of in bytes of each vertex (known as the stride),
	* and the offset in bytes from the start of the vertex
	* 
	* The attribute is then enabled by reference of its id
	* 
	* The first attribute defines position by reading in three values from a 7 float vertex
	* from the start of the vertex. Its id is 0
	* 
	* The second attribute defines color by reading in four values from a 7 float vertex
	* starting at the fourth value. Its id is 1
	*/
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 7, (const void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 7, (const void*)(sizeof(float) * 3));
	glEnableVertexAttribArray(1);

	while (!glfwWindowShouldClose(window)) {
		glClearColor(0.2f, 0.3f, 0.6f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Installs the program object as part of the rendering state
		glUseProgram(shaderProgram);

		// Supplies the _Time uniform with the time value
		float time = (float)glfwGetTime();
		glUniform1f(glGetUniformLocation(shaderProgram, "_Time"), time);
		
		// Draws the triangle using the data given to the program
		glDrawArrays(GL_TRIANGLES, 0, 6);

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

