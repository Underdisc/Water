#version 330 core

out vec4 AFragColor;

uniform vec3 ULightColor;

void main()
{
  AFragColor = vec4(ULightColor, 1.0f);
}
