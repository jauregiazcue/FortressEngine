#ifndef __FECONSTANTS_H__
#define __FECONSTANTS_H__ 1
#include <glm/ext/vector_float4.hpp>

	const int kWindowWidth = 1280;
	const int kWindowHeight = 700;
	const glm::vec4 kClearColor{1.0f,1.0f,1.0f,1.0f};
	const char kWindowName[] = "PalaceEngine";

	const float kRenderPerspectiveNear = 0.1f;
	const float kRenderPerspectiveFar  = 200.0f;

	const float kCameraMovementSpeed = 0.5f;
	const float kCameraRotationSpeed = 1.0f;

#endif //__FECONSTANTS_H__