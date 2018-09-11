
#ifndef CAMERACONTROLLER_H
#define CAMERACONTROLLER_H


#include "Camera.h"

class CameraController
{
public:
  CameraController(Camera & camera);
  void Update();
private:
  Camera & _camera;
  float _keyboardSpeed;
  float _keyboardSensitivity;
  float _controllerSpeed;
  float _controllerSensitivity;
  float _controllerEpsilon;
};

#endif // !CAMERACONTROLLER_H
