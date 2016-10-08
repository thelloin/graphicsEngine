#pragma once

#include <string>
#include <GL/glew.h>

namespace Tengine {

	// This program handles the compilation, linking and usage of a GLSL shader program.
	// Reference: http://opengl.com/wiki/Shader_Compilation
	class GLSLProgram
	{
	public:
		GLSLProgram();
		~GLSLProgram();

		void compileShaders(const std::string& vertexShaderFilePath, const std::string& fragmentShaderFilePath);

		void compileShadersFromSource(const char* vertexSource, const char* fragmentSource);
		
		void linkShaders();

		void addAttribute(const std::string& attributeName);

		GLint getUniformLocation(const std::string uniformName);

		void use();
		void unuse();

		void dispose();
	private:

		int _numAttributes;

		void compileShader(const char* source, const std::string& name, GLuint shaderID);

		GLuint _programID;

		GLuint _vertexShaderID;
		GLuint _fragmentShaderID;
	};

}