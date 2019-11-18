#shader vertex
#version 330 core

layout(location = 0) in vec3 aPos;

uniform mat4 mvp;

void main()
{
	gl_Position = mvp * vec4(aPos, 1.0);
}

#shader fragment
#version 330 core

out vec4 FragColor;

uniform vec3 particleColor;

void main()
{
	//FragColor = texture(texture_diffuse1, TexCoords);
	FragColor = vec4(particleColor, 1.0f);
}