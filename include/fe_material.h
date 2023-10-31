
// Author : jauregiaz
#ifndef __FEMATERIAL_H__
#define __FEMATERIAL_H__ 1
#define GLFW_INCLUDE_NONE

#include <glm/vec3.hpp>
#include <glm/ext/matrix_transform.hpp> 
#include <glad/gl.h>
#include <vector>


#include "fe_shader.h"




class FEMaterialComponent {
public:

	struct Vertex {
		glm::vec3 pos;
		glm::vec3 normal;
		glm::vec4 color;
		glm::vec2 texture;
	};

	FEMaterialComponent(std::shared_ptr<FEProgram> program, const std::vector<Vertex>& vertices,
										const std::vector<int> indices);

	~FEMaterialComponent();

	void enable();
	void enableWithOtherProgram(int program_id);

	void bindAndRender();

	void setUpModel(const glm::mat4x4& transform);

	void setUpCamera(const glm::mat4x4& projection, const glm::mat4x4& view);

	void setUpModelWithOtherProgram(const glm::mat4x4& transform,int program_id);

	void setUpCameraWithOtherProgram(
		const glm::mat4x4& projection, const glm::mat4x4& view, int program_id);

	GLint getUniformLocation(std::string name);
	void setUpReferenceUniform(std::string name, const glm::vec3 value);

	void setUpReferenceUniformWithOtherProgram(
		std::string name, const glm::vec3 value, int program_id);

private:
	std::shared_ptr<FEProgram> program_;

	GLuint VBO_;
	GLuint VAO_;

	GLuint vertex_index_;

	std::vector<Vertex> vertices_;
	std::vector<int> indices_;

};

#endif //__FEMATERIAL_H__


