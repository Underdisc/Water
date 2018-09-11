/* All content(c) 2016 - 2017 DigiPen(USA) Corporation, all rights reserved. */
/*****************************************************************************/
/*!
\file Camera.cpp
\author Connor Deakin
\par E-mail: connor.deakin\@digipen.edu
\par Project: Graphics
\date 09/06/2017
\brief
  Contains the implementation of the 3D camera class.
*/
/*****************************************************************************/

#include "Camera.h"

#define PI 3.14159265359f
#define EPSILON 1.0e-5f

/*****************************************************************************/
/*!
\brief
  Constructor for the Camera. This will locate the camera at the origin
  looking down the positive Z axis with a global up vector of (0.0, 1.0, 0.0).
*/
/*****************************************************************************/
Camera::Camera() :
  _globalUp(0.0f, 1.0f, 0.0f), _maxPitch(PI / 2.0f - EPSILON),
  _right(-1.0f, 0.0f, 0.0f), _location(0.0f, 1.0f, 0.0f),
  _front(0.0f, 0.0f, 1.0f), _pitch(0.0f), _yaw(PI / 2.0f), 
  _updatedFront(true), _updatedRight(true),  _updatedWorldToCamera(true)
{
  UpdateWorldToCamera();
}

/*!
\brief Set the location of the camera.
\param location The camera's new location.
*/
void Camera::SetLocation(const glm::vec3 & location)
{
  _location = location;
  _updatedWorldToCamera = false;
}

/*!
\brief Move the camera forward by an amount in world units.
\param amount The distance the camera will move forward by.
*/
void Camera::MoveForward(float amount)
{
  if (!_updatedFront)
    UpdateFront();
  _location += _front * amount;
  _updatedWorldToCamera = false;
}

/*!
\brief Move the camera right by an amount in world units.
\param amount The distance the camera will be moved to the right by.
*/
void Camera::MoveRight(float amount)
{
  if (!_updatedRight)
    UpdateRight();
  _location += _right * amount;
  _updatedWorldToCamera = false;
}

/*!
\brief Move the camera up in the direction of the global up vector.
\param amount The amount the camera will be moved in world units.
*/
void Camera::MoveGlobalUp(float amount)
{
  _location += _globalUp * amount;
  _updatedWorldToCamera = false;
}

/*!
\brief Change the pitch of camera by an amount in radians.
\param The amount the pitch will be changed by.
*/
void Camera::MovePitch(float amount)
{
  _pitch += amount;
  // restrict before looking straigt up
  // or straight down.
  if (_pitch > _maxPitch)
    _pitch = _maxPitch;
  else if (_pitch < -_maxPitch)
    _pitch = -_maxPitch;
  _updatedFront = false;
  _updatedWorldToCamera = false;
}

/*!
\brief Change the Yaw of camera by an amount in radians.
\param The amount the pitch will be changed by.
*/
void Camera::MoveYaw(float amount)
{
  _yaw += amount;
  _updatedFront = false;
  _updatedRight = false;
  _updatedWorldToCamera = false;
}

/*!
\brief Gets the location of the camera in world space.
\return The camera's location.
*/
const glm::vec3 & Camera::Location()
{
  return _location;
}

/*****************************************************************************/
/*!
\brief
  Gets the world to camera matrix for the current location and rotation of the
  camera.

\return The world to camera matrix for this camera.
*/
/*****************************************************************************/
const glm::mat4 & Camera::WorldToCamera()
{
  if (!_updatedWorldToCamera)
    UpdateWorldToCamera();
  return _worldToCamera;
}

/*!
\brief Updates the camera's right vector.
*/
void Camera::UpdateRight()
{
  if (!_updatedFront)
    UpdateFront();
  _right = glm::normalize(glm::cross(_front, _globalUp));
  _updatedRight = true;
}

/*!
\brief Updates the camera's front vector.
*/
void Camera::UpdateFront()
{
  _front.x = cos(_yaw) * cos(_pitch);
  _front.y = sin(_pitch);
  _front.z = sin(_yaw) * cos(_pitch);
  _updatedFront = true;
}

/*!
\brief Updates the camera's world to camera matrix.
*/
void Camera::UpdateWorldToCamera()
{
  if (!_updatedFront)
    UpdateFront();
  const glm::vec3 global_up = glm::vec3(0.0f, 1.0f, 0.0f);
  _worldToCamera = glm::lookAt(_location, _location + _front, global_up);
  _updatedWorldToCamera = true;
}
