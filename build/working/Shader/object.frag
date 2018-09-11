#version 330 core

in vec3 SFragPos;
in vec3 SNormal;

out vec4 FragColor;

uniform vec3 UObjectColor;
uniform vec3 ULightColor;
uniform vec3 ULightPosition;
uniform vec3 UCameraPosition;

// when doing specular, view space is easier since the eye is at
// (0, 0, 0)
void main()
{
  float ambient_factor = 0.1f;
  vec3 ambient_color = ambient_factor * ULightColor;

  vec3 norm = normalize(SNormal);
  vec3 light_dir = normalize(ULightPosition - SFragPos);
  float diffuse_factor = max(dot(norm, light_dir), 0.0);
  vec3 diffuse_color = diffuse_factor * ULightColor;

  float specular_factor = 0.5f;
  vec3 view_dir = normalize(UCameraPosition - SFragPos);
  vec3 reflect_light_dir = reflect(-light_dir, norm);
  float dot_result = dot(view_dir, reflect_light_dir);
  float specular_spread = pow(max(dot_result, 0.0f), 250);
  vec3 specular_color = specular_factor * specular_spread * ULightColor;

  vec3 result_color = (specular_color + ambient_color + diffuse_color) * UObjectColor;
  FragColor = vec4(result_color, 1.0);
}
