#version 330 core

// Varyings
in vec2 vUV;

uniform float uExposure = 0.5;
uniform float uGamma = 2.2;

// Uniforms
uniform sampler2D uHdrBuffer;
uniform sampler2D uBloomBlur;

// Shader outputs
layout (location = 0) out vec3 oColor;

void main()
{             
    vec3 pureColor = texture(uHdrBuffer, vUV).rgb;
    vec3 bloomColor = texture(uBloomBlur, vUV).rgb;

    pureColor += bloomColor;
    
    vec3 hdrColor = vec3(1.0) - exp(-uExposure * pureColor);

    oColor = pow(hdrColor, vec3(1.0/uGamma));
}