#include "fe_voxel.h"
#include "fe_program.h"


FEVoxel::FEVoxel(FETransformComponent& transform, std::vector<FEEntity> faces) :
	father_transform_(transform), faces_(faces){

}

FEVoxel::~FEVoxel() {
}






