
#version 330

out vec4 FragColor;
in vec3 color;

uniform vec3 PickingColour;

void main() {
	
	FragColor = vec4(color,1.0f);
}