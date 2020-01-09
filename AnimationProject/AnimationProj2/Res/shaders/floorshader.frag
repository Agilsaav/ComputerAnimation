#version 330 core

out vec4 outColor;

in vec2 TexCoords;

uniform vec4 color;
uniform int bLighting;

uniform sampler2D texture1;

void main()
{
	outColor = texture(texture1, TexCoords);
	//outColor = color;
}