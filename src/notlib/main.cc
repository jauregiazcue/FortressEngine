#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "fe_window.h"

#include "fe_transform.h"
#include "fe_material.h"
#include "fe_program.h"
#include "fe_render.h"

#include "fe_input.h"
#include "fe_debugInterface.h"

#include <glm/gtc/type_ptr.hpp>
#include <fe_world.h>



int main(void) {

  FEWindow fe_window{ {1.0f,1.0f,1.0f,1.0f} };
  if (fe_window.init() != 0) {
    return -1;
  }

  FEWorld chunk{ 20 };

  FERender renderer;
  renderer.SetCameraPosition({ 0.0f,0.0f,5.0f });

  
  

  bool wireframe = false;

  FEDebugInterface debugInterface;

  GLfloat deltaTime = 0.0f;
  GLfloat lastFrame = 0.0f;

  while (!fe_window.isDone()) {
    fe_window.clear();
    GLfloat currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;


    renderer.DebugCameraMovement();
    renderer.Render(chunk);
    debugInterface.Draw(chunk,deltaTime);

    fe_window.swap();
  }

  return 0;
}
