
#version 330

out vec4 FragColor;
in vec3 color;

uniform vec3 Colour;

void main() {
	
	FragColor = vec4(Colour,1.0f);
}