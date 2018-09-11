#version 330 core

in vec3 APosition;

uniform mat4 UTransform = mat4(1, 0, 0, 0,
                               0, 1, 0, 0,
                               0, 0, 1, 0,
                               0, 0, 0, 1);

void main()
{
  gl_Position = UTransform * vec4(APosition.x, APosition.y, APosition.z, 1.0);
}
