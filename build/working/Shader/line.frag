#version 330 core

out vec4 OFragColor;

uniform vec3 UColor = vec3(1.0f, 1.0f, 1.0f);

void main()
{
  OFragColor = vec4(UColor, 1.0f);
}
