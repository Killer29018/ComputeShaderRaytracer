#version 330 core

precision mediump float;

in vec2 vTexCoords;

out vec4 fragColour;

void main()
{
    fragColour = vec4(vTexCoords.x, vTexCoords.y, 0.0, 1.0);
}