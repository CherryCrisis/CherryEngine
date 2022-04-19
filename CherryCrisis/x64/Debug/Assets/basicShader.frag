#version 430 core

//#define BLINN_PHONG
#define USE_NORMAL_MAP

#define PCF 3
#define PCFSampleCount (1 + 2 * PCF) * (1 + 2 * PCF)
#define PCFFactor 1.0 / (PCFSampleCount)

// Varyings
in VS_OUT
{
	vec2 vUV;
	vec3 vNormal;
	vec3 vFragPosition;
	mat3 vTBN;
} fs_in;

struct Material
{
	vec3	ambientCol;
	vec3	diffuseCol;
	vec3	specularCol;
	vec3	emissiveCol;
	float	shininess;

	sampler2D albedoTex;
	sampler2D normalMap;
};

uniform Material uMaterial;

struct Light
{
	bool isEnabled;
	bool isPoint;
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	mat4 lightSpace;
};

#define NBR_LIGHTS 8
uniform sampler2D uShadowMaps[NBR_LIGHTS];
uniform Light uLights[NBR_LIGHTS];

uniform vec3 uViewPosition;

out vec4 oColor;

float getDirectionalShadow(int index)
{
    // Perspcetive divide
    vec4 fragPosLightSpace = uLights[index].lightSpace * vec4(fs_in.vFragPosition, 1.0);
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

    // Avoid shadow out of the frustum
    if (projCoords.z > 1.0)
        return 0.0;

    // [0,1]
    projCoords = projCoords * 0.5 + 0.5;

    vec3 lightDir = normalize(uLights[index].position.xyz - fs_in.vFragPosition);

    float slopeFactor = 1.0 - dot(fs_in.vNormal, lightDir);

    float minBias = 0.00005;
    float maxBias = 0.0005;
    float bias = max(minBias * slopeFactor, maxBias);
    float currentDepth = projCoords.z - bias;

    // Apply Percentage-Closer filtering to avoid "stair" shadows
    // Use to soft shadow boders
    float shadow = 0.0;
        
    // Calculate the texel size from the depth texture size
    vec2 texelSize = 1.0 / textureSize(uShadowMaps[index], 0);
        
    for (int x = -PCF; x <= PCF; x++)
    {
        for (int y = -PCF; y <= PCF; y++)
        {
            float pcfDepth = texture(uShadowMaps[index], projCoords.xy + vec2(x, y) * texelSize).r;

            // Compare pcf and current depth of fragment to determine shadow
            shadow += float(currentDepth > pcfDepth);
        }
    }

    return shadow * PCFFactor;
}

void getLightColor(in vec3 normal, out vec3 ambient, out vec3 diffuse, out vec3 specular)
{
	ambient = diffuse = specular = vec3(0.0);
	
	vec3 viewDir = normalize(uViewPosition - fs_in.vFragPosition);

	for (int i = 0; i < NBR_LIGHTS; i++)
	{
		Light light = uLights[i];

		if (!light.isEnabled)
			continue;

	    float shadow = 1.0 - getDirectionalShadow(i);

		// Compute ambient
		ambient += light.ambient;

		// Get light direction, if the light is a point light or a directionnal light
		vec3 lightDir = light.position - float(light.isPoint) * fs_in.vFragPosition;
	
		// Compute the light direction and the distance between the fragment and the light
		// Normalize it manually
		float lightDist = length(lightDir);
		lightDir /= lightDist;

		// Pre-compute normal lightDir
		float NdotL = dot(normal, lightDir);

		float diffuseVal = max(NdotL, 0.0);

		if (diffuseVal <= 0)
			continue;

		// Compute diffuse
		diffuse += diffuseVal * light.diffuse * shadow;

		// Compute specular
		#ifdef BLINN_PHONG
			// Blinn phong
			vec3 H = normalize(lightDir + viewDir);
			float dotValue = dot(normal, H);
		#else
			// Phong
			vec3 reflectDir = 2.0 * NdotL * normal - lightDir; 
			float dotValue = dot(viewDir, reflectDir);
		#endif

		specular += pow(clamp(dotValue, 0.0, 1.0), uMaterial.shininess) * light.specular * shadow;
	}
}

vec3 getShadedColor(in vec3 normal)
{
	vec3 ambient, diffuse, specular;

	getLightColor(normal, ambient, diffuse, specular);

	vec3 ambientColor = uMaterial.ambientCol * ambient;

	vec3 diffuseColor = uMaterial.diffuseCol * diffuse;

	vec3 albedoColor = (ambientColor + diffuseColor) * texture(uMaterial.albedoTex, fs_in.vUV).rgb;

	vec3 specularColor = uMaterial.specularCol * specular;

	vec3 emissiveColor = uMaterial.emissiveCol;

	return albedoColor + specularColor + emissiveColor;
}

void main()
{
	#ifdef USE_NORMAL_MAP
	vec3 N = texture(uMaterial.normalMap, fs_in.vUV).xyz * 2.0 - 1.0;
    N = normalize(fs_in.vTBN * N);

	vec3 shadedColor = getShadedColor(N);
	#else 
	vec3 shadedColor = getShadedColor(normalize(fs_in.vNormal));
	#endif
    oColor = vec4(shadedColor, 1.0);
}