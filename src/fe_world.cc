#include "fe_world.h"
#include "fe_constants.h"

#include <GLFW/glfw3.h>


FEWorld::FEWorld() {
  

   std::vector<FEMaterialComponent::Vertex> vertices = initFrontFace();

  std::vector<int> index = { 0, 1, 2 ,
                            3,2,1 };

  std::shared_ptr<FEProgram> pro = std::make_shared<FEProgram>(std::vector<FEShader>{
    FEShader("../src/shaders/common.vert", ShaderType::Vertex),
      FEShader("../src/shaders/common.frag", ShaderType::Fragment) });

  material_list_.push_back({ FEMaterialComponent{ pro,vertices,index } });

  vertices = initLeftFace();
  material_list_.push_back({ FEMaterialComponent{ pro,vertices,index } });

  vertices = initBackFace();
  material_list_.push_back({ FEMaterialComponent{ pro,vertices,index } });

  vertices = initRightFace();
  material_list_.push_back({ FEMaterialComponent{ pro,vertices,index } });

  vertices = initTopFace();
  material_list_.push_back({ FEMaterialComponent{ pro,vertices,index } });

  vertices = initBottomFace();
  material_list_.push_back({ FEMaterialComponent{ pro,vertices,index } });

  active_faces_ = 0;
  ms_for_chunk_creation_ = 0.0f;
  voxel_per_row_ = 0;
  offset_ = 1.0f;

  culling_ = true;
  greedy_meshing_ = true;
}

FEWorld::~FEWorld() {

}

void FEWorld::init(int voxelPerRow) {
  voxel_per_row_ = voxelPerRow;
  voxel_per_row_and_colum_ = voxel_per_row_ * voxel_per_row_;

  std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

  createChunks();

  std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

  ms_for_chunk_creation_ =
    std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();

  if (culling_) {
    Culling();
  }

  if( greedy_meshing_ ) {
    GreedyMeshing();
  }
  
}

void FEWorld::createChunks() {
  
  for (int x = 0; x < voxel_per_row_; x++) {
    for (int y = 0; y < voxel_per_row_; y++) {
      for (int z = 0; z < voxel_per_row_; z++) {
        transform_list_.push_back(FETransformComponent{
                              { (float)x * offset_,(float)-y * offset_,(float)-z * offset_},
                              { 0.0f,0.0f,0.0f },
                              { 1.0f,1.0f,1.0f } });

        
      }
    }
  }

  int voxel_id_ = 0;


  for (int x = 0; x < voxel_per_row_; x++) {
    for (int y = 0; y < voxel_per_row_; y++) {
      for (int z = 0; z < voxel_per_row_; z++) {
        glm::vec3 position{( float )x* offset_, ( float )-y * offset_, ( float )-z * offset_};
        Faces* faces = GetFaces(voxel_id_, position );
        voxel_list_.push_back({ voxel_id_,FEWorld::VoxelType::block,
          {faces[FRONTFACE],faces[LEFTFACES],faces[BACKFACES],
           faces[RIGHTFACES],faces[TOPFACES],faces[BOTTOMFACES]} });
        
        active_faces_ += FACES;
        voxel_id_ += 1;
      }
    }
  }

  
}

FEWorld::Faces* FEWorld::GetFaces(int voxel_id, glm::vec3 position ) {
  Faces faces_[FACES];

  for (int i = 0; i < FACES; i++) {
    faces_[i].material_id_ = i;
    faces_[i].active_ = true;
    faces_[i].colour_picking_active_ = true;

    int color_id_helper = (voxel_id * FACES) + i;
    faces_[i].real_color_id_ = color_id_helper;
    faces_[i].color_id_ =    { ((color_id_helper & 0x000000FF) >> 0)  / 255.0f ,
                               ((color_id_helper & 0x0000FF00) >> 8)  / 255.0f ,
                               ((color_id_helper & 0x00FF0000) >> 16) / 255.0f };
    faces_[i].transform_.setPosition( position );

  }

  return faces_;
}

