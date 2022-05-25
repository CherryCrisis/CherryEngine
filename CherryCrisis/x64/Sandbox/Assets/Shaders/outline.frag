#version 330 core

out vec4 FragColor;

in vec3 eyeNorm;
in vec3 eyePos;
in vec3 camera_fwd;

void main()
{
	float dotEyeNorm = dot(normalize(camera_fwd), normalize(eyeNorm));
	dotEyeNorm = max(0.0, dotEyeNorm);

	vec4 finalColor = vec4(0.0, 0.0, 0.0, 0.0);
    
	if (dotEyeNorm <= 0.2)
	{
		finalColor = vec4(1.0, 0.0, 0.0, 1.0);
	}

	FragColor = finalColor;
}