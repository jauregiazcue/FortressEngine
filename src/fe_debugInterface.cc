#include "..\include\fe_debugInterface.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

// Author : jauregiaz

FEDebugInterface::FEDebugInterface() {
  active_voxel_ = 0;
  active_face_ = 0;
  wireframe_ = false;
  fps = 0;
}

FEDebugInterface::~FEDebugInterface() {

}

void FEDebugInterface::Draw(FEWorld& world, GLfloat deltaTime) {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();

  ImGui::NewFrame();

  ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
  ImGui::SetNextWindowSize(ImVec2(200, 800), ImGuiCond_Always);

  ImGui::Begin("Voxel Debug", NULL, ImGuiWindowFlags_NoResize);

  fps = 1.0f / deltaTime;

  ImGui::Text("FPS : %03.00f",fps);
  ImGui::Text("Active Triangles : %d", world.active_triangles_);
  ImGui::Text("Active Faces : %d", world.active_triangles_ / 2);
  ImGui::Text("World Generation : %d ms", 
               world.ms_for_chunk_creation_);

  if (ImGui::InputInt("Voxel", &active_voxel_)) {
    if (active_voxel_ < 0) {
      active_voxel_ = 0;
    }

    if (active_voxel_ >= world.transform_list_.size()) {
      active_voxel_ = world.transform_list_.size() - 1;
    }
  }
  ImGui::SliderInt("Face", &active_face_, 0, 5);

  if (ImGui::Button("Change Face State")) {
      world.voxel_list_[active_voxel_].faces_[active_face_].active_ == false ?
        world.voxel_list_[active_voxel_].faces_[active_face_].active_ = true :
        world.voxel_list_[active_voxel_].faces_[active_face_].active_ = false;
  }

  if (ImGui::Button("Wireframe")) {
    if (wireframe_) {
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      wireframe_ = false;
    }
    else {
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      wireframe_ = true;
    }
  }

  ImGui::End();

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
