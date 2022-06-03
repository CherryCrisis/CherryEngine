#version 330 core

out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D aTex;
uniform int uHovered;

void main()
{
    FragColor = texture(aTex, TexCoord);
	
	if (uHovered == 1)
	{	
		FragColor *= vec4(.7f,.7f,.7f,1.f);
	}
}