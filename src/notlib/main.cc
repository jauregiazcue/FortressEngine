#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "fe_window.h"

#include "fe_transform.h"
#include "fe_material.h"
#include "fe_program.h"

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

  FEMaterialComponent mc{ pro,vertices,index };
  FETransformComponent tc{ { 0.0f,0.0f,0.0f },
                           { 0.0f,0.0f,0.0f }, 
                           { 1.0f,1.0f,1.0f } };

  glm::mat4 projection_ = glm::perspective(glm::radians(45.0f), 640.0f / 480.0f, 0.1f, 100.0f);


  FETransformComponent cameraTransform_;
  cameraTransform_.setPosition({ 0.0f,0.0f,5.0f });
  glm::mat4 view_ = glm::inverse(cameraTransform_.getTransform());



  while (!fe_window.isDone()) {
    fe_window.clear();



    mc.enable();

    mc.setUpModel(tc.getTransform());

    mc.setUpCamera(projection_
                  , view_ 
                  );
    mc.bindAndRender();



    

    fe_window.swap();
  }

  return 0;
}
