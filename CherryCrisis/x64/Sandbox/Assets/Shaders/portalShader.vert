#version 330 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec2 aUV;

uniform mat4 uModel;
uniform mat4 uModelLinked;
uniform mat4 uViewProjection;

out vec4 uScreenPos;

vec4 ComputeScreenPos (vec4 pos) 
{
  vec4 o = pos * 0.5f;
  o.xy = vec2(o.x, o.y) + o.w;
 
  o.zw = pos.zw;
  return o;
}

void main()
{
    gl_Position = uViewProjection * uModel * vec4(aPosition, 1.0);
	
	uScreenPos = ComputeScreenPos(gl_Position);
}