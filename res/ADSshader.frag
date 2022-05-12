#version 330 core //TODO: rename

in vec3 Normal;  
in vec3 FragPos;  
  
uniform vec3 lightPos; 
uniform vec3 viewPos; 
uniform vec3 lightColor;

out vec4 FragColor;

// texture
in vec2 TexCoords;
uniform sampler2D diffuse;

void main()
{
	// texture
	FragColor = texture2D(diffuse, TexCoords);

    // ambient
    float ambientStrength = 0.05f;
    vec3 ambient = ambientStrength * lightColor;
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // specular
    float specularStrength = 0.4f;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64);
    vec3 specular = specularStrength * spec * lightColor;  
        
    vec3 result = ambient + diffuse + specular;
    FragColor += vec4(result, 1.0);
} 