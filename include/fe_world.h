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

	void DrawVoxel(int voxel_id, glm::mat4 projection, glm::mat4 view);

	void DrawFace(int voxel_id,int face_id, glm::mat4 projection, glm::mat4 view);

	void DrawVoxelForColourPicking(
		int voxel_id, glm::mat4 projection, glm::mat4 view,int program_id);

	void DrawFaceForColourPicking(
		int voxel_id, int face_id, glm::mat4 projection, glm::mat4 view, int program_id);

	void Culling();
	void GreedyMeshing();
	void GenerateOctreeNodes();
	void SetNodesCenter();
	void SetNodesVoxels();
	void SetNodeCenter(int node_to_set,int corner_voxel, glm::vec3 second_point);

	void CheckFaces(int voxel_to_check);
	

	void ColourPicking(int colour_id, bool destroy);
	void CollisionDetection( FERender& render, bool destroy );

	void CollisionDetectionWithoutOctrees(FERender& render, bool destroy);
	void CollisionDetectionWithOctrees(FERender& render, bool destroy);
	bool CollisionCheck( glm::vec3 voxel, glm::vec3 mouse, float first_range,float second_range );

	glm::vec3 Raycast( FEWindow& window, FERender& render,float distance_helper );

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


	bool CheckVoxelIntersection(glm::vec3 ray_start, glm::vec3 ray_end, int voxel_id,int face_id);
	bool IntersectSegmentVoxelPlane(glm::vec3 ray_start, glm::vec3 ray_end, int voxel_id, int face_type);

	bool CheckNodeIntersection(glm::vec3 ray_start, glm::vec3 ray_end, int node_id, int face_id);
	bool IntersectSegmentNodePlane(glm::vec3 ray_start, glm::vec3 ray_end, int node_id, int face_type);

	FEMaterialComponent* material_list_;

	struct Faces {
		int material_id_;
		bool active_;
		bool colour_picking_active_;
		glm::vec3 color_id_;
		int real_color_id_;
	};

	Faces* GetFaces(int voxel_id,glm::vec3 position);

	enum class VoxelType {
		block = 0,
		air = 1,
		innactive = 2
	};

	#define FRONTFACE 0
	#define LEFTFACES 1
	#define BACKFACES 2
	#define RIGHTFACES 3
	#define TOPFACES 4
	#define BOTTOMFACES 5
	#define FACES 6
	#define NODES 8

	struct CollisionNodes {
		std::vector<int> voxels_;
		glm::vec3 center_;
	};

	CollisionNodes nodes_[NODES];
	float nodes_size_;
	float nodes_fake_size_;
	
	struct Voxel {
		int voxel_id_;
		VoxelType type_;
		Faces faces_[FACES];
		FETransformComponent transform_;
	};
	Voxel* voxel_list_;


	int voxel_per_row_;
	int voxel_per_row_and_colum_;
	int voxel_in_total_;

	int checking = 1;
	int active_faces_;
	long long ms_for_chunk_creation_;
	long long mc_for_voxel_updating_;

	float offset_;
	bool culling_;
	bool greedy_meshing_;
	bool collision_detection_;
	bool octrees_;
	glm::vec3 point_to_check;
};


#endif //__FEWORLD_H__