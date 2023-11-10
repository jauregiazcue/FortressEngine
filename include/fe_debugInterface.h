

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

	void CSVMaker(FERender& render);
	void CSVUpdate(FERender& render);

	int active_voxel_;
	int active_face_;

	bool wireframe_;

	float fps_;

	const float window_size_x = 200.0f;
	const float window_size_y = 450.0f;

	bool world_made_;
	bool offset_;

	int world_voxel_per_row_;

	FEWorld world_;
	const int csv_file_name_size_ = 80;
	char csv_file_name_[80];
};


#endif //__FEDEBUGINTERFACE_H__