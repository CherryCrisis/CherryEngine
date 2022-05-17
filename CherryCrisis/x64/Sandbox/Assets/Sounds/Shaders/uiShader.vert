#version 330 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec2 aUV;

out vec2 TexCoord;

uniform mat4 uModel;
uniform mat4 projection;

void main()
{
	TexCoord = aUV;
	// TODO: Move the matrix product on the CPU
	gl_Position = projection * uModel * vec4(aPosition, 1.0);
}