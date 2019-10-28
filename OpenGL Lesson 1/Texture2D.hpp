#pragma once
#include "GL/glew.h"
#include "stb_image.h"
#include "loguru/loguru.hpp"
#include <string>
#include <iostream>

class Texture2D
{
public:
	Texture2D();
	~Texture2D();

	bool loadTexture(const std::string& fileName, bool generateMipmaps = true);

	void bind(GLuint texUnit = 0);
	void unbind(GLuint texUnit = 0);

private:
	GLuint mTexture;
};