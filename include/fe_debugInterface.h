

#ifndef __FEDEBUGINTERFACE_H__
#define __FEDEBUGINTERFACE_H__ 1

#include <memory>
#include <glm/vec4.hpp>
#include <vector>
#include <fe_voxel.h>

class FEDebugInterface {
public:
	

	FEDebugInterface();

	~FEDebugInterface();

	void Draw(std::vector<FEVoxel>& voxel_list);

	int active_voxel_;
	int active_face_;

	bool wireframe_;

};


#endif //__FEDEBUGINTERFACE_H__