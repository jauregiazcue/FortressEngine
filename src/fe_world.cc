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

  active_triangles_ = 0;



  
}

FEWorld::~FEWorld() {

}

void FEWorld::init(int voxelPerRow, bool culling) {
  voxel_per_row_ = voxelPerRow;

  std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

  createChunks();

  std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

  ms_for_chunk_creation_ =
    std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();

  if (culling) {
    Culling();
  }

}

void FEWorld::createChunks() {
  
  for (int y = 0; y < voxel_per_row_; y++) {
    for (int z = 0; z < voxel_per_row_; z++) {
      for (int x = 0; x < voxel_per_row_; x++) {
        transform_list_.push_back(FETransformComponent{
                              { (float)x ,(float)-y,(float)-z },
                              { 0.0f,0.0f,0.0f },
                              { 1.0f,1.0f,1.0f } });

        
      }
    }
  }

  int voxel_id_ = 0;


  for (int y = 0; y < voxel_per_row_; y++) {
    for (int z = 0; z < voxel_per_row_; z++) {
      for (int x = 0; x < voxel_per_row_; x++) {
        Faces* faces = GetFaces(voxel_id_);
        voxel_list_.push_back({ voxel_id_,FEWorld::VoxelType::block,
          {faces[0],faces[1],faces[2],faces[3],faces[4],faces[5]} });
        
        //One voxel = 6 faces = 12 triangles
        active_triangles_ += 12;
        voxel_id_ += 1;
      }
    }
  }

  
}

FEWorld::Faces* FEWorld::GetFaces(int voxel_id) {
  Faces faces_[6];

  for (int i = 0; i < 6; i++) {
    faces_[i].material_id_ = i;
    faces_[i].active_ = true;

    int color_id_helper = (voxel_id * how_many_faces_) + i;
    faces_[i].real_color_id_ = color_id_helper;
    faces_[i].color_id_ =    { ((color_id_helper & 0x000000FF) >> 0)  / 255.0f ,
                               ((color_id_helper & 0x0000FF00) >> 8)  / 255.0f ,
                               ((color_id_helper & 0x00FF0000) >> 16) / 255.0f };


  }

  return faces_;
}

void FEWorld::DrawVoxel(int voxel_id_, glm::mat4 projection, glm::mat4 view) {
  if (voxel_list_[voxel_id_].type_ != VoxelType::air) {
    DrawFace(voxel_id_, 0, projection, view);
    DrawFace(voxel_id_, 1, projection, view);
    DrawFace(voxel_id_, 2, projection, view);
    DrawFace(voxel_id_, 3, projection, view);
    DrawFace(voxel_id_, 4, projection, view);
    DrawFace(voxel_id_, 5, projection, view);
  }
  
}


void FEWorld::DrawFace(int voxel_id_,int face_id_, glm::mat4 projection, glm::mat4 view) {
  if (voxel_list_[voxel_id_].faces_[face_id_].active_) {
    FEMaterialComponent& material = 
      material_list_[voxel_list_[voxel_id_].faces_[face_id_].material_id_];

    material.enable();

    material.setUpReferenceUniform("PickingColour",
      voxel_list_[voxel_id_].faces_[face_id_].color_id_);

    material.setUpModel(transform_list_[voxel_id_].getTransform());

    material.setUpCamera(projection, view);

    material.bindAndRender();
  }
}

void FEWorld::DrawVoxelForColourPicking(int voxel_id_, glm::mat4 projection, glm::mat4 view, int program_id) {
  if (voxel_list_[voxel_id_].type_ != VoxelType::air) {
    DrawFaceForColourPicking(voxel_id_, 0, projection, view, program_id);
    DrawFaceForColourPicking(voxel_id_, 1, projection, view, program_id);
    DrawFaceForColourPicking(voxel_id_, 2, projection, view, program_id);
    DrawFaceForColourPicking(voxel_id_, 3, projection, view, program_id);
    DrawFaceForColourPicking(voxel_id_, 4, projection, view, program_id);
    DrawFaceForColourPicking(voxel_id_, 5, projection, view, program_id);
  }
  
}

void FEWorld::DrawFaceForColourPicking(int voxel_id_, int face_id_, glm::mat4 projection, glm::mat4 view, int program_id) {
  if (voxel_list_[voxel_id_].faces_[face_id_].active_) {
    FEMaterialComponent& material =
      material_list_[voxel_list_[voxel_id_].faces_[face_id_].material_id_];

    material.enableWithOtherProgram(program_id);

    material.setUpReferenceUniformWithOtherProgram("PickingColour",
      voxel_list_[voxel_id_].faces_[face_id_].color_id_, program_id);

    material.setUpModelWithOtherProgram(transform_list_[voxel_id_].getTransform(), program_id);

    material.setUpCameraWithOtherProgram(projection, view, program_id);

    material.bindAndRender();
  }
}


