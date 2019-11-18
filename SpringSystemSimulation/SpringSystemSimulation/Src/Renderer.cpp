#include "Renderer.h"

#include <iostream>


//Clear error
void GLClearError()
{
	while (glGetError() != GL_NO_ERROR);
}

//Get error message
bool GLLogCall(const char* function, const char* file, int line)
{
	while (GLenum error = glGetError())
	{
		std::cout << "[OpenGL Error] (" << error << ")" << function <<
			" " << file << ":" << line << std::endl;
		return false;
	}
	return true;
}

void Renderer::Clear() const
{
	glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const
{
	shader.Bind();
	va.Bind();
	ib.Bind();

	GLCall(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr));
}

void Renderer::Draw_NoIb(const VertexArray& va, const Shader& shader) const
{
	shader.Bind();
	va.Bind();

	glDrawArrays(GL_TRIANGLES, 0, 36);
	//GLCall(glDrawElements(GL_TRIANGLES, 0, GL_UNSIGNED_INT, nullptr));
}

void Renderer::Draw_Triangle(const VertexArray& va, const Shader& shader) const
{
	shader.Bind();
	va.Bind();

	glDrawArrays(GL_TRIANGLES, 0, 3);
}

void Renderer::Draw_Line(const glm::vec3& vert1, const glm::vec3& vert2) const
{
	glBegin(GL_LINES);
	glVertex3f(vert1.x, vert1.y, vert1.z);
	glVertex3f(vert2.x, vert2.y, vert2.z);
	glEnd();
}

