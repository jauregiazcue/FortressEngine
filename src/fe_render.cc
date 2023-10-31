#include "fe_render.h"
#include <fe_input.h>
#include <glm/ext/matrix_clip_space.hpp>

#include "fe_constants.h"
#include "fe_shader.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <memory>

FERender::FERender() {


	projection_ = glm::perspective(glm::radians(45.0f), 
																 (float)kWindowWidth / (float)kWindowHeight, 0.1f, 200.0f);


	camera_transform_.setRotation({ 0.0f,0.0f,0.0f });
	view_ = glm::inverse(camera_transform_.getTransform());
	colour_picking_program_ = std::make_shared<FEProgram>(std::vector<FEShader>{
		FEShader("../src/shaders/colourPicking.vert", ShaderType::Vertex),
			FEShader("../src/shaders/colourPicking.frag", ShaderType::Fragment) });
}

void FERender::Render(FEWorld& world) {
	glBindFramebuffer(GL_FRAMEBUFFER, color_picking_buffer_.id_);
	glEnable(GL_DEPTH_TEST);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (int voxel = 0; voxel < world.voxel_list_.size(); voxel++) {
		world.DrawVoxelForColourPicking(voxel,projection_,view_,
			colour_picking_program_.get()->getId());
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_DEPTH_TEST);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); 
	glClear(GL_COLOR_BUFFER_BIT);

	for (int voxel = 0; voxel < world.voxel_list_.size(); voxel++) {
		world.DrawVoxel(voxel, projection_, view_);
	}

}

void FERender::SetCameraPosition(glm::vec3 position) {
	camera_transform_.setPosition(position);
	view_ = glm::inverse(camera_transform_.getTransform());

}

void FERender::SetCameraRotation(glm::vec3 rotation) {
	camera_transform_.setRotation(rotation);
	view_ = glm::inverse(camera_transform_.getTransform());
}

glm::vec3 FERender::GetCameraPosition()
{
	return camera_transform_.getPosition();
}

void FERender::MoveCamera(glm::vec2 directions, glm::vec2 speed) {


	if (directions.x >= 0) {
		glm::mat4 cT = camera_transform_.getTransform();
		glm::vec3 aux{ cT[2].x,cT[2].y,cT[2].z };
		aux = glm::normalize(aux);
		aux *= speed.x;
		camera_transform_.setPosition(camera_transform_.getPosition() - aux);
		view_ = glm::inverse(camera_transform_.getTransform());
	}
	else {
		glm::mat4 cT = camera_transform_.getTransform();
		glm::vec3 aux{ cT[2].x,cT[2].y,cT[2].z };
		aux = glm::normalize(aux);
		aux *= speed.x;
		camera_transform_.setPosition(camera_transform_.getPosition() + aux);
		view_ = glm::inverse(camera_transform_.getTransform());
	}

	if (directions.y >= 0) {
		glm::mat4 cT = camera_transform_.getTransform();
		glm::vec3 aux{ cT[0].x,cT[0].y,cT[0].z };
		aux = glm::normalize(aux);
		aux *= speed.y;
		camera_transform_.setPosition(camera_transform_.getPosition() + aux);
		view_ = glm::inverse(camera_transform_.getTransform());
	}
	else {
		glm::mat4 cT = camera_transform_.getTransform();
		glm::vec3 aux{ cT[0].x,cT[0].y,cT[0].z };
		aux = glm::normalize(aux);
		aux *= speed.y;
		camera_transform_.setPosition(camera_transform_.getPosition() - aux);
		view_ = glm::inverse(camera_transform_.getTransform());
	}


}

void FERender::RotateCamera(glm::vec3 added_rotation) {
	camera_transform_.setRotation(camera_transform_.getRotation() + added_rotation);
	view_ = glm::inverse(camera_transform_.getTransform());
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