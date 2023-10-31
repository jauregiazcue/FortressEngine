
#version 330

out vec4 FragColor;
in vec3 color;

uniform vec3 PickingColour;

void main() {
	
	FragColor = vec4(PickingColour,1.0f);
}