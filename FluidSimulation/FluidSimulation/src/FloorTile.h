#pragma once

#include "Texture.h"
#include "ShaderProgram.h"
#include "TexturedQuad.h"

#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>


class FloorTile
{
public:
	FloorTile();
	~FloorTile();

	void DrawFloorTile();
	void setUniforms(glm::mat4 proj, glm::mat4 modelview);

private:
	unsigned int vbo, ibo, va;
	Texture texture;
	ShaderProgram program;

	void LoadTexture(const std::string &path);
	void initShaders();
};