void FEWorld::DrawVoxel(int voxel_id_, glm::mat4 projection, glm::mat4 view) {
  if (voxel_list_[voxel_id_].type_ != VoxelType::air) {
    DrawFace(voxel_id_, FRONTFACE, projection, view);
    DrawFace(voxel_id_, LEFTFACES, projection, view);
    DrawFace(voxel_id_, BACKFACES, projection, view);
    DrawFace(voxel_id_, RIGHTFACES, projection, view);
    DrawFace(voxel_id_, TOPFACES, projection, view);
    DrawFace(voxel_id_, BOTTOMFACES, projection, view);
  }
  
}


void FEWorld::DrawFace(int voxel_id_,int face_id_, glm::mat4 projection, glm::mat4 view) {
  Faces& drawing_face = voxel_list_[voxel_id_].faces_[face_id_];
  if ( drawing_face.active_) {
    FEMaterialComponent& material = 
      material_list_[drawing_face.material_id_];

    material.enable();

    material.setUpReferenceUniform("PickingColour",
      drawing_face.color_id_);

    material.setUpModel( drawing_face.transform_.getTransform());

    material.setUpCamera(projection, view);

    material.bindAndRender();
  }
}

void FEWorld::DrawVoxelForColourPicking(int voxel_id_, glm::mat4 projection, glm::mat4 view, int program_id) {
  if (voxel_list_[voxel_id_].type_ != VoxelType::air) {
    DrawFaceForColourPicking(voxel_id_, FRONTFACE, projection, view, program_id);
    DrawFaceForColourPicking(voxel_id_, LEFTFACES, projection, view, program_id);
    DrawFaceForColourPicking(voxel_id_, BACKFACES, projection, view, program_id);
    DrawFaceForColourPicking(voxel_id_, RIGHTFACES, projection, view, program_id);
    DrawFaceForColourPicking(voxel_id_, TOPFACES, projection, view, program_id);
    DrawFaceForColourPicking(voxel_id_, BOTTOMFACES,projection, view, program_id);
  }
  
}

void FEWorld::DrawFaceForColourPicking(int voxel_id_, int face_id_, glm::mat4 projection, glm::mat4 view, int program_id) {
  Faces& drawing_face = voxel_list_[voxel_id_].faces_[face_id_];
  if ( drawing_face.colour_picking_active_ ) {
    FEMaterialComponent& material =
      material_list_[drawing_face.material_id_];

    material.enableWithOtherProgram(program_id);

    material.setUpReferenceUniformWithOtherProgram("PickingColour",
      drawing_face.color_id_, program_id);

    material.setUpModelWithOtherProgram(
      transform_list_[voxel_id_].getTransform(), program_id);

    material.setUpCameraWithOtherProgram(projection, view, program_id);

    material.bindAndRender();
  }
}


void FEWorld::Culling() {
  for (int i = 0; i < voxel_list_.size(); i++) {
    CheckFaces(i);
  }
}

void FEWorld::GreedyMeshing() {
  if (voxel_list_.size() > 1) {
    glm::vec3 last_position_;
    glm::vec3 first_position_ = transform_list_[voxel_list_[0].voxel_id_].getPosition();
    for (int i = 1; i < voxel_per_row_and_colum_; i++) {
      voxel_list_[i].faces_[LEFTFACES].active_ = false;
      last_position_ = transform_list_[voxel_list_[i].voxel_id_].getPosition();
    }
    glm::vec3 center_of_face{
      (first_position_.x + last_position_.x) / 2,
      (first_position_.y + last_position_.y) / 2,
      (first_position_.z + last_position_.z) / 2, };

    voxel_list_[0].faces_[LEFTFACES].transform_.setPosition(center_of_face);
    float new_scale = (float)voxel_per_row_;
    voxel_list_[0].faces_[LEFTFACES].transform_.setScale({ 1.0f,new_scale,new_scale });

  }
  
}

