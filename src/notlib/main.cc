#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "fe_window.h"

#include "fe_transform.h"
#include "fe_material.h"
#include "fe_program.h"
#include "fe_entity.h"
#include "fe_render.h"

#include "fe_voxel.h"

#include <glm/gtc/type_ptr.hpp>



int main(void) {

  FEWindow fe_window{ {1.0f,1.0f,1.0f,1.0f} };
  if (fe_window.init(1280, 700) != 0) {
    return -1;
  }

  FEVoxel voxel{ FETransformComponent{ { 0.0f,0.0f,0.0f },
                                       { 0.0f,0.0f,0.0f },
                                       { 1.0f,1.0f,1.0f } } };

  FERender renderer;
  renderer.SetCameraPosition({ 0.0f,0.0f,5.0f });

  while (!fe_window.isDone()) {
    fe_window.clear();

    renderer.DebugCameraMovement();
    renderer.Render(voxel.faces_);

    fe_window.swap();
  }

  return 0;
}
