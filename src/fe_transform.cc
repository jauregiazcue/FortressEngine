
// Author : jauregiaz
#include "fe_transform.h"
#include <glm/gtc/matrix_transform.hpp>


FETransformComponent::FETransformComponent() {
  position_ = {0.0f,0.0f,0.0f};
  scale_ = {1.0f,1.0f,1.0f};
  rotation_ = {0.0f,0.0f,0.0f};

  setTransform();
}

FETransformComponent::FETransformComponent(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale) {
  setTransform(position,rotation,scale);
}

FETransformComponent::~FETransformComponent()
{
}

glm::vec3 FETransformComponent::getPosition() {
	return position_;
}

void FETransformComponent::setPosition(const glm::vec3& pos) {
  position_.x = pos.x;
  position_.y = pos.y;
  position_.z = pos.z;
  setTransform();
}

glm::vec3 FETransformComponent::getScale() {
	return scale_;
}

void FETransformComponent::setScale(glm::vec3 scale) {
  scale_ = scale;
  setTransform();
}

glm::vec3 FETransformComponent::getRotation() {
	return rotation_;
}

void FETransformComponent::setRotation(glm::vec3 rot) {
  rotation_ = rot;
  setTransform();
}

void FETransformComponent::setTransform(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale) {
  transform_ = glm::mat4(1.0f);

  position_ = position;
  scale_ = scale;
  rotation_ = rotation;


  transform_ = glm::translate(transform_, position_);
  transform_ = glm::scale(transform_, scale_);
  
  transform_ = glm::rotate(transform_, glm::radians(rotation_.z), { 0.0f,0.0f,1.0f });
  transform_ = glm::rotate(transform_, glm::radians(rotation_.y), { 0.0f,1.0f,0.0f });
  transform_ = glm::rotate(transform_, glm::radians(rotation_.x), { 1.0f,0.0f,0.0f });
  
  
 
}

void FETransformComponent::setTransform() {

  transform_ = glm::mat4(1.0f);


  transform_ = glm::translate(transform_, position_);
  transform_ = glm::scale(transform_, scale_);
  
  transform_ = glm::rotate(transform_, glm::radians(rotation_.z), { 0.0f,0.0f,1.0f });
  transform_ = glm::rotate(transform_, glm::radians(rotation_.y), { 0.0f,1.0f,0.0f });
  transform_ = glm::rotate(transform_, glm::radians(rotation_.x), { 1.0f,0.0f,0.0f });
 
  

}

glm::mat4x4& FETransformComponent::getTransform() {
  return transform_;
}