#version 330 core

layout(location = 0) in vec3 positions;
layout(location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

uniform mat4 projection, model, view;

void main()
{
	TexCoords = aTexCoords;
	gl_Position = projection* view * model * vec4(positions, 1.0);
};