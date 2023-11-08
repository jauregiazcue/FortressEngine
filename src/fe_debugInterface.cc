#include "..\include\fe_debugInterface.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

// Author : jauregiaz

FEScene::FEScene() {
  active_voxel_ = 0;
  active_face_ = 0;
  wireframe_ = false;
  fps_ = 0;
  world_made_ = false;
  world_voxel_per_row_ = 1;
  
}

FEScene::~FEScene() {

}

void FEScene::Update(GLfloat deltaTime, FERender& render) {
  
  world_.ColourPicking(render.colour_id_, render.destroy_);
  Interface(deltaTime, render);
}


void FEScene::Interface(GLfloat deltaTime, FERender& render) {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();

  ImGui::NewFrame();

  ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
  ImGui::SetNextWindowSize(ImVec2(window_size_x, window_size_y), ImGuiCond_Always);

  ImGui::Begin("Voxel Debug", NULL, ImGuiWindowFlags_NoResize);

  

  if (world_made_) {

    ImVec2 wsize = { 200,200 };
    ImGui::Image((ImTextureID)render.color_picking_buffer_.texture_id_,
      wsize, { 0,1 }, { 1,0 });

    fps_ = 1.0f / deltaTime;
    ImGui::Text("FPS : %03.00f", fps_);
    ImGui::Text("Active Triangles : %d", world_.active_faces_ * 2);
    ImGui::Text("Active Faces : %d", world_.active_faces_ );
    ImGui::Text("World Generation : %d ms",
      world_.ms_for_chunk_creation_);

    if (ImGui::InputInt("Voxel", &active_voxel_)) {
      if (active_voxel_ < 0) {
        active_voxel_ = 0;
      }

      if (active_voxel_ >= world_.transform_list_.size()) {
        active_voxel_ = world_.transform_list_.size() - 1;
      }
    }
    ImGui::SliderInt("Face", &active_face_, 0, 5);

    if (ImGui::Button("Change Face State")) {
      world_.voxel_list_[active_voxel_].faces_[active_face_].active_ == false ?
        world_.voxel_list_[active_voxel_].faces_[active_face_].active_ = true :
        world_.voxel_list_[active_voxel_].faces_[active_face_].active_ = false;
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

    if (ImGui::Button("Destroy World")) {
      world_.voxel_list_.clear();
      world_.transform_list_.clear();
      world_.active_faces_ = 0;
      world_made_ = false;
    }
  }
  else {
    ImGui::Text("Culling");
    if (ImGui::Button("True")) {
      world_.culling_ = true;
    }
    ImGui::SameLine();
    if (ImGui::Button("False")) {
      world_.culling_ = false;
    }

    ImGui::Text( "Greedy Meshing" );
    if( ImGui::Button( "True###GreedyTrue" ) ) {
      world_.greedy_meshing_ = true;
    }
    ImGui::SameLine();
    if( ImGui::Button( "False###GreedyFalse" ) ) {
      world_.greedy_meshing_ = false;
    }

    ImGui::Text("Offset");
    if (ImGui::Button("True###OffsetTrue")) {
      world_.offset_ = 1.5f;
    }
    ImGui::SameLine();
    if (ImGui::Button("False###OffsetFalse")) {
      world_.offset_ = 1.0f;
    }

    ImGui::Text("Colour Picking");
    if (ImGui::Button("True###ColourTrue")) {
      render.colour_picking_ = true;
    }
    ImGui::SameLine();
    if (ImGui::Button("False###ColourFalse")) {
      render.colour_picking_ = false;
    }

    ImGui::Text("VoxelPerRow");
    if (ImGui::InputInt("##VoxelPerRow", &world_voxel_per_row_)) {
      if (world_voxel_per_row_ < 0) {
        world_voxel_per_row_ = 0;
      }

      if (world_voxel_per_row_ > 999) {
        world_voxel_per_row_ = 999;
      }
    }

    if (ImGui::Button("Create World")) {
      world_.init(world_voxel_per_row_);
      world_made_ = true;
    }
  }



  ImGui::End();

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