void FEWorld::CheckFaces(int voxel_to_check) {

  if (voxel_to_check - 1 >= 0 && (voxel_to_check % voxel_per_row_) != 0) {
    voxel_list_[voxel_to_check].faces_[FRONTFACE].active_ = false;
    voxel_list_[voxel_to_check].faces_[FRONTFACE].colour_picking_active_ = false;
    active_faces_ -= 1;

  }

  if (voxel_to_check + 1 < voxel_list_.size() 
    && ((voxel_to_check + 1) % voxel_per_row_) != 0) {
    voxel_list_[voxel_to_check].faces_[BACKFACES].active_ = false;
    voxel_list_[voxel_to_check].faces_[BACKFACES].colour_picking_active_ = false;
    active_faces_ -= 1;

  }
  
  int voxel_to_check_2 = voxel_to_check % (voxel_per_row_and_colum_);
  if (voxel_to_check_2  >= 0  && voxel_to_check_2 >= voxel_per_row_) {
    voxel_list_[voxel_to_check].faces_[TOPFACES].active_ = false;
    voxel_list_[voxel_to_check].faces_[TOPFACES].colour_picking_active_ = false;
    active_faces_ -= 1;

  }
  
  int last_row = (( voxel_per_row_and_colum_ ) - voxel_per_row_);
  if (voxel_to_check_2 < voxel_list_.size() 
    && voxel_to_check_2 < last_row) {
    voxel_list_[voxel_to_check].faces_[BOTTOMFACES].active_ = false;
    voxel_list_[voxel_to_check].faces_[BOTTOMFACES].colour_picking_active_ = false;
    active_faces_ -= 1;

  }

  if (voxel_to_check >= (voxel_per_row_and_colum_)) {
    voxel_list_[voxel_to_check].faces_[LEFTFACES].active_ = false;
    voxel_list_[voxel_to_check].faces_[LEFTFACES].colour_picking_active_ = false;
    active_faces_ -= 1;

  }

  if (voxel_to_check < ((voxel_per_row_and_colum_ * voxel_per_row_) - (voxel_per_row_and_colum_))) {
    voxel_list_[voxel_to_check].faces_[RIGHTFACES].active_ = false;
    voxel_list_[voxel_to_check].faces_[RIGHTFACES].colour_picking_active_ = false;
    active_faces_ -= 1;

  } 
}



void FEWorld::ColourPicking( int colour_id,bool destroy) {
  
  if (colour_id != -1) {
    //If the id is found, the voxel will be destroy or place
    for (int i = 0; i < voxel_list_.size(); i++) {
      for (int x = 0; x < FACES; x++) {
        if (voxel_list_[i].type_ != VoxelType::air) {
          if (voxel_list_[i].faces_[x].real_color_id_ == colour_id) {
            if (destroy) {
              DestroyVoxel(i);
              if (culling_) UpdateAdjacentFacesWhenDestroy(i);
              return;
            }
            else {
              PlaceVoxel(i,x);
              return;
            }
          }
        }
      }
    }
  }
  
}

