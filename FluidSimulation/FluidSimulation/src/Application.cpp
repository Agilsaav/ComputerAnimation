#include <GL/glew.h>
#include <GL/glut.h>
#include "Application.h"

void Application::init()
{
	bPlay = true;
	glClearColor(1.f, 1.f, 1.f, 1.0f);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	scene.init();
	
	for(unsigned int i=0; i<256; i++)
	{
	  keys[i] = false;
	  specialKeys[i] = false;
	}
	mouseButtons[0] = false;
	mouseButtons[1] = false;
	lastMousePos = glm::ivec2(-1, -1);

	lastX = -1.0;
	lastY = -1.0f;
	firstMouse = true;
}

bool Application::loadMesh(const char *filename)
{
  return scene.loadMesh(filename);
}

bool Application::update(int deltaTime)
{
	scene.update(deltaTime);
	
	return bPlay;
}

void Application::render(double fps)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	scene.render(fps);
}

void Application::resize(int width, int height)
{
  glViewport(0, 0, width, height);
  scene.getCamera().resizeCameraViewport(width, height);
}

void Application::keyPressed(int key)
{
	if(key == 27) // Escape code
		bPlay = false;
	if (key == 119 or key == 87)
		scene.getCamera().ProcessKeyboard(FORWARD);
	if (key == 115 or key == 83)
		scene.getCamera().ProcessKeyboard(BACKWARD);
	if (key == 97 or key == 65)
		scene.getCamera().ProcessKeyboard(LEFT);
	if (key == 100 or key == 68)
		scene.getCamera().ProcessKeyboard(RIGHT);

	if (key == 77 or key == 109)
		scene.changeModelMode();
	keys[key] = true;
}

void Application::keyReleased(int key)
{
	keys[key] = false;
}

void Application::specialKeyPressed(int key)
{
	specialKeys[key] = true;
}

void Application::specialKeyReleased(int key)
{
	specialKeys[key] = false;
	if(key == GLUT_KEY_F1)
	  scene.switchPolygonMode();
}

void Application::mouseMovePress(int x, int y)
{
	
	if (mouseButtons[0])
		scene.getCamera().processZoom(ZOOM_IN, 0.05f * (y - lastY));

	if (mouseButtons[1])
		scene.getCamera().processZoom(ZOOM_OUT, 0.05f * (y - lastY));
		
	lastY = y;
	lastX = x;
}
void Application::mouseMove(int x, int y)
{

		if (firstMouse)
		{
			lastX = x;
			lastY = y;
			firstMouse = false;
		}

		float xoffset = x - lastX;
		float yoffset = lastY - y; // reversed since y-coordinates go from bottom to top

		lastX = x;
		lastY = y;

		scene.getCamera().ProcessMouseMovement(xoffset, yoffset, true);

		if (x < 250 || x > 640 - 250) {  //you can use values other than 100 for the screen edges if you like, kind of seems to depend on your mouse sensitivity for what ends up working best
			lastX = 640 / 2;   //centers the last known position, this way there isn't an odd jump with your cam as it resets
			lastY = 480 / 2;
			glutWarpPointer(640 / 2, 480 / 2);  //centers the cursor
		}
		else if (y < 200 || y > 480 - 200) {
			lastX = 640 / 2;   
			lastY = 480 / 2;
			glutWarpPointer(640 / 2, 480 / 2);
		}
		
}

void Application::mousePress(int button)
{
  mouseButtons[button] = true;

}

void Application::mouseRelease(int button)
{
  mouseButtons[button] = false;
  if (!mouseButtons[0] && !mouseButtons[1])
  {
	  lastMousePos = glm::ivec2(-1, -1);
  }
}

bool Application::getKey(int key) const
{
	return keys[key];
}

bool Application::getSpecialKey(int key) const
{
	return specialKeys[key];
}


