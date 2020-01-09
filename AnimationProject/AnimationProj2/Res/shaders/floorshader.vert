#version 330 core

layout(location = 0) in vec3 positions;
layout(location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

uniform mat4 projection, modelview;

void main()
{
	TexCoords = aTexCoords;
	gl_Position = projection* modelview * vec4(positions, 1.0);
};