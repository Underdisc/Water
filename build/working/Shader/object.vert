#version 330 core

in vec3 APosition;
in vec3 ANormal;

out vec3 SFragPos;
out vec3 SNormal;

uniform mat4 UProjection = mat4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
uniform mat4 UView = mat4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
uniform mat4 UModel = mat4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);

void main()
{
  gl_Position = UProjection * UView * UModel * vec4(APosition, 1.0);
  SFragPos = vec3(UModel * vec4(APosition, 1.0));
  //This is not a good thing to have here. You have been warned.
  //Calculate this matrix on the cpu and send it to the shader as a uniform.
  // alternatively, use cross product.
  SNormal = mat3(transpose(inverse(UModel))) * ANormal;
}
