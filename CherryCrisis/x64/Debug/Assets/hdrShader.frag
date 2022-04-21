#version 330 core

// Varyings
in vec2 vUV;

uniform float exposure = 0.5;

// Uniforms
uniform sampler2D hdrBuffer;
uniform sampler2D bloomBlur;

// Shader outputs
out vec4 oColor;

void main()
{             
    vec3 pureColor = texture(hdrBuffer, vUV).rgb;
    vec3 bloomColor = texture(bloomBlur, vUV).rgb;

    pureColor += bloomColor; // additive blending

    vec3 hdrColor = pureColor;
    const float gamma = 2.2;
    
    vec3 result;
    
    hdrColor = vec3(1.0) - exp(-pureColor * exposure);
    
    result = pow(hdrColor, vec3(1.0/gamma));
    oColor = vec4(result, 1.0);
} 