void FEWorld::DestroyVoxel(int voxel_id) {
  voxel_list_[voxel_id].type_ = VoxelType::air;
  if (voxel_list_[voxel_id].faces_[FRONTFACE].active_) {
    voxel_list_[voxel_id].faces_[FRONTFACE].active_ = false;
    voxel_list_[voxel_id].faces_[FRONTFACE].colour_picking_active_ = false;
    active_faces_ -= 1;
  }

  if (voxel_list_[voxel_id].faces_[LEFTFACES].active_) {
    voxel_list_[voxel_id].faces_[LEFTFACES].active_ = false;
    voxel_list_[voxel_id].faces_[LEFTFACES].colour_picking_active_ = false;
    active_faces_ -= 1;
  }

  if (voxel_list_[voxel_id].faces_[BACKFACES].active_) {
    voxel_list_[voxel_id].faces_[BACKFACES].active_ = false;
    voxel_list_[voxel_id].faces_[BACKFACES].colour_picking_active_ = false;
    active_faces_ -= 1;
  }

  if (voxel_list_[voxel_id].faces_[RIGHTFACES].active_) {
    voxel_list_[voxel_id].faces_[RIGHTFACES].active_ = false;
    voxel_list_[voxel_id].faces_[RIGHTFACES].colour_picking_active_ = false;
    active_faces_ -= 1;
  }

  if (voxel_list_[voxel_id].faces_[TOPFACES].active_) {
    voxel_list_[voxel_id].faces_[TOPFACES].active_ = false;
    voxel_list_[voxel_id].faces_[TOPFACES].colour_picking_active_ = false;
    active_faces_ -= 1;
  }
  
  if (voxel_list_[voxel_id].faces_[BOTTOMFACES].active_) {
    voxel_list_[voxel_id].faces_[BOTTOMFACES].active_ = false;
    voxel_list_[voxel_id].faces_[BOTTOMFACES].colour_picking_active_ = false;
    active_faces_ -= 1;
  }
  
}

void FEWorld::UpdateAdjacentFacesWhenDestroy(int voxel_to_check) {
  //FRONT FACE OF THE BACK VOXEL OF THE VOXEL THAT IS BEING ELIMINATED
  int new_voxel_to_check = voxel_to_check + 1;
  if (new_voxel_to_check < voxel_list_.size()) {
    if (new_voxel_to_check % voxel_per_row_ != 0
      && voxel_list_[new_voxel_to_check].type_ != VoxelType::air) {
      voxel_list_[new_voxel_to_check].faces_[FRONTFACE].active_ = true;
      voxel_list_[new_voxel_to_check].faces_[FRONTFACE].colour_picking_active_ = true;
      active_faces_ += 1;
    }
  }
  //BACK FACE OF THE FRONT VOXEL OF THE VOXEL THAT IS BEING ELIMINATED
  new_voxel_to_check = voxel_to_check - 1;
  if (new_voxel_to_check >= 0 ) {
    if ((voxel_to_check % voxel_per_row_) != 0 &&
      voxel_list_[new_voxel_to_check].type_ != VoxelType::air) {
      voxel_list_[new_voxel_to_check].faces_[BACKFACES].active_ = true;
      voxel_list_[new_voxel_to_check].faces_[BACKFACES].colour_picking_active_ = true;
      active_faces_ += 1;
    }
  }
  //TOP FACE OF THE BOTTOM VOXEL OF THE VOXEL THAT IS BEING ELIMINATED
  new_voxel_to_check = voxel_to_check + voxel_per_row_;
  if (new_voxel_to_check < voxel_list_.size()) {
    if (voxel_list_[new_voxel_to_check].type_ != VoxelType::air) {
      voxel_list_[new_voxel_to_check].faces_[TOPFACES].active_ = true;
      voxel_list_[new_voxel_to_check].faces_[TOPFACES].colour_picking_active_ = true;
      active_faces_ += 1;
    }
  }
  //BOTTOM FACE OF THE TOP VOXEL OF THE VOXEL THAT IS BEING ELIMINATED
  new_voxel_to_check = voxel_to_check - voxel_per_row_;
  if (new_voxel_to_check >= 0) {
    if (voxel_list_[new_voxel_to_check].type_ != VoxelType::air) {
      voxel_list_[new_voxel_to_check].faces_[BOTTOMFACES].active_ = true;
      voxel_list_[new_voxel_to_check].faces_[BOTTOMFACES].colour_picking_active_ = true;
      active_faces_ += 1;
    }
  }
  //LEFT FACE OF THE RIGHT VOXEL OF THE VOXEL THAT IS BEING ELIMINATED
  new_voxel_to_check = voxel_to_check + (voxel_per_row_and_colum_);
  if (new_voxel_to_check < voxel_list_.size()) {
    if (voxel_list_[new_voxel_to_check].type_ != VoxelType::air) {
      voxel_list_[new_voxel_to_check].faces_[LEFTFACES].active_ = true;
      voxel_list_[new_voxel_to_check].faces_[LEFTFACES].colour_picking_active_ = true;
      active_faces_ += 1;
    }
  }
  //RIGHT FACE OF THE LEFT VOXEL OF THE VOXEL THAT IS BEING ELIMINATED
  new_voxel_to_check = voxel_to_check - (voxel_per_row_and_colum_);
  if (new_voxel_to_check >= 0) {
    if (voxel_list_[new_voxel_to_check].type_ != VoxelType::air) {
      voxel_list_[new_voxel_to_check].faces_[RIGHTFACES].active_ = true;
      voxel_list_[new_voxel_to_check].faces_[RIGHTFACES].colour_picking_active_ = true;
      active_faces_ += 1;
    }
  }
}

