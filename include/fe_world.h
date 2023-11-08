#ifndef __FEWORLD_H__
#define __FEWORLD_H__ 1

#include <chrono>
#include <vector>
#include <fe_material.h>
#include <fe_transform.h>
#include <fe_window.h>
#include <fe_program.h>

class FEWorld {
public:
	

	FEWorld();

	~FEWorld();

	void init(int voxelPerRow);

	void createChunks();

	void DrawVoxel(int voxel_id_, glm::mat4 projection, glm::mat4 view);

	void DrawFace(int voxel_id_,int face_id_, glm::mat4 projection, glm::mat4 view);

	void DrawVoxelForColourPicking(
		int voxel_id_, glm::mat4 projection, glm::mat4 view,int program_id);

	void DrawFaceForColourPicking(
		int voxel_id_, int face_id_, glm::mat4 projection, glm::mat4 view, int program_id);

	void Culling();

	void CheckFaces(int voxel_to_check);
	

	void ColourPicking(int colour_id, bool destroy);

	void DestroyVoxel(int voxel_id);

	void UpdateAdjacentFacesWhenDestroy(int voxel_to_check);

	void PlaceVoxel(int voxel_id,int face_id);
	void UpdateAdjacentFacesWhenPlace(int voxel_to_check);

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
		glm::vec3 color_id_;
		int real_color_id_;
	};

	Faces* GetFaces(int voxel_id);

	enum class VoxelType {
		block = 0,
		air = 1,
		innactive = 2
	};

	enum class FaceType {
		front = 0,
	};

	#define FRONTFACE 0
	#define LEFTFACES 1
	#define BACKFACES 2
	#define RIGHTFACES 3
	#define TOPFACES 4
	#define BOTTOMFACES 5
	#define FACES 6
	
	struct Voxel {
		int voxel_id_;
		VoxelType type_;
		Faces faces_[FACES];
	};

	std::vector<Voxel> voxel_list_;

	int voxel_per_row_;
	int voxel_per_row_and_colum_;

	int checking = 1;
	int active_faces_;
	long long ms_for_chunk_creation_;

	float offset_;
	bool culling_;
};


#endif //__FEWORLD_H__