

#ifndef __FEENTITY_H__
#define __FEENTITY_H__ 1

#include "fe_transform.h"
#include "fe_material.h"

class FEEntity {
public:
	
	FEEntity(FETransformComponent& transform, FEMaterialComponent& material);
	~FEEntity();

	void Draw(glm::mat4 projection, glm::mat4 view);

	FETransformComponent& transform_;
	FEMaterialComponent& material_;

	bool active_;

};


#endif //__FEENTITY_H__