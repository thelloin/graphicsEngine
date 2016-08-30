#include "GLSLProgram.h"
#include "Errors.h"

#include <vector>

#include <fstream>

namespace Tengine {

	GLSLProgram::GLSLProgram() : _numAttributes(0), _programID(0), _vertexShaderID(0), _fragmentShaderID(0)
	{
	}


	GLSLProgram::~GLSLProgram()
	{
	}

	// Compiles the shaders into a form that your GPU can understand
	void GLSLProgram::compileShaders(const std::string& vertexShaderFilePath, const std::string& fragmentShaderFilePath)
	{
		//Vertex and fragment shaders are successfully compiled.
		//Now time to link them together into a program.
		//Get a program object.
		_programID = glCreateProgram();

		// Create the vertex shader object, and store its ID
		_vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
		if (_vertexShaderID == 0) {
			fatalError("Vertex shader failed to be created!");
		}

		// Create the fragment shader object, and store its ID
		_fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
		if (_fragmentShaderID == 0) {
			fatalError("Fragment shader failed to be created!");
		}

		compileShader(vertexShaderFilePath, _vertexShaderID);
		compileShader(fragmentShaderFilePath, _fragmentShaderID);

	}

	void GLSLProgram::linkShaders()
	{
		//Attach our shaders to our program
		glAttachShader(_programID, _vertexShaderID);
		glAttachShader(_programID, _fragmentShaderID);

		//Link our program
		glLinkProgram(_programID);

		//Note the different functions here: glGetProgram* instead of glGetShader*.
		GLint isLinked = 0;
		glGetProgramiv(_programID, GL_LINK_STATUS, (int *)&isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(_programID, GL_INFO_LOG_LENGTH, &maxLength);

			//The maxLength includes the NULL character
			std::vector<char> errorLog(maxLength);
			glGetProgramInfoLog(_programID, maxLength, &maxLength, &errorLog[0]);

			//We don't need the program anymore.
			glDeleteProgram(_programID);
			//Don't leak shaders either.
			glDeleteShader(_vertexShaderID);
			glDeleteShader(_fragmentShaderID);

			std::printf("%s\n", &errorLog[0]);
			fatalError("Shaders failed to link!");
		}

		//Always detach shaders after a successful link.
		glDetachShader(_programID, _vertexShaderID);
		glDetachShader(_programID, _fragmentShaderID);
		glDeleteShader(_vertexShaderID);
		glDeleteShader(_fragmentShaderID);
	}

	// Adds an attribute to our shader. Should be called between compiling and linking.
	void GLSLProgram::addAttribute(const std::string& attributeName)
	{
		glBindAttribLocation(_programID, _numAttributes++, attributeName.c_str());
	}

	GLint GLSLProgram::getUniformLocation(const std::string uniformName)
	{
		GLint location = glGetUniformLocation(_programID, uniformName.c_str());
		if (location == GL_INVALID_INDEX) {
			fatalError("Uniform " + uniformName + " not found in shader!");
		}

		return location;
	}

	// Enable the shader, and all its attributes
	void GLSLProgram::use()
	{
		glUseProgram(_programID);
		// Enable all the attributes we added with addAttribute
		for (int i = 0; i < _numAttributes; i++) {
			glEnableVertexAttribArray(i);
		}
	}

	void GLSLProgram::unuse()
	{
		glUseProgram(0);
		for (int i = 0; i < _numAttributes; i++) {
			glDisableVertexAttribArray(i);
		}
	}

	// Compiles a single shader file
	void GLSLProgram::compileShader(const std::string& filePath, GLuint shaderID)
	{
		// Open the file
		std::ifstream shaderFile(filePath);
		if (shaderFile.fail()) {
			perror(filePath.c_str());
			fatalError("Failed to open " + filePath);
		}

		// File contents stores all the text in the file
		std::string fileContents = "";
		// Line is used to grab each line of the file
		std::string line;

		while (std::getline(shaderFile, line)) {
			fileContents += line + "\n";
		}

		shaderFile.close();

		// Get a pointer to our file contents c string
		const char* contentsPtr = fileContents.c_str();
		// Tell openGL that we want to use fileContents as the contents of the shader file
		glShaderSource(shaderID, 1, &contentsPtr, nullptr);

		// Compile the shader
		glCompileShader(shaderID);

		// Check for success
		GLint success = 0;
		glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);

		if (success == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<char> errorLog(maxLength);
			glGetShaderInfoLog(shaderID, maxLength, &maxLength, &errorLog[0]);

			// Provide the infolog in whatever manor you deem best.
			// Exit with failure
			glDeleteShader(shaderID); // Don't leak the shader.

			// Print error log and quit
			std::printf("%s\n", &(errorLog[0]));
			fatalError("Shader " + filePath + "failed to compile");
		}
	}

}