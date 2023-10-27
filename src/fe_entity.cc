#include "..\include\fe_entity.h"

FEEntity::FEEntity(FETransformComponent& transform, FEMaterialComponent& material) :
transform_(transform), material_(material) {
  active_ = true;
}

FEEntity::~FEEntity() {
}

void FEEntity::Draw(glm::mat4 projection, glm::mat4 view) {
  if (active_) {
    material_.enable();

    material_.setUpModel(transform_.getTransform());

    material_.setUpCamera(projection, view);
    material_.bindAndRender();
  }
}
