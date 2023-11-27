#include "fe_world.h"
#include "fe_constants.h"
#include "fe_render.h"
#include <GLFW/glfw3.h>


FEWorld::FEWorld() {
  std::vector<int> index = { 0, 1, 2 ,
                            3,2,1 };

  std::shared_ptr<FEProgram> pro = std::make_shared<FEProgram>(std::vector<FEShader>{
    FEShader("../src/shaders/common.vert", ShaderType::Vertex),
      FEShader("../src/shaders/common.frag", ShaderType::Fragment) });

  material_list_ = new FEMaterialComponent[FACES]
  { { pro,initFrontFace(),index } , { pro,initLeftFace(),index } , { pro,initBackFace(),index } ,
    { pro,initRightFace(),index } , { pro,initTopFace(),index } , { pro,initBottomFace(),index } };


  active_faces_ = 0;
  ms_for_chunk_creation_ = 0.0f;
  voxel_per_row_ = 0;
  offset_ = 1.0f;

  culling_ = true;
  greedy_meshing_ = false;
  collision_detection_ = true;
}

FEWorld::~FEWorld() {
  if( voxel_in_total_ > 0 ) {
    delete[] voxel_list_;
    voxel_list_ = nullptr;
  }
  

  delete[] material_list_;
  material_list_ = nullptr;
}

void FEWorld::init(int voxelPerRow) {
  voxel_per_row_ = voxelPerRow;
  voxel_per_row_and_colum_ = voxel_per_row_ * voxel_per_row_;
  voxel_in_total_ = voxel_per_row_ * voxel_per_row_ * voxel_per_row_;
  voxel_list_ = new Voxel[voxel_in_total_];

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
  
  GenerateOctreeNodes();
}

