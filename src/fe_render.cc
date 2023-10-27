#include "fe_render.h"
#include <fe_input.h>
#include <glm/ext/matrix_clip_space.hpp>

#include "fe_constants.h"

FERender::FERender() {
	projection_ = glm::perspective(glm::radians(45.0f), 
																 (float)kWindowWidth / (float)kWindowHeight, 0.1f, 100.0f);


	cameraTransform_.setRotation({ 0.0f,0.0f,0.0f });
	view_ = glm::inverse(cameraTransform_.getTransform());
}

void FERender::Render(FEWorld& world) {
	for (int voxel = 0; voxel < world.voxel_list_.size(); voxel++) {
		world.DrawVoxel(voxel,projection_,view_);
	}
}

void FERender::SetCameraPosition(glm::vec3 position) {
	cameraTransform_.setPosition(position);
	view_ = glm::inverse(cameraTransform_.getTransform());

}

void FERender::SetCameraRotation(glm::vec3 rotation) {
	cameraTransform_.setRotation(rotation);
	view_ = glm::inverse(cameraTransform_.getTransform());
}

glm::vec3 FERender::GetCameraPosition()
{
	return cameraTransform_.getPosition();
}

void FERender::MoveCamera(glm::vec2 directions, glm::vec2 speed) {


	if (directions.x >= 0) {
		glm::mat4 cT = cameraTransform_.getTransform();
		glm::vec3 aux{ cT[2].x,cT[2].y,cT[2].z };
		aux = glm::normalize(aux);
		aux *= speed.x;
		cameraTransform_.setPosition(cameraTransform_.getPosition() - aux);
		view_ = glm::inverse(cameraTransform_.getTransform());
	}
	else {
		glm::mat4 cT = cameraTransform_.getTransform();
		glm::vec3 aux{ cT[2].x,cT[2].y,cT[2].z };
		aux = glm::normalize(aux);
		aux *= speed.x;
		cameraTransform_.setPosition(cameraTransform_.getPosition() + aux);
		view_ = glm::inverse(cameraTransform_.getTransform());
	}

	if (directions.y >= 0) {
		glm::mat4 cT = cameraTransform_.getTransform();
		glm::vec3 aux{ cT[0].x,cT[0].y,cT[0].z };
		aux = glm::normalize(aux);
		aux *= speed.y;
		cameraTransform_.setPosition(cameraTransform_.getPosition() + aux);
		view_ = glm::inverse(cameraTransform_.getTransform());
	}
	else {
		glm::mat4 cT = cameraTransform_.getTransform();
		glm::vec3 aux{ cT[0].x,cT[0].y,cT[0].z };
		aux = glm::normalize(aux);
		aux *= speed.y;
		cameraTransform_.setPosition(cameraTransform_.getPosition() - aux);
		view_ = glm::inverse(cameraTransform_.getTransform());
	}


}

void FERender::RotateCamera(glm::vec3 added_rotation) {
	cameraTransform_.setRotation(cameraTransform_.getRotation() + added_rotation);
	view_ = glm::inverse(cameraTransform_.getTransform());
}


void FERender::DebugCameraMovement() {
	if (FEInput::keyDown(Key::KEY_W)) {
		MoveCamera({ 1.0f,0.0f }, { 0.5f,0.0f });
	}
	if (FEInput::keyDown(Key::KEY_S)) {
		MoveCamera({ -1.0f,0.0f }, { 0.5f,0.0f });
	}
	if (FEInput::keyDown(Key::KEY_A)) {
		MoveCamera({ 0.0f,-1.0f }, { 0.0f,0.5f });
	}
	if (FEInput::keyDown(Key::KEY_D)) {
		MoveCamera({ 0.0f,1.0f }, { 0.0f,0.5f });
	}

	if (FEInput::keyDown(Key::KEY_UP)) {
		RotateCamera({ 1.0f,0.0f,0.0f });
	}
	if (FEInput::keyDown(Key::KEY_DOWN)) {
		RotateCamera({ -1.0f,0.0f,0.0f });
	}
	if (FEInput::keyDown(Key::KEY_LEFT)) {
		RotateCamera({ 0.0f,1.0f,0.0f });
	}
	if (FEInput::keyDown(Key::KEY_RIGHT)) {
		RotateCamera({ 0.0f,-1.0f,0.0f });
	}


}