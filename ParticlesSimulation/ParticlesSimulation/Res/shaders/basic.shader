#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normals;
layout(location = 2) in vec2 texCoord;

uniform mat4 mvp;

void main()
{
	gl_Position = mvp * vec4(position, 1.0);
}

#shader fragment
#version 330 core

out vec4 FragColor;

void main()
{
	FragColor = vec4(0.2f, 0.3f, 0.7f, 1.0f);
}
