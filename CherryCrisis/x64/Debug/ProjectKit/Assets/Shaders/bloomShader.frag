#version 330 core

// Varyings
in vec2 vUV;

// Uniforms
uniform sampler2D brightImage;

uniform bool horizontal;
uniform float weight[5] = float[] (0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162);

// Shader outputs
layout (location = 0) out vec3 oColor;

void main()
{             
    vec2 tex_offset = 1.0 / textureSize(brightImage, 0); // gets size of single texel
    vec3 result = texture(brightImage, vUV).rgb * weight[0];

    float xoffset = float(horizontal) * tex_offset.x;
    float yoffset = float(!horizontal) * tex_offset.y;

    for(int i = 1; i < 5; ++i)
    {
        vec2 uvoffset = i * vec2(xoffset, yoffset);

        result += texture(brightImage, vUV + uvoffset).rgb * weight[i];
        result += texture(brightImage, vUV - uvoffset).rgb * weight[i];
    }

     oColor = result;
}