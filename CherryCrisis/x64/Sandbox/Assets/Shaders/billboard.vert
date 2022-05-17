#version 330 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec2 aUV;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

out vec2 TexCoord;

void main()
{
	vec3 CameraRight_worldspace = vec3(uView[0][0], uView[1][0], uView[2][0]);
	vec3 CameraUp_worldspace    = vec3(uView[0][1], uView[1][1], uView[2][1]);
	vec3 center   				= vec3(uModel[0][3], uModel[1][3], uModel[2][3]);
	vec3 position = center + CameraRight_worldspace * aPosition.x + CameraUp_worldspace * aPosition.y;
	
	gl_Position = uProjection * uView * uModel * vec4(position, 1.0);
	
    TexCoord = aUV;
}