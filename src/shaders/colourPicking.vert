#version 330

uniform mat4 model_matrix;
uniform mat4 projection_matrix;

layout (location=0) in vec3 a_position;
layout (location=1) in vec3 a_normal;
layout (location=2) in vec3 a_color;

out vec3 color;

void main() {
	color = abs(a_normal);
	gl_Position = projection_matrix * model_matrix * vec4(a_position, 1.0);
}