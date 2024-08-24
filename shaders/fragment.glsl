#version 330 core

out vec4 FragColor;

in vec3 normal;
in vec3 fragmentPos;
in vec2 texture_coordinates;

uniform sampler2D diffuse_map0;

void main()
{
  FragColor = texture(diffuse_map0, texture_coordinates);
}

