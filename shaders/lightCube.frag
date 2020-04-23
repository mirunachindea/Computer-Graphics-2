#version 410 core

uniform int colorChoice;
out vec4 fColor;

void main() 
{    
	if (colorChoice == 0) 
		fColor = vec4(1.0f, 0.0f, 1.0f, 1.0f);
	else 
		fColor = vec4(0.0f);
}
