#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform vec3 hudColor;
uniform float alpha;
uniform bool useTexture;
uniform sampler2D hudTexture;

void main() {
    vec4 color = vec4(hudColor, alpha);
    
    if (useTexture) {
        vec4 texColor = texture(hudTexture, TexCoord);
        color = texColor * vec4(hudColor, alpha);
    }
    
    // Add subtle glow effect for better readability
    float glow = 0.2;
    vec3 glowColor = hudColor * (1.0 + glow);
    color.rgb = mix(color.rgb, glowColor, 0.3);
    
    // Anti-aliasing for crisp edges
    color.a *= alpha;
    
    FragColor = color;
} 