

#ifndef __FEVOXEL_H__
#define __FEVOXEL_H__ 1

#include "fe_transform.h"
#include "fe_material.h"
#include <fe_entity.h>

class FEVoxel {
public:
	
	FEVoxel(FETransformComponent& transform, std::vector<FEEntity> faces);
	~FEVoxel();


	FETransformComponent& father_transform_;

	std::vector<FEEntity> faces_;

};


#endif //__FEVOXEL_H__