void FEWorld::PlaceVoxel(int voxel_id, int face_id) {
  int new_voxel_id = -1;
  switch (face_id) {
  case FRONTFACE: 
    new_voxel_id = voxel_id - 1;
    if ((voxel_id % voxel_per_row_) == 0) return;
    
    break;
  case LEFTFACES: 
    new_voxel_id = voxel_id - (voxel_per_row_and_colum_);
    break;
  case BACKFACES: 
    new_voxel_id = voxel_id + 1;
    if ((new_voxel_id % voxel_per_row_) == 0) return;
    break;
  case RIGHTFACES: 
    new_voxel_id = voxel_id + (voxel_per_row_and_colum_);
    break;
  case TOPFACES: 
    if (voxel_id % (voxel_per_row_and_colum_) < voxel_per_row_) return;
    new_voxel_id = voxel_id - voxel_per_row_;
    break;
  case BOTTOMFACES:
    if ((voxel_id + voxel_per_row_) % (voxel_per_row_and_colum_) < voxel_per_row_) return;
    new_voxel_id = voxel_id + voxel_per_row_;
    break;
  }

  if (new_voxel_id >= 0 && new_voxel_id < voxel_list_.size()) {
    voxel_list_[new_voxel_id].type_ = VoxelType::block;
    voxel_list_[new_voxel_id].faces_[FRONTFACE].active_ = true;
    voxel_list_[new_voxel_id].faces_[LEFTFACES].active_ = true;
    voxel_list_[new_voxel_id].faces_[BACKFACES].active_ = true;
    voxel_list_[new_voxel_id].faces_[RIGHTFACES].active_ = true;
    voxel_list_[new_voxel_id].faces_[TOPFACES].active_ = true;
    voxel_list_[new_voxel_id].faces_[BOTTOMFACES].active_ = true;

    voxel_list_[new_voxel_id].faces_[FRONTFACE].colour_picking_active_ = true;
    voxel_list_[new_voxel_id].faces_[LEFTFACES].colour_picking_active_ = true;
    voxel_list_[new_voxel_id].faces_[BACKFACES].colour_picking_active_ = true;
    voxel_list_[new_voxel_id].faces_[RIGHTFACES].colour_picking_active_ = true;
    voxel_list_[new_voxel_id].faces_[TOPFACES].colour_picking_active_ = true;
    voxel_list_[new_voxel_id].faces_[BOTTOMFACES].colour_picking_active_ = true;
    active_faces_ += FACES;
    if(culling_) UpdateAdjacentFacesWhenPlace(new_voxel_id);
  }

}

