#version 400 //TODO: rename?

out vec4 FragColor;

uniform vec3 lightDir;
in vec3 normal;

void main() 
{
	float intensity;
	vec4 color;

	intensity = dot(lightDir, normal); //TODO: maybe normalize(normal) instead?

	if (intensity > 0.95)
		color = vec4(1.0,0.5,0.5,1.0);
	else if (intensity > 0.5)
		color = vec4(0.6,0.3,0.3,1.0);
	else if (intensity > 0.25)
		color = vec4(0.4,0.2,0.2,1.0);
	else
		color = vec4(0.2,0.1,0.1,1.0);

	FragColor = color;
}
