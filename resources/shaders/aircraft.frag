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
    
    // Enhanced ambient lighting with sky contribution
    vec3 skyAmbient = vec3(0.4, 0.5, 0.7) * 0.3;
    vec3 ambient = (ambientStrength * lightColor * material_ambient) + skyAmbient;
    
    // Enhanced diffuse lighting with wrap-around
    float wrapDiff = max(dot(norm, lightDir) * 0.5 + 0.5, 0.0);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diffuseStrength * mix(wrapDiff * 0.5, diff, 0.7) * lightColor * material_diffuse;
    
    // Specular lighting with Blinn-Phong
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), material_shininess * 2.0);
    vec3 specular = specularStrength * spec * lightColor * material_specular;
    
    // Rim lighting for better definition
    float rimPower = 3.0;
    float rimStrength = 0.4;
    float rim = 1.0 - max(dot(norm, viewDir), 0.0);
    rim = pow(rim, rimPower);
    vec3 rimColor = lightColor * rim * rimStrength;
    
    // Combine lighting
    vec3 result = ambient + diffuse + specular + rimColor;
    
    // Apply aircraft color with metallic variation
    vec3 baseColor = aircraftColor;
    
    // Add panel lines and detail using position
    float panelDetail = sin(WorldPos.x * 20.0) * sin(WorldPos.z * 20.0) * 0.02;
    baseColor *= (1.0 + panelDetail);
    
    result *= baseColor;
    
    // Altitude-based atmospheric tinting
    float altitude = FragPos.y;
    float altitudeFactor = smoothstep(0.0, 2000.0, altitude);
    vec3 highAltitudeTint = vec3(0.9, 0.95, 1.0);
    result = mix(result, result * highAltitudeTint, altitudeFactor * 0.2);
    
    // Enhanced edge highlighting for better aircraft definition
    float edgeFactor = 1.0 - max(dot(norm, viewDir), 0.0);
    edgeFactor = pow(edgeFactor, 2.5);
    result += vec3(0.3, 0.35, 0.4) * edgeFactor * 0.5;
    
    // Improved atmospheric fog with distance-based color shift
    float distance = length(viewPos - FragPos);
    float fogFactor = exp(-fogDensity * distance * 0.0001);
    fogFactor = clamp(fogFactor, 0.0, 1.0);
    
    // Distance-based fog color (bluer at distance)
    vec3 distanceFogColor = mix(vec3(0.7, 0.8, 0.95), fogColor, clamp(distance / 10000.0, 0.0, 1.0));
    result = mix(distanceFogColor, result, fogFactor);
    
    // Ensure minimum brightness for visibility
    result = max(result, vec3(0.05));
    
    // Slight color grading for better visuals
    result = pow(result, vec3(0.95)); // Gamma adjustment
    
    FragColor = vec4(result, 1.0);
} 