#pragma once
#include "Shader.h"

class LineShader : public Shader
{
public:
  LineShader() : Shader("Shader/line.vert", "Shader/line.frag")
  {
    APositon = GetAttribLocation("APosition");
    UTransform = GetUniformLocation("UTransform");
    UColor = GetUniformLocation("UColor");
  }
  GLuint APositon;
  GLuint UTransform;
  GLuint UColor;
};