void FEWorld::createChunks() {

  float x = 0.0f;
  float y = 0.0f;
  float z = 0.0f;
  for (int voxel_id = 0; voxel_id < voxel_in_total_; voxel_id++) {
    glm::vec3 position{ x * offset_,-y * offset_,-z * offset_ };
    Faces* faces = GetFaces(voxel_id, position);
    Voxel test({ voxel_id,FEWorld::VoxelType::block,
          {faces[FRONTFACE],faces[LEFTFACES],faces[BACKFACES],
           faces[RIGHTFACES],faces[TOPFACES],faces[BOTTOMFACES]}, 
          FETransformComponent{position,
                               { 0.0f,0.0f,0.0f },
                               { 1.0f,1.0f,1.0f } } });
    voxel_list_[voxel_id] = test;

    active_faces_ += FACES;
    z += 1.0f;

    if (z  == (float)voxel_per_row_) {
      z = 0.0f;
      y += 1.0f;
    }

    if (y == (float)voxel_per_row_) {
      y = 0.0f;
      x += 1.0f;
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

  }

  return faces_;
}

void FEWorld::DrawVoxel(int voxel_id, glm::mat4 projection, glm::mat4 view) {
  if (voxel_list_[voxel_id].type_ != VoxelType::air) {
    DrawFace(voxel_id, FRONTFACE, projection, view);
    DrawFace(voxel_id, LEFTFACES, projection, view);
    DrawFace(voxel_id, BACKFACES, projection, view);
    DrawFace(voxel_id, RIGHTFACES, projection, view);
    DrawFace(voxel_id, TOPFACES, projection, view);
    DrawFace(voxel_id, BOTTOMFACES, projection, view);
  }
  
}


void FEWorld::DrawFace(int voxel_id,int face_id, glm::mat4 projection, glm::mat4 view) {
  Faces& drawing_face = voxel_list_[voxel_id].faces_[face_id];
  if ( drawing_face.active_) {
    FEMaterialComponent& material = 
      material_list_[drawing_face.material_id_];

    material.enable();

    material.setUpReferenceUniform("PickingColour",
      drawing_face.color_id_);

    material.setUpModel( voxel_list_[voxel_id].transform_.getTransform());

    material.setUpCamera(projection, view);

    material.bindAndRender();
  }
}

void FEWorld::DrawVoxelForColourPicking(int voxel_id, glm::mat4 projection, glm::mat4 view, int program_id) {
  if (voxel_list_[voxel_id].type_ != VoxelType::air) {
    DrawFaceForColourPicking(voxel_id, FRONTFACE, projection, view, program_id);
    DrawFaceForColourPicking(voxel_id, LEFTFACES, projection, view, program_id);
    DrawFaceForColourPicking(voxel_id, BACKFACES, projection, view, program_id);
    DrawFaceForColourPicking(voxel_id, RIGHTFACES, projection, view, program_id);
    DrawFaceForColourPicking(voxel_id, TOPFACES, projection, view, program_id);
    DrawFaceForColourPicking(voxel_id, BOTTOMFACES,projection, view, program_id);
  }
  
}

void FEWorld::DrawFaceForColourPicking(int voxel_id, int face_id, glm::mat4 projection, glm::mat4 view, int program_id) {
  Faces& drawing_face = voxel_list_[voxel_id].faces_[face_id];
  if ( drawing_face.colour_picking_active_ ) {
    FEMaterialComponent& material =
      material_list_[drawing_face.material_id_];

    material.enableWithOtherProgram(program_id);

    material.setUpReferenceUniformWithOtherProgram("PickingColour",
      drawing_face.color_id_, program_id);

    material.setUpModelWithOtherProgram(
      voxel_list_[voxel_id].transform_.getTransform(), program_id);

    material.setUpCameraWithOtherProgram(projection, view, program_id);

    material.bindAndRender();
  }
}


void FEWorld::Culling() {
  for (int i = 0; i < voxel_in_total_; i++) {
    CheckFaces(i);
  }
}

void FEWorld::GreedyMeshing() {
  /*if (voxel_in_total_ > 1) {
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

  }*/
  
}

void FEWorld::GenerateOctreeNodes() {
  SetNodesCenter();
  SetNodesVoxels();
  
   
  for (int i = 0; i < NODES; i++) {
    printf("\nNode %d :  \n", i);
    for (int x = 0; x < nodes_[i].voxels_.size(); x++) {
      printf("%d, ", nodes_[i].voxels_[x]);
    }
  }
}

void FEWorld::SetNodesCenter() {
  glm::vec3 first_point = voxel_list_[0].transform_.getPosition();
  glm::vec3 last_point = voxel_list_[voxel_in_total_ - 1].transform_.getPosition();
  glm::vec3 center_point{
    (first_point.x + last_point.x) * 0.5f,
    (first_point.y + last_point.y) * 0.5f,
    (first_point.z + last_point.z) * 0.5f };

  //Top Front Left
  SetNodeCenter(0, 0, center_point);
  //Top Back Left
  SetNodeCenter(1, voxel_per_row_ - 1, center_point);
  //Bottom Front Left
  SetNodeCenter(2, voxel_per_row_and_colum_ - voxel_per_row_, center_point);
  //Bottom Back Left
  SetNodeCenter(3, voxel_per_row_and_colum_ - 1, center_point);

  //Top Front Right
  SetNodeCenter(4, voxel_per_row_and_colum_ * voxel_per_row_and_colum_, center_point);
  //Top Back Right
  SetNodeCenter(5, (voxel_per_row_and_colum_ * voxel_per_row_and_colum_) + voxel_per_row_ - 1, center_point);
  //Bottom Front Right
  SetNodeCenter(6, voxel_in_total_ - voxel_per_row_, center_point);
  //Bottom Back Right
  SetNodeCenter(7, voxel_in_total_ - 1, center_point);
}

void FEWorld::SetNodesVoxels()
{
}

void FEWorld::SetNodeCenter(int node_to_set, int corner_voxel, glm::vec3 second_point) {
  glm::vec3 first_point = voxel_list_[corner_voxel].transform_.getPosition();
  nodes_[node_to_set].center_ = {
   (first_point.x + second_point.x) * 0.5f,
   (first_point.y + second_point.y) * 0.5f,
   (first_point.z + second_point.z) * 0.5f };
}


void FEWorld::CheckFaces(int voxel_to_check) {
  int active_faces_now = active_faces_;

  if (voxel_to_check - 1 >= 0 && (voxel_to_check % voxel_per_row_) != 0) {
    voxel_list_[voxel_to_check].faces_[FRONTFACE].active_ = false;
    voxel_list_[voxel_to_check].faces_[FRONTFACE].colour_picking_active_ = false;
    active_faces_ -= 1;

  }

  if (voxel_to_check + 1 < voxel_in_total_
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
  if (voxel_to_check_2 < voxel_in_total_
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
    for (int i = 0; i < voxel_in_total_; i++) {
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

void FEWorld::CollisionDetection( FERender& render, bool destroy ) {
  if( voxel_in_total_ > 0 ) {
    //Get the forward of the camera
    glm::mat4 cT = render.camera_transform_.getTransform();
    glm::vec3 forward{ cT[2].x, cT[2].y, cT[2].z };
    forward = glm::normalize(forward);
    forward *= -100.0f;

    //Get the start and end position of the line
    glm::vec3 start{ render.camera_transform_.getPosition() };
    glm::vec3 end = start + forward + glm::vec3{1.0f,1.0f,1.0f};

    for (int i = 0; i < voxel_in_total_; i++) {
      for (int x = 0; x < FACES; x++) {
        if (CheckIntersection(start, end, i,x) && voxel_list_[i].type_ != VoxelType::air) {
          if (destroy) {
            DestroyVoxel(i);
            if (culling_) UpdateAdjacentFacesWhenDestroy(i);
            return;
          } else {
            PlaceVoxel(i, x);
            return;
          }
          
        }
      }
      
    }
  }
}

bool FEWorld::CollisionCheck( glm::vec3 voxel, glm::vec3 mouse ) {
  //AABBAABB collision test created using :
  //http://www.r-5.org/files/books/computers/algo-list/realtime-3d/Christer_Ericson-Real-Time_Collision_Detection-EN.pdf
  //as reference (page 117 - 118)

  float range = 0.5f;
  glm::vec3 voxelMax = {voxel.x + range,voxel.y + range,voxel.z + range};
  glm::vec3 voxelMin = {voxel.x - range,voxel.y - range,voxel.z - range};
  range = 0.1f;
  glm::vec3 mouseMax = {mouse.x + range,mouse.y + range,mouse.z + range};
  glm::vec3 mouseMin = {mouse.x - range,mouse.y - range,mouse.z - range};
  // Exit with no intersection if separated along an axis
  if ( voxelMax.x < mouseMin.x || voxelMin.x > mouseMax.x ) return false;
  if ( voxelMax.y < mouseMin.y || voxelMin.y > mouseMax.y ) return false;
  if ( voxelMax.z < mouseMin.z || voxelMin.z > mouseMax.z ) return false;
  // Overlapping on all axes means AABBs are intersecting
  return true;
}

glm::vec3 FEWorld::Raycast( FEWindow& window, FERender& render, float distance_helper ) {
  //Get the position of the mouse
  double xpos, ypos;
  glfwGetCursorPos( window.window_.get(), &xpos, &ypos );

  //Get the height of the window
  int height,width;
  glfwGetWindowSize( window.window_.get(), &width, &height );

  //Get the Raycast Clip Coordinates
  glm::vec4 ray_clip_coords{
    (2.0f * xpos) / width - 1.0f, // Normalize Coords of the X of the raycast
    1.0f - (2.0f * ypos) / height,// Normalize Coords of the Y of the raycast
    -1.0f,1.0f};


  
  //Get the Camera Coordinate for the raycast
  glm::vec4 ray_camera_coords{glm::inverse( render.projection_ ) * ray_clip_coords};

  //Change the z due to -1.0f being forward in OpenGl (we want the ray to go forward)
  ray_camera_coords = {ray_camera_coords.x,ray_camera_coords.y,-1.0f,0.0f};

  //Get the World Coordinate for the Raycast
  glm::vec4 ray_world_coords{glm::inverse(render.view_) * ray_camera_coords};
  ray_world_coords = glm::normalize(ray_world_coords);

  
  glm::vec3 ray{
    ray_world_coords.x,
    ray_world_coords.y,
    ray_world_coords.z};
  ray += render.camera_transform_.getPosition();

  return ray;
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
  if (new_voxel_to_check < voxel_in_total_) {
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
  if (new_voxel_to_check < voxel_in_total_) {
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
  if (new_voxel_to_check < voxel_in_total_) {
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

  if (new_voxel_id >= 0 && new_voxel_id < voxel_in_total_) {
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
  if (new_voxel_to_check < voxel_in_total_) {
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
  if (new_voxel_to_check < voxel_in_total_ && new_voxel_to_check % (voxel_per_row_and_colum_) >= voxel_per_row_) {
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
  if (new_voxel_to_check < voxel_in_total_) {
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

bool FEWorld::CheckIntersection(glm::vec3 ray_start, glm::vec3 ray_end, int voxel_id,int face_id) {
  bool intersection;
  if (voxel_list_[voxel_id].faces_[face_id].active_) {
    intersection = IntersectSegmentPlane(ray_start, ray_end, voxel_id, face_id);
    
    if (intersection) {
      intersection = CollisionCheck(voxel_list_[voxel_id].transform_.getPosition(), point_to_check);
      if (intersection) return true;
    }
  }
  return false;
}

bool FEWorld::IntersectSegmentPlane(glm::vec3 ray_start, glm::vec3 ray_end, int voxel_id, int face_type) {
  glm::vec3 a, b, c;
  switch (face_type) {
  case FRONTFACE :
    a = { -0.5f,-0.5f,0.5f };
    b = { 0.5f,-0.5f,0.5f };
    c = { -0.5f,0.5f,0.5f };
    break;
  case LEFTFACES:
    a = { -0.5f,-0.5f,-0.5f };
    b = { -0.5f,-0.5f,0.5f };
    c = { -0.5f,0.5f,-0.5f };
    break;
  case BACKFACES:
    a = { 0.5f,-0.5f,-0.5f };
    b = { -0.5f,-0.5f,-0.5f };
    c = { 0.5f,0.5f,-0.5f };
    break;
  case RIGHTFACES:
    a = { 0.5f,-0.5f,0.5f };
    b = { 0.5f,-0.5f,-0.5f };
    c = { 0.5f,0.5f,0.5f };
    break;
  case TOPFACES:
    a = { -0.5f,0.5f,0.5f };
    b = { 0.5f,0.5f,0.5f };
    c = { -0.5f,0.5f,-0.5f };
    break;
  case BOTTOMFACES:
    a = { 0.5f,-0.5f,0.5f };
    b = { -0.5f,-0.5f,0.5f };
    c = { 0.5f,-0.5f,-0.5f };
    break;
  }

  a += voxel_list_[voxel_id].transform_.getPosition();
  b += voxel_list_[voxel_id].transform_.getPosition();
  c += voxel_list_[voxel_id].transform_.getPosition();

  //Get the normal of the Face
  glm::vec3 normal = glm::normalize(glm::cross(b - a, c - a));

  //Get the dot of the normal and a point in the plane
  float dotOfPlane = glm::dot(normal, a);

  glm::vec3 ray = ray_end - ray_start;

  float t = (dotOfPlane - glm::dot(normal, ray_start)) / glm::dot(normal, ray);
  if (t >= 0.0f && t <= 1.0f) {
    point_to_check = ray_start + t * ray;
    return true;
  }

  return false;
}




