
// Author : jauregiaz
#ifndef __FETRANSFORM_H__
#define __FETRANSFORM_H__ 1

#include <glm/ext/matrix_transform.hpp> 


class FETransformComponent {
public :

	FETransformComponent();

	FETransformComponent(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale);

	~FETransformComponent();

	glm::vec3 getPosition();

	void setPosition(const glm::vec3& pos);

	glm::vec3 getScale();

	void setScale(glm::vec3 scale);

	glm::vec3 getRotation();

	void setRotation(glm::vec3 rot);

	glm::mat4x4& getTransform();

	void setTransform(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale);
private :

	void setTransform();
	glm::vec3 position_;
	glm::vec3 scale_;
	glm::vec3 rotation_;
	glm::mat4x4 transform_;
};



#endif //__FETRANSFORM_H__


