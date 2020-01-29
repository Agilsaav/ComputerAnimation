#include "FloorTile.h"

#include <iostream>

FloorTile::FloorTile()
{
	float vertices[] = {
		// positions          // texture coords
		 0.5f,  -0.5f,  0.5f,   1.0f, 1.0f, // top right
		 0.5f,  -0.5f, -0.5f,   1.0f, 0.0f, // bottom right
		-0.5f,  -0.5f, -0.5f,   0.0f, 0.0f, // bottom left
		-0.5f,  -0.5f,  0.5f,   0.0f, 1.0f  // top left 
	};
	unsigned int indices[] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};

	glGenVertexArrays(1, &va);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ibo);

	glBindVertexArray(va);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	
	initShaders();

	LoadTexture("Res/Textures/floortile.jpg");
}

FloorTile::~FloorTile()
{
	glDeleteBuffers(1, &va);
	glDeleteBuffers(1, &ibo);
	glDeleteBuffers(1, &vbo);
}

void FloorTile::LoadTexture(const std::string &path)
{
	texture.loadFromFile(path, TEXTURE_PIXEL_FORMAT_RGB);
}

void FloorTile::DrawFloorTile()
{
	texture.use();
	glBindVertexArray(va);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void FloorTile::initShaders()
{
	Shader vShader, fShader;

	vShader.free();
	fShader.free();
	vShader.initFromFile(VERTEX_SHADER, "Res/shaders/floorshader.vert");
	if (!vShader.isCompiled())
	{
		cout << "Vertex Shader Error" << endl;
		cout << "" << vShader.log() << endl << endl;
	}
	fShader.initFromFile(FRAGMENT_SHADER, "Res/shaders/floorshader.frag");
	if (!fShader.isCompiled())
	{
		cout << "Fragment Shader Error" << endl;
		cout << "" << fShader.log() << endl << endl;
	}
	program.init();
	program.addShader(vShader);
	program.addShader(fShader);
	program.link();
	if (!program.isLinked())
	{
		cout << "Shader Linking Error" << endl;
		cout << "" << program.log() << endl << endl;
	}
	program.bindFragmentOutput("outColor");
}

void FloorTile::setUniforms(glm::mat4 proj, glm::mat4 modelview)
{
	program.use();
	program.setUniform4f("color", 0.9f, 0.2f, 0.75f, 1.0f);
	program.setUniform1i("bLighting", 1);
	program.setUniformMatrix4f("projection", proj);
	program.setUniformMatrix4f("modelview", modelview);

	program.setUniform1i("texture1", 0);
}
