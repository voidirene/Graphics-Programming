#version 400 //TODO: rename?
out vec4 FragColor;
in vec4 v_pos; 

uniform vec3 fogColor;


uniform float maxDist; //fog max distance
uniform float minDist; //fog min distance

void main() 
{
	float dist = abs( v_pos.z ); //return the absolute value (i.e the positive)
	float fogFactor = (maxDist - dist) / (maxDist - minDist);
	fogFactor = clamp( fogFactor, 0.0, 1.0 ); // clamp between 0 and 1
	vec3 lightColor = vec3(1.0,0.2,0.2);
	vec3 color = mix( fogColor, lightColor, fogFactor);

	FragColor = vec4(color, 1.0);
}
