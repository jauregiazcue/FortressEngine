#ifndef __FERENDER_H__
#define __FERENDER_H__ 1

#include <fe_transform.h>
#include <fe_entity.h>

class FERender {
public:
	FERender();

	void Render(std::vector<FEEntity>& entity_list);

	void SetCameraPosition(glm::vec3 position);

	void SetCameraRotation(glm::vec3 rotation);

	glm::vec3 GetCameraPosition();

	void DebugCameraMovement();

	private :

		void MoveCamera(glm::vec2 directions, glm::vec2 speed);
		void RotateCamera(glm::vec3 added_rotation);

		glm::mat4 projection_;
		glm::mat4 view_;

		FETransformComponent cameraTransform_;
};


#endif //__FERENDER_H__