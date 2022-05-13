#include "shader_program.hpp"

#include <cstdio>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>


ShaderProgram::ShaderProgram() 
{
	m_handle = 0;
}

ShaderProgram::~ShaderProgram() 
{
	if(m_handle != 0) {
		glDeleteProgram(m_handle);
	}
}

ShaderProgram::ShaderProgram(ShaderProgram&& other) 
{
	m_handle = other.m_handle;
	other.m_handle = 0;
}

ShaderProgram& ShaderProgram::operator=(ShaderProgram&& other) 
{
	m_handle = other.m_handle;
	other.m_handle = 0;
	return *this;
}

bool ShaderProgram::fromSources(const char *vertexShaderSource, const char *fragmentShaderSource) 
{
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	GLint result = GL_TRUE;
	int infoLogLength;

	// Compile Vertex Shader
	printf("Compiling vertex shader...\n");
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	// Check Vertex Shader
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
	glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &infoLogLength);
	if ( result == GL_FALSE && infoLogLength > 0 ){
		std::vector<char> errMsg(infoLogLength+1);
		glGetShaderInfoLog(vertexShader, infoLogLength, NULL, &errMsg[0]);
		printf("%s\n", &errMsg[0]);
		
		glDeleteShader(vertexShader);
		return false;
	}



	// Compile Fragment Shader
	printf("Compiling fragment shader...\n");
	glShaderSource(fragmentShader, 1, &fragmentShaderSource , NULL);
	glCompileShader(fragmentShader);

	// Check Fragment Shader
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &infoLogLength);
	if ( result == GL_FALSE && infoLogLength > 0 ){
		std::vector<char> errMsg(infoLogLength+1);
		glGetShaderInfoLog(fragmentShader, infoLogLength, NULL, &errMsg[0]);
		printf("%s\n", &errMsg[0]);

		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
		return false;
	}



	// Link the program
	if(m_handle != 0) {
		glDeleteProgram(m_handle);
	}

	m_handle = glCreateProgram();

	printf("Linking program\n");
	glAttachShader(m_handle, vertexShader);
	glAttachShader(m_handle, fragmentShader);
	glLinkProgram(m_handle);

	// Check the program
	glGetProgramiv(m_handle, GL_LINK_STATUS, &result);
	glGetProgramiv(m_handle, GL_INFO_LOG_LENGTH, &infoLogLength);
	if ( result == GL_FALSE && infoLogLength > 0 ){
		std::vector<char> errMsg(infoLogLength+1);
		glGetProgramInfoLog(m_handle, infoLogLength, NULL, &errMsg[0]);
		printf("%s\n", &errMsg[0]);

		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
		glDeleteProgram(m_handle);
		m_handle = 0;

		return false;
	}

	
	glDetachShader(m_handle, vertexShader);
	glDetachShader(m_handle, fragmentShader);
	
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return true;
}

bool ShaderProgram::fromFiles(const char *vertexShaderFile, const char *fragmentShaderFile) 
{
	// Read the Vertex Shader code from the file
	std::string vertexShaderCode;
	std::ifstream vertexShaderStream(vertexShaderFile, std::ios::in);
	if(vertexShaderStream.is_open()){
		std::stringstream sstr;
		sstr << vertexShaderStream.rdbuf();
		vertexShaderCode = sstr.str();
		vertexShaderStream.close();
	} else{
		printf("Can't open file %s.", vertexShaderFile);
		return 0;
	}

	// Read the Fragment Shader code from the file
	std::string fragmentShaderCode;
	std::ifstream fragmentShaderStream(fragmentShaderFile, std::ios::in);
	if(fragmentShaderStream.is_open()){
		std::stringstream sstr;
		sstr << fragmentShaderStream.rdbuf();
		fragmentShaderCode = sstr.str();
		fragmentShaderStream.close();
	} else{
		printf("Can't open file %s.", fragmentShaderFile);
		return 0;
	}

	return fromSources(vertexShaderCode.c_str(), fragmentShaderCode.c_str());	
}

GLuint ShaderProgram::getHandle() const
{
	return m_handle;
}

void ShaderProgram::bind() const
{
	glUseProgram(m_handle);
}

void ShaderProgram::unbind() const
{
	glUseProgram(0);
}