void FEWorld::Culling() {
  for (int i = 0; i < voxel_list_.size(); i++) {
    CheckFaces(i);
  }
}

void FEWorld::CheckFaces(int voxel_to_check) {
  int deactive_faces = 0;
  //LEFT FACE CHECKING
  if (voxel_to_check - 1 >= 0 && (voxel_to_check % voxel_per_row_) != 0) {
    voxel_list_[voxel_to_check].faces_[1].active_ = false;
    active_triangles_ -= 2;
    deactive_faces += 1;
  }

  //RIGHT FACE CHECKING
  if (voxel_to_check + 1 < voxel_list_.size() 
    && ((voxel_to_check + 1) % voxel_per_row_) != 0) {
    voxel_list_[voxel_to_check].faces_[3].active_ = false;
    active_triangles_ -= 2;
    deactive_faces += 1;
  }

  int voxel_to_check_2 = voxel_to_check % (voxel_per_row_ * voxel_per_row_);
  //FRONT FACE CHECKING
  if (voxel_to_check_2  >= 0  && voxel_to_check_2 >= voxel_per_row_) {
    voxel_list_[voxel_to_check].faces_[0].active_ = false;
    active_triangles_ -= 2;
    deactive_faces += 1;
  }

  int last_row = ((voxel_per_row_ * voxel_per_row_) - voxel_per_row_);
  //BACK FACE CHECKING
  if (voxel_to_check_2 < voxel_list_.size() 
    && voxel_to_check_2 < last_row) {
    voxel_list_[voxel_to_check].faces_[2].active_ = false;
    active_triangles_ -= 2;
    deactive_faces += 1;
  }
  //TOP FACE CHECKING
  if (voxel_to_check >= (voxel_per_row_ * voxel_per_row_)) {
    voxel_list_[voxel_to_check].faces_[4].active_ = false;
    active_triangles_ -= 2;
    deactive_faces += 1;
  }
  //BOTTOM FACE CHECKING
  if (voxel_to_check < ((voxel_per_row_ * voxel_per_row_ * voxel_per_row_) - (voxel_per_row_ * voxel_per_row_))) {
    voxel_list_[voxel_to_check].faces_[5].active_ = false;
    active_triangles_ -= 2;
    deactive_faces += 1;
  }

  if (deactive_faces >= 6) {
    voxel_list_[voxel_to_check].type_ = VoxelType::air;
  }
}

void FEWorld::ColourPicking( int colour_id,bool destroy) {
  

  //If the id is found, the voxel will be destroy or place
  for (int i = 0; i < voxel_list_.size(); i++) {
    for (int x = 0; x < how_many_faces_; x++) {
      if (voxel_list_[i].type_ != VoxelType::air) {
        if (voxel_list_[i].faces_[x].real_color_id_ == colour_id) {
          if (destroy) {
            DestroyVoxel(i);
            return;
          }
          else {
            PlaceVoxel(i);
          }
        }
      }
    }
  }
}

void FEWorld::DestroyVoxel(int voxel_id) {

  if (voxel_list_[voxel_id].faces_[0].active_) {
    voxel_list_[voxel_id].faces_[0].active_ = false;
    active_triangles_ -= 2;
  }

  if (voxel_list_[voxel_id].faces_[1].active_) {
    voxel_list_[voxel_id].faces_[1].active_ = false;
    active_triangles_ -= 2;
  }

  if (voxel_list_[voxel_id].faces_[2].active_) {
    voxel_list_[voxel_id].faces_[2].active_ = false;
    active_triangles_ -= 2;
  }

  if (voxel_list_[voxel_id].faces_[3].active_) {
    voxel_list_[voxel_id].faces_[3].active_ = false;
    active_triangles_ -= 2;
  }

  if (voxel_list_[voxel_id].faces_[4].active_) {
    voxel_list_[voxel_id].faces_[4].active_ = false;
    active_triangles_ -= 2;
  }
  
  if (voxel_list_[voxel_id].faces_[5].active_) {
    voxel_list_[voxel_id].faces_[5].active_ = false;
    active_triangles_ -= 2;
  }
  
}

void FEWorld::PlaceVoxel(int voxel_id) {

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




