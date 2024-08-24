#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 normalPos;
layout (location = 2) in vec2 texture_coordinates_input;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 texture_coordinates;

void main()
{
  texture_coordinates = texture_coordinates_input;
  gl_Position = projection * view * model * vec4(aPos, 1.0f);
}
