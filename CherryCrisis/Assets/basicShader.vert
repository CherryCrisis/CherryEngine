#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aUV;
layout(location = 2) in vec3 aNormal;

uniform mat4 uModel;
uniform mat4 uViewProjection;

out vec2 vUV;
out vec3 vNormal;
out vec3 vFragPosition;

void main()
{
    vUV = aUV;
    vNormal = aNormal;

	vNormal = transpose(inverse(mat3(uModel))) * aNormal;

    vec4 fragPos = uModel * vec4(aPosition, 1.0);

	vFragPosition = vec3(fragPos);

    gl_Position = uViewProjection * fragPos;
}