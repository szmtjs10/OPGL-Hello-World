#version 330 core

out vec4 FragColor;

void main()
{
	// Outline colour 
	//FragColor = vec4(1.0);
	FragColor = 4*vec4(0.04, 0.28, 0.26, 1.0);
}