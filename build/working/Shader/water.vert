#version 330 core

in vec3 APosition;
in vec3 ANormal;
in vec3 AOffset;

out vec3 SNormal;
out vec3 SFragPos;

uniform mat4 UTransform = mat4(1, 0, 0, 0,
                               0, 1, 0, 0,
                               0, 0, 1, 0,
                               0, 0, 0, 1);

void main()
{
  vec3 pos_fin = APosition + AOffset;
  gl_Position = UTransform * vec4(pos_fin.x, pos_fin.y, pos_fin.z, 1.0);
  SNormal = ANormal;
  SFragPos = pos_fin;
}
