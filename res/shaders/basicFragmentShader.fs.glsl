#version 430 core

in vec2 vTexCoords;

out vec4 fragColour;

uniform sampler2D u_Texture;

void main()
{
    // fragColour = vec4(1.0, 0.0, 0.0, 1.0);
    fragColour = texture(u_Texture, vTexCoords);
}