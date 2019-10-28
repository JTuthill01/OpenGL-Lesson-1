#pragma once
#include "GL/glew.h"
#include "glm/glm.hpp"
#include "loguru/loguru.hpp"

#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords;
};

class Mesh
{
public:
	Mesh();
	~Mesh();

	void render();

	bool loadOBJ(const std::string& filename);

private:
	void initBuffers();

	bool mIsLoaded;

	std::vector<Vertex> mVertices;

	GLuint mVBO, mVAO;
};