void FEWorld::UpdateAdjacentFacesWhenPlace(int voxel_to_check) {
  //FRONT FACE OF THE BACK VOXEL OF THE VOXEL THAT IS BEING PLACED
  int new_voxel_to_check = voxel_to_check + 1;
  if (new_voxel_to_check < voxel_list_.size()) {
    if (new_voxel_to_check % voxel_per_row_ != 0
      && voxel_list_[new_voxel_to_check].type_ != VoxelType::air) {
      voxel_list_[new_voxel_to_check].faces_[FRONTFACE].active_ = false;
      voxel_list_[voxel_to_check].faces_[BACKFACES].active_ = false;

      voxel_list_[new_voxel_to_check].faces_[FRONTFACE].colour_picking_active_ = false;
      voxel_list_[voxel_to_check].faces_[BACKFACES].colour_picking_active_ = false;
      active_faces_ -= 2;
    }
  }
  //BACK FACE OF THE FRONT VOXEL OF THE VOXEL THAT IS BEING PLACED
  new_voxel_to_check = voxel_to_check - 1;
  if (new_voxel_to_check >= 0) {
    if ((voxel_to_check % voxel_per_row_) != 0 &&
      voxel_list_[new_voxel_to_check].type_ != VoxelType::air) {
      voxel_list_[new_voxel_to_check].faces_[BACKFACES].active_ = false;
      voxel_list_[voxel_to_check].faces_[FRONTFACE].active_ = false;

      voxel_list_[new_voxel_to_check].faces_[BACKFACES].colour_picking_active_ = false;
      voxel_list_[voxel_to_check].faces_[FRONTFACE].colour_picking_active_ = false;
      active_faces_ -= 2;
    }
  }
  //TOP FACE OF THE BOTTOM VOXEL OF THE VOXEL THAT IS BEING PLACED
  new_voxel_to_check = voxel_to_check + voxel_per_row_;
  if (new_voxel_to_check < voxel_list_.size() && new_voxel_to_check % (voxel_per_row_and_colum_) >= voxel_per_row_) {
    if (voxel_list_[new_voxel_to_check].type_ != VoxelType::air) {
      voxel_list_[new_voxel_to_check].faces_[TOPFACES].active_ = false;
      voxel_list_[voxel_to_check].faces_[BOTTOMFACES].active_ = false;

      voxel_list_[new_voxel_to_check].faces_[TOPFACES].colour_picking_active_ = false;
      voxel_list_[voxel_to_check].faces_[BOTTOMFACES].colour_picking_active_ = false;
      active_faces_ -= 2;
    }
  }
  //BOTTOM FACE OF THE TOP VOXEL OF THE VOXEL THAT IS BEING PLACED
  new_voxel_to_check = voxel_to_check - voxel_per_row_;
  if (new_voxel_to_check >= 0 
    && new_voxel_to_check % (voxel_per_row_and_colum_) < ((voxel_per_row_and_colum_ ) - voxel_per_row_)) {
    if (voxel_list_[new_voxel_to_check].type_ != VoxelType::air) {
      voxel_list_[new_voxel_to_check].faces_[BOTTOMFACES].active_ = false;
      voxel_list_[voxel_to_check].faces_[TOPFACES].active_ = false;

      voxel_list_[new_voxel_to_check].faces_[BOTTOMFACES].colour_picking_active_ = false;
      voxel_list_[voxel_to_check].faces_[TOPFACES].colour_picking_active_ = false;
      active_faces_ -= 2;
    }
  }
  //LEFT FACE OF THE RIGHT VOXEL OF THE VOXEL THAT IS BEING PLACED
  new_voxel_to_check = voxel_to_check + (voxel_per_row_and_colum_);
  if (new_voxel_to_check < voxel_list_.size()) {
    if (voxel_list_[new_voxel_to_check].type_ != VoxelType::air) {
      voxel_list_[new_voxel_to_check].faces_[LEFTFACES].active_ = false;
      voxel_list_[voxel_to_check].faces_[RIGHTFACES].active_ = false;

      voxel_list_[new_voxel_to_check].faces_[LEFTFACES].colour_picking_active_ = false;
      voxel_list_[voxel_to_check].faces_[RIGHTFACES].colour_picking_active_ = false;
      active_faces_ -= 2;
    }
  }
  //RIGHT FACE OF THE LEFT VOXEL OF THE VOXEL THAT IS BEING PLACED
  new_voxel_to_check = voxel_to_check - (voxel_per_row_and_colum_);
  if (new_voxel_to_check >= 0) {
    if (voxel_list_[new_voxel_to_check].type_ != VoxelType::air) {
      voxel_list_[new_voxel_to_check].faces_[RIGHTFACES].active_ = false;
      voxel_list_[voxel_to_check].faces_[LEFTFACES].active_ = false;

      voxel_list_[new_voxel_to_check].faces_[RIGHTFACES].colour_picking_active_ = false;
      voxel_list_[voxel_to_check].faces_[LEFTFACES].colour_picking_active_ = false;
      active_faces_ -= 2;
    }
  }
}


