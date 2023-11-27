#include "..\include\fe_debugInterface.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <fstream>
#include <fe_input.h>
#include "fe_constants.h"


FEScene::FEScene() {
  active_voxel_ = 0;
  active_face_ = 0;
  wireframe_ = false;
  fps_ = 1.0f;
  world_made_ = false;
  world_voxel_per_row_ = 1;
  offset_ = false;
  
  std::strcpy(csv_file_name_, "../data/test.csv");
}

FEScene::~FEScene() {

}

void FEScene::Update(GLfloat deltaTime, FERender& render, FEWindow& window ) {
  
  world_.ColourPicking(render.colour_id_, render.destroy_);
  Interface(deltaTime, render, window );
}


void FEScene::Interface(GLfloat deltaTime, FERender& render, FEWindow& window) {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
  if (world_made_) {
    

    ImGui::SetNextWindowPos(ImVec2(kWindowCenterWidth - 2.0f, kWindowCenterHeight - 2.0f), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(2.0f, 2.0f), ImGuiCond_Always);

    ImGui::Begin("Center", NULL, ImGuiWindowFlags_NoResize);

    ImGui::End();
  }
  



  ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
  ImGui::SetNextWindowSize(ImVec2(window_size_x, window_size_y), ImGuiCond_Always);

  ImGui::Begin("Voxel Debug", NULL, ImGuiWindowFlags_NoResize);

  if( FEInput::mouseKeyPress( Mouse::KEY_MOUSE_LEFT ) ) {
    world_.CollisionDetection(render,true);
  }

  if (FEInput::mouseKeyPress(Mouse::KEY_MOUSE_RIGHT)) {
    world_.CollisionDetection( render, false);
  }

  if (world_made_) {


    if (render.colour_picking_) {
      ImVec2 wsize = { 200,200 };
      ImGui::Image((ImTextureID)render.color_picking_buffer_.texture_id_,
        wsize, { 0,1 }, { 1,0 });
    }
    

    fps_ = 1.0f / deltaTime;
    ImGui::Text("FPS : %03.00f", fps_);
    ImGui::Text("Time Per Frame : %03.00fms", (1.0f/fps_) * 1000.0f);
    ImGui::Text("Active Triangles : %d", world_.active_faces_ * 2);
    ImGui::Text("Active Faces : %d", world_.active_faces_ );
    ImGui::Text("World Generation : %d ms",
      world_.ms_for_chunk_creation_);

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
      delete[] world_.voxel_list_;
      world_.voxel_list_ = nullptr;
      world_.voxel_in_total_ = 0;
      world_.active_faces_ = 0;
      world_made_ = false;
    }
  }
  else {
    ImGui::Checkbox("Culling",&world_.culling_);
    //ImGui::Checkbox("Greedy Meshing", &world_.greedy_meshing_);
    if (ImGui::Checkbox("Colour Picking", &render.colour_picking_)) {
      render.colour_picking_ == true ? 
        world_.collision_detection_ = false : world_.collision_detection_ = true;
    }

    if (ImGui::Checkbox("Collision Detection", &world_.collision_detection_)) {
      world_.collision_detection_ == true ?
        render.colour_picking_ = false : render.colour_picking_ = true;
    }
    
    if (ImGui::Checkbox("Offset",&offset_)) {
      offset_ == true ? world_.offset_ = 1.5f : world_.offset_ = 1.0f;

    }


    ImGui::Text("VoxelPerRow");
    if (ImGui::InputInt("##VoxelPerRow", &world_voxel_per_row_)) {
      if (world_voxel_per_row_ < 1) {
        world_voxel_per_row_ = 1;
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

  ImGui::SetNextWindowPos(ImVec2(0, window_size_y), ImGuiCond_Always);
  ImGui::SetNextWindowSize(ImVec2(window_size_x, window_size_y), ImGuiCond_Always);

  ImGui::Begin("CSV File Maker", NULL, ImGuiWindowFlags_NoResize);
  ImGui::InputText("CSV", csv_file_name_, csv_file_name_size_);
  if (ImGui::Button("Make CSV File")) {
    CSVMaker(render);
  }
  if (ImGui::Button("Update CSV File")) {
    CSVUpdate(render);
  }

  ImGui::End();
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void FEScene::CSVMaker(FERender& render) {
  std::ofstream myfile;
  myfile.open(csv_file_name_);

  myfile << "Culling,"
    << "Greedy Meshing,"
    << "Colour Picking,"
    << "Voxel In Scene,"
    << "Active Voxels,"
    << "Active Faces,"
    << "Active Triangles,"
    << "FPS,"
    << "Time Per Frame,"
    << "Time Taken for World Generation,"
    << "\n";

  //Culling
  world_.culling_ == true ? myfile << "True," : myfile << "False,";
  //Greedy Meshing
  world_.greedy_meshing_ == true ? myfile << "True," : myfile << "False,";
  //Colour Picking
  render.colour_picking_ == true ? myfile << "True," : myfile << "False,";
  //Voxels in Scene
  myfile << (world_.voxel_per_row_ * world_.voxel_per_row_ * world_.voxel_per_row_) << ",";
  //Active Voxels
  myfile << world_.active_faces_ / 6 << ",";
  //Active faces
  myfile << world_.active_faces_ << ",";
  //Active Triangles
  myfile << world_.active_faces_ * 2 << ",";
  //FPS
  myfile << fps_ << ",";
  //Times Per Frame
  myfile << (1.0f / fps_) * 1000.0f << ",";
  //Time Taken For World Generation
  myfile << world_.ms_for_chunk_creation_ << "\n";


  myfile.close();


}

void FEScene::CSVUpdate(FERender& render) {
  std::ofstream myfile;
  myfile.open(csv_file_name_, std::ofstream::out | std::ofstream::app);

  //Culling
  world_.culling_ == true ? myfile << "True," : myfile << "False,";
  //Greedy Meshing
  world_.greedy_meshing_ == true ? myfile << "True," : myfile << "False,";
  //Colour Picking
  render.colour_picking_ == true ? myfile << "True," : myfile << "False,";
  //Voxels in Scene
  myfile << (world_.voxel_per_row_ * world_.voxel_per_row_ * world_.voxel_per_row_) << ",";
  //Active Voxels
  myfile << world_.active_faces_ / 6 << ",";
  //Active faces
  myfile << world_.active_faces_ << ",";
  //Active Triangles
  myfile << world_.active_faces_ * 2 << ",";
  //FPS
  myfile << fps_ << ",";
  //Times Per Frame
  myfile << (1.0f / fps_) * 1000.0f << ",";
  //Time Taken For World Generation
  myfile << world_.ms_for_chunk_creation_ << "\n";


  myfile.close();
}

