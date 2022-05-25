#version 330 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec2 aUV;
layout (location = 2) in vec3 aNormal;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

out vec3 camera_fwd;
void main()
{

	camera_fwd = vec3(uView[0][2], uView[1][2], uView[2][2]);
    gl_Position = uProjection * uView * uModel * vec4(aPosition,1.0);
}