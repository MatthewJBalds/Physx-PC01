#version 330 core

out vec4 FragColor; // Returns a color

uniform vec3 objectColor;

//Simple shader that colors the model 
void main()
{
	//				  R   G   B  a  Ranges from 0->1
	//FragColor = vec4(1.0f,1.0f,1.0f,1.0f); //Sets the color of the fragment

	FragColor = vec4(objectColor, 1.0);
}