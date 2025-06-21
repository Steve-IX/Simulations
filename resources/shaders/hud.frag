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
    
    FragColor = color;
} 