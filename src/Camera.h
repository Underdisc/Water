/* All content(c) 2016 - 2017 DigiPen(USA) Corporation, all rights reserved. */
/*****************************************************************************/
/*!
\file Camera.h
\author Connor Deakin
\par E-mail: connor.deakin\@digipen.edu
\par Project: Graphics
\date 9/6/2017
\brief
  Interface for the 3D, glm based camera class.
*/
/*****************************************************************************/

#ifndef CAMERA_H
#define CAMERA_H

#include <GLM\glm\vec3.hpp>
#include <GLM\glm\gtc\matrix_transform.hpp>

/*****************************************************************************/
/*!
\class Camera
\brief
  A 3D, glm based camera.

\par
  Important Notes
  - All amounts should be given in world units or radians depending on the
    nature of the operation.
  - The camera will not go over a pitch of PI / 2 nor under a pitch of - PI / 2
*/
/*****************************************************************************/
class Camera
{
public:
  Camera();
  void SetLocation(const glm::vec3 & location);
  void MoveForward(float amount);
  void MoveRight(float amount);
  void MoveGlobalUp(float amount);
  void MovePitch(float amount);
  void MoveYaw(float amount);
  const glm::vec3 & Location();
  const glm::mat4 & WorldToCamera();
private:
  void UpdateRight();
  void UpdateFront();
  void UpdateWorldToCamera();
  //! Camera's global up vector.
  const glm::vec3 _globalUp;
  //! Max pitch of the camera.
  const float _maxPitch;
  //! Camera's front vector.
  glm::vec3 _front;
  //! Camera's right vector.
  glm::vec3 _right;
  //! Camera's location in world space.
  glm::vec3 _location;
  //! Camera's pitch in radians.
  float _pitch;
  //! Camera's yaw in radians.
  float _yaw;
  //! Camera's world to camera matrix.
  glm::mat4 _worldToCamera;
  //! Tracks if the front vector is up to date with the rest of the camera.
  bool _updatedFront;
  //! Tracks if the right vector is up to date with the rest of the camera.
  bool _updatedRight;
  //! Tracks if the world to camera matrix is up to date.
  bool _updatedWorldToCamera;
};

#endif // !CAMERA_H

