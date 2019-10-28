#pragma once
#include "GL/glew.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>

#include "loguru/loguru.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

class Shader
{
public:
	unsigned int ID;

	Shader(const GLchar* vertexPath, const GLchar* fragmentPath);

	void use();
	void setBool(const std::string& name, bool value)const;
	void setInt(const std::string& name, int value)const;
	void setFloat(const std::string& name, float value)const;
	void setMat4(const std::string& name, const glm::mat4& mat) const;
	void setUniform(const GLchar* name, const glm::vec2& v);
	void setUniform(const GLchar* name, const glm::vec3& v);
	void setUniform(const GLchar* name, const glm::vec4& v);
	void setUniform(const GLchar* name, const glm::mat4& m);
	void setUniform(const GLchar* name, const GLfloat f);
	void setUniform(const GLchar* name, const GLint v);
	void setUniformSampler(const GLchar* name, const GLint& slot);

private:
	void checkCompileErrors(unsigned int shader, std::string type);

	std::map<std::string, GLint> mUniformLocations;
	GLint getUniformLocation(const GLchar* name);

	GLuint mHandle;
};