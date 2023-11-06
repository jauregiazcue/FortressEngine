

#ifndef __FEDEBUGINTERFACE_H__
#define __FEDEBUGINTERFACE_H__ 1


#include <fe_world.h>
#include <fe_render.h>
class FEScene {
public:
	

	FEScene();

	~FEScene();

	void Update(GLfloat deltaTime,FERender& render);

	void Interface(GLfloat deltaTime, FERender& render);

	int active_voxel_;
	int active_face_;

	bool wireframe_;

	float fps_;

	const float window_size_x = 200.0f;
	const float window_size_y = 450.0f;

	bool world_made_;

	int world_voxel_per_row_;

	FEWorld world_;
};


#endif //__FEDEBUGINTERFACE_H__