#version 330 core

#define USE_NORMAL_MAP

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec2 aUV;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

uniform mat4 uModel;
uniform mat4 uViewProjection;

out VS_OUT
{
    vec2 vUV;
    vec3 vNormal;
    vec3 vFragPosition;
	mat3 vTBN;
} vs_out;

void main()
{
    vs_out.vUV = aUV;

    vec3 N = normalize(mat3(uModel) * aNormal);

    #ifdef USE_NORMAL_MAP
    vec3 T = normalize(mat3(uModel) * aTangent);

    // Re-orthogonalize T
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);

    vs_out.vTBN = mat3(T, B, N);
    #else 
    vs_out.vTBN = mat3(1.0);
    #endif


    vs_out.vNormal = N;

    vec4 fragPos4 = uModel * vec4(aPosition, 1.0);

    vs_out.vFragPosition = vec3(fragPos4);

    gl_Position = uViewProjection * fragPos4;
}