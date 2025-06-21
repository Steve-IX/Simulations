#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;
in vec3 WorldPos;

// Lighting uniforms
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;
uniform float ambientStrength;
uniform float diffuseStrength;
uniform float specularStrength;

// Material uniforms
uniform vec3 material_ambient;
uniform vec3 material_diffuse;
uniform vec3 material_specular;
uniform float material_shininess;

// Aircraft color
uniform vec3 aircraftColor;

// Fog uniforms
uniform float fogDensity;
uniform vec3 fogColor;

void main() {
    // Normalize vectors
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 viewDir = normalize(viewPos - FragPos);
    
    // Ambient lighting
    vec3 ambient = ambientStrength * lightColor * material_ambient;
    
    // Diffuse lighting
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diffuseStrength * diff * lightColor * material_diffuse;
    
    // Specular lighting
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material_shininess);
    vec3 specular = specularStrength * spec * lightColor * material_specular;
    
    // Combine lighting
    vec3 result = ambient + diffuse + specular;
    
    // Apply aircraft color
    result *= aircraftColor;
    
    // Add some variation based on position for better visibility
    float heightFactor = smoothstep(0.0, 100.0, FragPos.y);
    result += vec3(0.1) * heightFactor;
    
    // Add edge highlighting for better aircraft definition
    float edgeFactor = 1.0 - max(dot(norm, viewDir), 0.0);
    edgeFactor = pow(edgeFactor, 3.0);
    result += vec3(0.2) * edgeFactor;
    
    // Apply fog
    float distance = length(viewPos - FragPos);
    float fogFactor = exp(-fogDensity * distance);
    fogFactor = clamp(fogFactor, 0.0, 1.0);
    result = mix(fogColor, result, fogFactor);
    
    // Ensure minimum brightness for visibility
    result = max(result, vec3(0.1));
    
    FragColor = vec4(result, 1.0);
} 