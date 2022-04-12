#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aUV;
layout(location = 2) in vec3 aNormal;

uniform mat4 uModel;
uniform mat4 uViewProjection;

out VS_OUT
{
    vec2 vUV;
    vec3 vNormal;
    vec3 vFragPosition;
} vs_out;

void main()
{
    vs_out.vUV = aUV;

	vs_out.vNormal = transpose(inverse(mat3(uModel))) * aNormal;

    vec4 fragPos4 = uModel * vec4(aPosition, 1.0);

	vs_out.vFragPosition = vec3(fragPos4);

    gl_Position = uViewProjection * fragPos4;
}