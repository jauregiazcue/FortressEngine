

#ifndef __FEDEBUGINTERFACE_H__
#define __FEDEBUGINTERFACE_H__ 1

#include <memory>
#include <glm/vec4.hpp>
#include <vector>
#include <fe_world.h>

class FEDebugInterface {
public:
	

	FEDebugInterface();

	~FEDebugInterface();

	void Draw(GLfloat deltaTime);

	int active_voxel_;
	int active_face_;

	bool wireframe_;

	float fps_;

	bool world_made_;
	bool world_culling_;

	int world_voxel_per_row_;

	FEWorld world_;
};


#endif //__FEDEBUGINTERFACE_H__