
// Author : jauregiaz

#include "fe_material.h"
#include "fe_shader.h"
#include <glm/gtc/type_ptr.hpp>
#include <memory>
#include <fstream>

#include "fe_program.h"



FEMaterialComponent::FEMaterialComponent(std::shared_ptr<FEProgram> program,
  const std::vector<Vertex>& vertices, const std::vector<int> indices) {

  program_ = program;

  vertices_ = vertices;
  indices_ = indices;

  glUseProgram(program_->getId());
  assert(glGetError() == GL_NO_ERROR);
  //Create VAO, VBO and the index buffer
  glGenVertexArrays(1, &VAO_);
  assert(glGetError() == GL_NO_ERROR);
  glBindVertexArray(VAO_);
  assert(glGetError() == GL_NO_ERROR);

  glGenBuffers(1, &VBO_);
  assert(glGetError() == GL_NO_ERROR);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_);
  assert(glGetError() == GL_NO_ERROR);

  glGenBuffers(1, &vertex_index_);
  assert(glGetError() == GL_NO_ERROR);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertex_index_);
  assert(glGetError() == GL_NO_ERROR);

  //Load vertices
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices_.size(), &vertices_[0], GL_STATIC_DRAW);
  assert(glGetError() == GL_NO_ERROR);

  //Load indices
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * (indices_.size() + 1), &indices_[0], GL_STATIC_DRAW);
  assert(glGetError() == GL_NO_ERROR);

  glEnableVertexAttribArray(0);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);

  glEnableVertexAttribArray(1);

  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(float) * 3));


  glEnableVertexAttribArray(2);

  glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(float) * 6));


  glEnableVertexAttribArray(3);

  glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(float) * 10));
}


FEMaterialComponent::~FEMaterialComponent() {

}


void FEMaterialComponent::enable() {
  glUseProgram(program_->getId());
  assert(glGetError() == GL_NO_ERROR);
}

void FEMaterialComponent::enableWithOtherProgram(int program_id) {
  glUseProgram(program_id);
  assert(glGetError() == GL_NO_ERROR);
}

void FEMaterialComponent::bindAndRender() {

  glBindVertexArray(VAO_);
  assert(glGetError() == GL_NO_ERROR);
  glDrawElements(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_INT, 0);
  assert(glGetError() == GL_NO_ERROR);

}

void FEMaterialComponent::setUpModel(const glm::mat4x4& transform) {
  GLint id = 0;
  id = glGetUniformLocation(program_->getId(), "model_matrix");
  assert(glGetError() == GL_NO_ERROR);

  glUniformMatrix4fv(id, 1, GL_FALSE, glm::value_ptr(transform));
  assert(glGetError() == GL_NO_ERROR);

}


void FEMaterialComponent::setUpCamera(const glm::mat4x4& projection, const glm::mat4x4& view) {
  GLint id = 0;

  id = glGetUniformLocation(program_->getId(), "projection_matrix");
  assert(glGetError() == GL_NO_ERROR);
  glm::mat4x4 aux = projection * view;

  glUniformMatrix4fv(id, 1, GL_FALSE, glm::value_ptr(aux));
  assert(glGetError() == GL_NO_ERROR);
}

void FEMaterialComponent::setUpModelWithOtherProgram(const glm::mat4x4& transform, int program_id){
  GLint id = 0;
  id = glGetUniformLocation(program_id, "model_matrix");
  assert(glGetError() == GL_NO_ERROR);

  glUniformMatrix4fv(id, 1, GL_FALSE, glm::value_ptr(transform));
  assert(glGetError() == GL_NO_ERROR);
}

void FEMaterialComponent::setUpCameraWithOtherProgram(const glm::mat4x4& projection, const glm::mat4x4& view, int program_id) {
  GLint id = 0;

  id = glGetUniformLocation(program_id, "projection_matrix");
  assert(glGetError() == GL_NO_ERROR);
  glm::mat4x4 aux = projection * view;

  glUniformMatrix4fv(id, 1, GL_FALSE, glm::value_ptr(aux));
  assert(glGetError() == GL_NO_ERROR);
}

GLint FEMaterialComponent::getUniformLocation(std::string name) {
  GLint id = -1;
  id = glGetUniformLocation(program_->getId(), name.c_str());


  return id;
}


void FEMaterialComponent::setUpReferenceUniform(std::string name, const glm::vec3 value) {
  glUniform3f(getUniformLocation(name), value.x, value.y, value.z);
}

void FEMaterialComponent::setUpReferenceUniformWithOtherProgram(std::string name, const glm::vec3 value, int program_id) {
  GLint id = -1;
  id = glGetUniformLocation(program_id, name.c_str());
  glUniform3f(id, value.x, value.y, value.z);
}













