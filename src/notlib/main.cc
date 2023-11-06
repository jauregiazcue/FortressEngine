#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "fe_window.h"
#include "fe_render.h"
#include "fe_debugInterface.h"




int main(void) {

  FEWindow fe_window;
  if (fe_window.init() != 0) {
    return -1;
  }

  FERender renderer;
  renderer.SetCameraPosition({ 0.0f,0.0f,5.0f });

  bool wireframe = false;

  FEScene scene;

  GLfloat deltaTime = 0.0f;
  GLfloat lastFrame = 0.0f;

  while (!fe_window.isDone()) {
    fe_window.clear();
    GLfloat currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    renderer.DebugCameraMovement();
    renderer.Render(scene.world_,fe_window);
    scene.Update(deltaTime,renderer);

    fe_window.swap();
  }

  return 0;
}
