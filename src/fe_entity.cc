#include "..\include\fe_entity.h"

FEEntity::FEEntity(FETransformComponent transform, FEMaterialComponent material) :
transform_(transform), material_(material) {
}

FEEntity::~FEEntity() {
}

void FEEntity::Draw(glm::mat4 projection, glm::mat4 view) {
  material_.enable();

  material_.setUpModel(transform_.getTransform());

  material_.setUpCamera(projection, view);
  material_.bindAndRender();
}
