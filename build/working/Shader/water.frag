#version 330 core

in vec3 SNormal;
in vec3 SFragPos;

out vec4 OFragColor;

uniform vec3 UWaterColor;
uniform float UAmbientFactor = 0.2f;
uniform float UDiffuseFactor = 0.4f;
uniform float USpecularFactor = 1.0f;
uniform int USpecularExponent = 20;
uniform vec3 UAmbientColor = vec3(0.160f, 0.909f, 0.960f);
uniform vec3 UDiffuseColor = vec3(0.160f, 0.909f, 0.960f);
uniform vec3 USpecularColor = vec3(1.0f, 1.0f, 1.0f);


uniform vec3 ULightDirection = vec3(1.0f, 1.0f, 0.0f);
uniform vec3 UCameraPosition = vec3(0.0f, 0.0f, 0.0f);
uniform float UTime = 0.0f;

void main()
{
  vec3 ambient_color = UAmbientFactor * UAmbientColor;

  vec3 normal = normalize(SNormal);
  vec3 light_dir = normalize(ULightDirection);
  float ndotl = max(dot(normal, light_dir), 0.0f);
  vec3 diffuse_color = UDiffuseFactor * ndotl * UDiffuseColor;

  vec3 view_dir = normalize(UCameraPosition - SFragPos);
  vec3 reflect_light_dir = reflect(-light_dir, normal);
  float dot_result = dot(view_dir, reflect_light_dir);
  float specular_spread = pow(max(dot_result, 0.0f), USpecularExponent);
  vec3 specular_color = USpecularFactor * specular_spread * USpecularColor;

  vec3 result_color = (specular_color + diffuse_color + ambient_color) * UWaterColor;
  result_color = result_color;

  OFragColor = vec4(result_color, 1.0);


  // rolling color.
  /*float r = (gl_FragCoord.x + gl_FragCoord.y) / 600.0f;
  float g = (sin(mod(UTime * 2.0f + gl_FragCoord.x /  300.0f, 6.28318530718) * 2.0f) + 1.0f) / 2.0f;
  float b = gl_FragCoord.y / 600.0f;
  OFragColor = vec4(r, g, b, 1.0f);*/
}
