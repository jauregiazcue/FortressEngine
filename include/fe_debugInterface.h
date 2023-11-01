

#ifndef __FEDEBUGINTERFACE_H__
#define __FEDEBUGINTERFACE_H__ 1

#include <memory>
#include <glm/vec4.hpp>
#include <vector>
#include <fe_world.h>
#include <fe_window.h>

class FEScene {
public:
	

	FEScene();

	~FEScene();

	void Update(GLfloat deltaTime,int colour_id,bool destroy,int texture_id);

	void ColourPickingInput(FEWindow& window);
	void Interface(GLfloat deltaTime, int texture_id);

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