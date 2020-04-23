#version 410 core

in vec3 textureCoordinates;
out vec4 color;

uniform samplerCube skybox;
void main()
{
	vec4 fcolor = texture(skybox, textureCoordinates);
	vec4 fogColor = vec4(0.4f, 0.4f, 0.6f, 1.0f);
	color = 0.8 * fcolor + 0.2 * fogColor;
    //color = texture(skybox, textureCoordinates);
}