std::vector<FEMaterialComponent::Vertex> FEWorld::initFrontFace() {

  std::vector<FEMaterialComponent::Vertex> vertices;
  float normal[3]{ 0.0f,0.0f,0.0f };

  vertices.push_back({ {-0.5f,-0.5f,0.5f},
                       {normal[0],normal[1],normal[2]},
                       {0.0f,0.0f,0.0f,0.0f},
                       {0.0f,0.0f} }); // bottom left

  vertices.push_back({ {0.5f,-0.5f,0.5f},
                      {normal[0],normal[1],normal[2]},
                       {0.0f,0.0f,0.0f,0.0f},
                       {1.0f,0.0f} }); // bottom right

  vertices.push_back({ {-0.5f,0.5f,0.5f},
                       {normal[0],normal[1],normal[2]},
                       {0.0f,0.0f,0.0f,0.0f},
                       {0.0f,1.0f} }); // top left

  vertices.push_back({ {0.5f,0.5f,0.5f},
                       {normal[0],normal[1],normal[2]},
                       {0.0f,0.0f,0.0f,0.0f},
                       {1.0f,1.0f} }); // top right

  return vertices;
}

std::vector<FEMaterialComponent::Vertex> FEWorld::initRightFace() {
  std::vector<FEMaterialComponent::Vertex> vertices;
  float normal[3]{ 1.0f,0.0f,0.0f };

  vertices.push_back({ {0.5f,-0.5f,0.5f},
                       {normal[0],normal[1],normal[2]},
                       {0.0f,0.0f,0.0f,0.0f},
                       {0.0f,0.0f} }); // bottom left

  vertices.push_back({ {0.5f,-0.5f,-0.5f},
                       {normal[0],normal[1],normal[2]},
                       {0.0f,0.0f,0.0f,0.0f},
                       {1.0f,0.0f} }); // bottom right

  vertices.push_back({ {0.5f,0.5f,0.5f},
                       {normal[0],normal[1],normal[2]},
                       {0.0f,0.0f,0.0f,0.0f},
                       {0.0f,1.0f} }); // top left

  vertices.push_back({ {0.5f,0.5f,-0.5f},
                       {normal[0],normal[1],normal[2]},
                       {0.0f,0.0f,0.0f,0.0f},
                       {1.0f,1.0f} }); // top right

  return vertices;
}

std::vector<FEMaterialComponent::Vertex> FEWorld::initBackFace() {
  std::vector<FEMaterialComponent::Vertex> vertices;
  float normal[3]{ 0.0f,1.0f,0.0f };

  vertices.push_back({ {0.5f,-0.5f,-0.5f},
                       {normal[0],normal[1],normal[2]},
                       {0.0f,0.0f,0.0f,0.0f},
                       {0.0f,0.0f} }); // bottom left

  vertices.push_back({ {-0.5f,-0.5f,-0.5f},
                       {normal[0],normal[1],normal[2]},
                       {0.0f,0.0f,0.0f,0.0f},
                       {1.0f,0.0f} }); // bottom right

  vertices.push_back({ {0.5f,0.5f,-0.5f},
                       {normal[0],normal[1],normal[2]},
                       {0.0f,0.0f,0.0f,0.0f},
                       {0.0f,1.0f} }); // top left

  vertices.push_back({ {-0.5f,0.5f,-0.5f},
                       {normal[0],normal[1],normal[2]},
                       {0.0f,0.0f,0.0f,0.0f},
                       {1.0f,1.0f} }); // top right

  return vertices;
}

