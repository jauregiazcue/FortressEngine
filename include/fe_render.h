#ifndef __FERENDER_H__
#define __FERENDER_H__ 1

#include <fe_transform.h>

#include <fe_world.h>
#include <fe_framebuffer.h>
#include <fe_program.h>

class FERender {
public:
	FERender();

	void Render(FEWorld& world, FEWindow& window);

	void SetCameraPosition(glm::vec3 position);

	void SetCameraRotation(glm::vec3 rotation);

	glm::vec3 GetCameraPosition();

	void DebugCameraMovement();

	void ColourPicking(FEWindow& window);

	FEFramebuffer color_picking_buffer_;

	int colour_id_;
	bool destroy_;
	private :

		void MoveCamera(glm::vec2 directions, glm::vec2 speed);
		void RotateCamera(glm::vec3 added_rotation);

		glm::mat4 projection_;
		glm::mat4 view_;

		FETransformComponent camera_transform_;
		std::shared_ptr<FEProgram> colour_picking_program_;
		
		
};


#endif //__FERENDER_H__