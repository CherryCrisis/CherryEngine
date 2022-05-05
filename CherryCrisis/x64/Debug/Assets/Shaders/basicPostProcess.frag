#version 330 core

// Varyings
in vec2 vUV;

// Uniforms
uniform sampler2D uColorTexture;
uniform int uTransformType;
uniform float uOffset;

// Shader outputs
out vec4 oColor;

vec3 invert(in vec3 color)
{
    return 1.0 - color;
}

vec4 invert(in vec4 color)
{
    return vec4(invert(color.rgb), 1.0);
}

vec3 sepia(in vec3 color)
{
    vec3 result;
    result.r = dot(color, vec3(0.393, 0.769, 0.189));
    result.g = dot(color, vec3(0.349, 0.686, 0.168));
    result.b = dot(color, vec3(0.272, 0.534, 0.131));

    return result;
}

vec4 sepia(in vec4 color)
{
    return vec4(sepia(color.rgb), 1.0);
}

vec3 grayScale(in vec3 color, in vec3 colorSpectrum = vec3(1.0, 1.0, 1.0))
{
    float average = dot(colorSpectrum, color) / 3.0;
    return vec3(average);
}

vec4 grayScale(in vec4 color, in vec3 colorSpectrum = vec3(1.0, 1.0, 1.0))
{
    return vec4(grayScale(color.rgb, colorSpectrum), 1.0);
}

vec2 directions[9] = vec2[](
    vec2(-1.0, 1.0), // top-left
    vec2( 0.0, 1.0), // top-center
    vec2( 1.0, 1.0), // top-right
    vec2(-1.0, 0.0),   // center-left
    vec2( 0.0, 0.0),   // center-center
    vec2( 1.0, 0.0),   // center-right
    vec2(-1.0,-1.0), // bottom-left
    vec2( 0.0,-1.0), // bottom-center
    vec2( 1.0,-1.0)  // bottom-right    
);

mat3 blurKernel = mat3(1.0, 2.0, 1.0,
                       2.0, 4.0, 2.0,
                       1.0, 2.0, 1.0) / 16.0;

mat3 edgeDetection = mat3(1.0, 1.0, 1.0,
                          1.0,-8.0, 1.0,
                          1.0, 1.0, 1.0);

vec4 kernelEffect(in mat3 kernel)
{
    vec3 result = vec3(0.0);
    for(int i = 0; i < 3; i++)
    {
        for(int j = 0; j < 3; j++)
        {
            vec3 sampleTex = texture(uColorTexture, vUV + directions[i] * uOffset).rgb;
            result += sampleTex * kernel[i][j];
        }
    }

    return vec4(result, 1.0);
}

void main()
{
    switch (uTransformType)
    {
    case 0:
        oColor = texture(uColorTexture, vUV);
        break;

    case 1:
        oColor = grayScale(texture(uColorTexture, vUV), vec3(0.2126 , 0.7152 , 0.0722));
        break;

    case 2:
        oColor = invert(texture(uColorTexture, vUV));
        break;

    case 3:
        oColor = kernelEffect(edgeDetection);
        break;

    case 4:
        oColor = sepia(texture(uColorTexture, vUV));
        break;
    }
}