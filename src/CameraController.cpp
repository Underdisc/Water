#include "Time.h"
#include "Context.h"

#include "CameraController.h"

CameraController::CameraController(Camera & camera) :
  _camera(camera), _keyboardSpeed(8.0f), _keyboardSensitivity(1.0f),
  _controllerSpeed(2.5f), _controllerSensitivity(1.5f), 
  _controllerEpsilon(0.1f)
{}

#include <iostream>

void CameraController::Update()
{
  _keyboardSpeed += Input::MouseWheelMotion() * _keyboardSpeed * 0.1f;
  if (_keyboardSpeed < 0.1f) _keyboardSpeed = 0.1f;

  float speed_product = Time::DT() * _keyboardSpeed;
  float sensitivity_product = Time::DT() * _keyboardSensitivity;

  if (Input::MouseButtonDown(MButton::RIGHT)) {
    std::pair<int, int> mouse_motion = Input::MouseMotion();
    _camera.MoveYaw(mouse_motion.first * sensitivity_product);
    _camera.MovePitch(-mouse_motion.second * sensitivity_product);
  }

  if (Input::KeyDown(Key::W)) _camera.MoveForward(speed_product);
  if (Input::KeyDown(Key::S)) _camera.MoveForward(-speed_product);
  if (Input::KeyDown(Key::D)) _camera.MoveRight(speed_product);
  if (Input::KeyDown(Key::A)) _camera.MoveRight(-speed_product);
  if (Input::KeyDown(Key::E)) _camera.MoveGlobalUp(speed_product);
  if (Input::KeyDown(Key::Q)) _camera.MoveGlobalUp(-speed_product);

  // controller code
  /*const Input::Controller & c = *Input::GetController(0);

  if (c.ButtonDown(CButton::BUMPERLEFT)) _controllerSpeed -= 0.1f;
  if (c.ButtonDown(CButton::BUMPERRIGHT)) _controllerSpeed += 0.1f;

  speed_product = Time::DT() * _controllerSpeed;
  sensitivity_product = Time::DT() * _controllerSensitivity;

  if(c.AnalogActive(CAnalog::STICKLEFTX))
    _camera.MoveRight(speed_product * c.StickLeft().first);
  if (c.AnalogActive(CAnalog::STICKLEFTY))
   _camera.MoveForward(speed_product * c.StickLeft().second);
  if (c.AnalogActive(CAnalog::TRIGGERLEFT))
   _camera.MoveGlobalUp(-speed_product * c.TriggerLeft());
  if (c.AnalogActive(CAnalog::TRIGGERRIGHT))
   _camera.MoveGlobalUp(speed_product * c.TriggerRight());
  if (c.AnalogActive(CAnalog::STICKRIGHTX))
   _camera.MoveYaw(sensitivity_product * c.StickRight().first);
  if (c.AnalogActive(CAnalog::STICKRIGHTY))
   _camera.MovePitch(sensitivity_product * c.StickRight().second);*/


}