std::vector<FEMaterialComponent::Vertex> FEWorld::initLeftFace() {
  std::vector<FEMaterialComponent::Vertex> vertices;
  float normal[3]{ 0.0f,0.0f,1.0f };

  vertices.push_back({ {-0.5f,-0.5f,-0.5f},
                       {normal[0],normal[1],normal[2]},
                       {0.0f,0.0f,0.0f,0.0f},
                       {0.0f,0.0f} }); // bottom left

  vertices.push_back({ {-0.5f,-0.5f,0.5f},
                       {normal[0],normal[1],normal[2]},
                       {0.0f,0.0f,0.0f,0.0f},
                       {1.0f,0.0f} }); // bottom right

  vertices.push_back({ {-0.5f,0.5f,-0.5f},
                       {normal[0],normal[1],normal[2]},
                       {0.0f,0.0f,0.0f,0.0f},
                       {0.0f,1.0f} }); // top left

  vertices.push_back({ {-0.5f,0.5f,0.5f},
                       {normal[0],normal[1],normal[2]},
                       {0.0f,0.0f,0.0f,0.0f},
                       {1.0f,1.0f} }); // top right

  return vertices;
}

std::vector <FEMaterialComponent::Vertex> FEWorld::initTopFace() {
  std::vector<FEMaterialComponent::Vertex> vertices;
  float normal[3]{ 0.5f,0.5f,0.5f };

  vertices.push_back({ {-0.5f,0.5f,0.5f},
                       {normal[0],normal[1],normal[2]},
                       {0.0f,0.0f,0.0f,0.0f},
                       {0.0f,0.0f} }); // bottom left

  vertices.push_back({ {0.5f,0.5f,0.5f},
                       {normal[0],normal[1],normal[2]},
                       {0.0f,0.0f,0.0f,0.0f},
                       {1.0f,0.0f} }); // bottom right

  vertices.push_back({ {-0.5f,0.5f,-0.5f},
                       {normal[0],normal[1],normal[2]},
                       {0.0f,0.0f,0.0f,0.0f},
                       {0.0f,1.0f} }); // top left

  vertices.push_back({ {0.5f,0.5f,-0.5f},
                       {normal[0],normal[1],normal[2]},
                       {0.0f,0.0f,0.0f,0.0f},
                       {1.0f,1.0f} }); // top right

  return vertices;
}

std::vector<FEMaterialComponent::Vertex> FEWorld::initBottomFace() {
  std::vector<FEMaterialComponent::Vertex> vertices;
  float normal[3]{ 1.0f,1.0f,0.0f };

  vertices.push_back({ {0.5f,-0.5f,0.5f},
                       {normal[0],normal[1],normal[2]},
                       {0.0f,0.0f,0.0f,0.0f},
                       {0.0f,0.0f} }); // bottom left

  vertices.push_back({ {-0.5f,-0.5f,0.5f},
                       {normal[0],normal[1],normal[2]},
                       {0.0f,0.0f,0.0f,0.0f},
                       {1.0f,0.0f} }); // bottom right

  vertices.push_back({ {0.5f,-0.5f,-0.5f},
                       {normal[0],normal[1],normal[2]},
                       {0.0f,0.0f,0.0f,0.0f},
                       {0.0f,1.0f} }); // top left

  vertices.push_back({ {-0.5f,-0.5f,-0.5f},
                       {normal[0],normal[1],normal[2]},
                       {0.0f,0.0f,0.0f,0.0f},
                       {1.0f,1.0f} }); // top right

  return vertices;
}




