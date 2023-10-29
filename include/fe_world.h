#ifndef __FEWORLD_H__
#define __FEWORLD_H__ 1

#include <chrono>
#include <vector>
#include <fe_material.h>
#include <fe_transform.h>

#include <fe_program.h>

class FEWorld {
public:
	

	FEWorld(int voxelPerRow);

	~FEWorld();

	void createChunks();

	void DrawVoxel(int voxel_id_, glm::mat4 projection, glm::mat4 view);

	void DrawFace(int voxel_id_,int face_id_, glm::mat4 projection, glm::mat4 view);

	void Culling();

	void CheckFaces(int voxel_to_check);

	std::vector<FEMaterialComponent::Vertex> initFrontFace();
	std::vector<FEMaterialComponent::Vertex> initRightFace();
	std::vector<FEMaterialComponent::Vertex> initBackFace();
	std::vector<FEMaterialComponent::Vertex> initLeftFace();
	std::vector<FEMaterialComponent::Vertex> initTopFace();
	std::vector<FEMaterialComponent::Vertex> initBottomFace();

	


	
	std::vector<FEMaterialComponent> material_list_;
	std::vector<FETransformComponent> transform_list_;
	struct Faces {
		int material_id_;
		bool active_;
	};

	enum class VoxelType {
		block = 0,
		air = 1,
		other = 2
	};

	struct Voxel {
		int voxel_id_;
		VoxelType type_;
		Faces faces_[6];
	};

	std::vector<Voxel> voxel_list_;

	int voxelPerRow_;
	const int how_many_faces_ = 6;
	int active_triangles_;
	long long ms_for_chunk_creation_;
};


#endif //__FEWORLD_H__