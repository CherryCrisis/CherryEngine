#version 330 core

layout (location = 0) in vec3 aPosition;

uniform mat4 uLightSpaceModel;

void main()
{
	// TODO: Move the matrix product on the CPU
	gl_Position = uLightSpaceModel * vec4(aPosition, 1.0);
}