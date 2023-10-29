#include "fe_world.h"

FEWorld::FEWorld(int voxelPerRow) {
  voxelPerRow_ = voxelPerRow;

   std::vector<FEMaterialComponent::Vertex> vertices = initFrontFace();

  std::vector<int> index = { 0, 1, 2 ,
                            3,2,1 };

  std::shared_ptr<FEProgram> pro = std::make_shared<FEProgram>(std::vector<FEShader>{
    FEShader("../src/shaders/test.vert", ShaderType::Vertex),
      FEShader("../src/shaders/test.frag", ShaderType::Fragment) });

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

  createChunks();
  Culling();
}

FEWorld::~FEWorld() {

}

void FEWorld::createChunks() {
  
  for (int y = 0; y < voxelPerRow_; y++) {
    for (int z = 0; z < voxelPerRow_; z++) {
      for (int x = 0; x < voxelPerRow_; x++) {
        transform_list_.push_back(FETransformComponent{
                              { (float)x * 1.5f,(float)y * -1.5f,(float)z * -1.5f },
                              { 0.0f,0.0f,0.0f },
                              { 1.0f,1.0f,1.0f } });

        
      }
    }
  }

  int voxel_id_ = 0;


  for (int y = 0; y < voxelPerRow_; y++) {
    for (int z = 0; z < voxelPerRow_; z++) {
      for (int x = 0; x < voxelPerRow_; x++) {

        voxel_list_.push_back({ voxel_id_,FEWorld::VoxelType::block,
          {{ 0,true },
           { 1,true },
           { 2,true },
           { 3,true },
           { 4,true },
           { 5,true }} });
        
        //One voxel = 6 faces = 12 triangles
        active_triangles_ += 12;
        voxel_id_ += 1;
      }
    }
  }

  
}

void FEWorld::DrawVoxel(int voxel_id_, glm::mat4 projection, glm::mat4 view) {
  DrawFace(voxel_id_, 0, projection, view);
  DrawFace(voxel_id_, 1, projection, view);
  DrawFace(voxel_id_, 2, projection, view);
  DrawFace(voxel_id_, 3, projection, view);
  DrawFace(voxel_id_, 4, projection, view);
  DrawFace(voxel_id_, 5, projection, view);
}


void FEWorld::DrawFace(int voxel_id_,int face_id_, glm::mat4 projection, glm::mat4 view) {
  if (voxel_list_[voxel_id_].faces_[face_id_].active_) {
    FEMaterialComponent& material = 
      material_list_[voxel_list_[voxel_id_].faces_[face_id_].material_id_];

    material.enable();

    material.setUpModel(transform_list_[voxel_id_].getTransform());

    material.setUpCamera(projection, view);
    material.bindAndRender();
  }
}

void FEWorld::Culling() {
  for (int i = 0; i < voxel_list_.size(); i++) {
    CheckFaces(i);
  }
}

void FEWorld::CheckFaces(int voxel_to_check) {

  //LEFT FACE CHECKING
  if (voxel_to_check - 1 >= 0 && (voxel_to_check % voxelPerRow_ ) != 0) {
    voxel_list_[voxel_to_check].faces_[1].active_ = false;
    active_triangles_ -= 2;
  }

  //RIGHTA FACE CHECKING
  if (voxel_to_check + 1 < voxel_list_.size() 
    && ((voxel_to_check + 1) % voxelPerRow_ ) != 0) {
    voxel_list_[voxel_to_check].faces_[3].active_ = false;
    active_triangles_ -= 2;
  }

  int voxel_to_check_2 = voxel_to_check % (voxelPerRow_ * voxelPerRow_);
  //FRONT FACE CHECKING
  if (voxel_to_check_2  >= 0  && voxel_to_check_2 >= voxelPerRow_) {
    voxel_list_[voxel_to_check].faces_[0].active_ = false;
    active_triangles_ -= 2;
  }

  int last_row = ((voxelPerRow_ * voxelPerRow_) - voxelPerRow_);
  //BACK FACE CHECKING
  if (voxel_to_check_2 < voxel_list_.size() 
    && voxel_to_check_2 < last_row) {
    voxel_list_[voxel_to_check].faces_[2].active_ = false;
    active_triangles_ -= 2;
  }

  if (voxel_to_check >= (voxelPerRow_ * voxelPerRow_)) {
    voxel_list_[voxel_to_check].faces_[4].active_ = false;
    active_triangles_ -= 2;
  }

  if (voxel_to_check < ((voxelPerRow_ * voxelPerRow_ * voxelPerRow_) - (voxelPerRow_ * voxelPerRow_))) {
    voxel_list_[voxel_to_check].faces_[5].active_ = false;
    active_triangles_ -= 2;
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


