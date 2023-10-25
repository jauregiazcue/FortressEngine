

#ifndef __FEVOXEL_H__
#define __FEVOXEL_H__ 1

#include "fe_transform.h"
#include "fe_material.h"
#include <fe_entity.h>

class FEVoxel {
public:
	
	FEVoxel(FETransformComponent transform);
	~FEVoxel();

	std::vector<FEMaterialComponent::Vertex> initFrontFace();
	std::vector<FEMaterialComponent::Vertex> initRightFace();
	std::vector<FEMaterialComponent::Vertex> initBackFace();
	std::vector<FEMaterialComponent::Vertex> initLeftFace();
	std::vector<FEMaterialComponent::Vertex> initTopFace();
	std::vector<FEMaterialComponent::Vertex> initBottomFace();

	FETransformComponent father_transform_;

	std::vector<FEEntity> faces_;

};


#endif //__FEVOXEL_H__