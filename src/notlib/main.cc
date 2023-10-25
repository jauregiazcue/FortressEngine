#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "fe_window.h"

#include "fe_transform.h"
#include "fe_material.h"
#include "fe_program.h"
#include "fe_entity.h"
#include "fe_render.h"

#include "fe_voxel.h"
#include "fe_input.h"

#include <glm/gtc/type_ptr.hpp>

void FaceActiveTestInput(enum Key keyType, FEEntity& face_entity) {
  if (FEInput::keyPress(keyType)) {
    face_entity.active_ == false ? face_entity.active_ = true :
      face_entity.active_ = false;
  }
}

int main(void) {

  FEWindow fe_window{ {1.0f,1.0f,1.0f,1.0f} };
  if (fe_window.init(1280, 700) != 0) {
    return -1;
  }

  

  std::vector<FEVoxel> voxel_list;
  for (int y = 0; y < 5; y++) {
    for (int z = 0; z < 5; z++) {
      for (int x = 0; x < 5; x++) {
        voxel_list.push_back(FEVoxel{ FETransformComponent{ 
                              { (float)x * 1.5f,-(float)y * 1.5f,-(float)z * 1.5f },
                                                            { 0.0f,0.0f,0.0f },
                                                            { 1.0f,1.0f,1.0f } } });
      }
    }
  }
  

  FERender renderer;
  renderer.SetCameraPosition({ 0.0f,0.0f,5.0f });

  
  

  bool wireframe = false;

  while (!fe_window.isDone()) {
    fe_window.clear();

    if (FEInput::keyPress(Key::KEY_1)) {
      if (wireframe) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        wireframe = false;
      }
      else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        wireframe = true;
      }
    }

    FaceActiveTestInput(Key::KEY_2, voxel_list[0].faces_[0]);
    FaceActiveTestInput(Key::KEY_3, voxel_list[0].faces_[1]);
    FaceActiveTestInput(Key::KEY_4, voxel_list[0].faces_[2]);
    FaceActiveTestInput(Key::KEY_5, voxel_list[0].faces_[3]);
    FaceActiveTestInput(Key::KEY_6, voxel_list[0].faces_[4]);
    FaceActiveTestInput(Key::KEY_7, voxel_list[0].faces_[5]);




    renderer.DebugCameraMovement();
    renderer.Render(voxel_list);

    fe_window.swap();
  }

  return 0;
}
