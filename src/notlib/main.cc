#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "fe_window.h"

#include "fe_transform.h"
#include "fe_material.h"
#include "fe_program.h"
#include "fe_entity.h"
#include "fe_render.h"

#include <glm/gtc/type_ptr.hpp>



int main(void) {

  FEWindow fe_window{ {1.0f,1.0f,1.0f,1.0f} };
  if (fe_window.init(1280, 700) != 0) {
    return -1;
  }

  std::vector<FEMaterialComponent::Vertex> vertices;
  glm::vec3 position{ -0.5f,-0.5,1.0f };
  
  vertices.push_back({ {-0.5f,-0.5f,0.0f}, {0.0f,0.0f,1.0f}, {0.0f,0.0f,0.0f,0.0f}, {0.0f,0.0f} }); // bottom left
  vertices.push_back({ {0.5f,-0.5f,0.0f}, {0.0f,1.0f,0.0f},{0.0f,0.0f,0.0f,0.0f}, {1.0f,0.0f} }); // bottom right
  vertices.push_back({ {-0.5f,0.5f,0.0f}, {1.0f,0.0f,0.0f},{0.0f,0.0f,0.0f,0.0f}, {0.0f,1.0f} }); // top left
  vertices.push_back({ {0.5f,0.5f,0.0f}, {1.0f,1.0f,1.0f}, {0.0f,0.0f,0.0f,0.0f}, {1.0f,1.0f} }); // top right

  std::vector<int> index = { 0, 1, 2 ,
                            3,2,1 };

  std::shared_ptr<FEProgram> pro = std::make_shared<FEProgram>(std::vector<FEShader>{
    FEShader("../src/shaders/test.vert", ShaderType::Vertex),
    FEShader("../src/shaders/test.frag", ShaderType::Fragment) });
  FEEntity entity{ FETransformComponent{ { 0.0f,0.0f,0.0f },
                                         { 0.0f,0.0f,0.0f },
                                         { 1.0f,1.0f,1.0f } },
                   FEMaterialComponent{ pro,vertices,index }};




  std::vector<FEEntity> entity_list;

  entity_list.push_back(entity);

  FERender renderer;
  renderer.SetCameraPosition({ 0.0f,0.0f,5.0f });

  while (!fe_window.isDone()) {
    fe_window.clear();

    renderer.DebugCameraMovement();
    renderer.Render(entity_list);
    //entity.Draw(projection_,view_);



    

    fe_window.swap();
  }

  return 0;
}
