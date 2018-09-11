/* All content(c) 2016 - 2017 DigiPen(USA) Corporation, all rights reserved. */
/*****************************************************************************/
/*!
\file
\author Connor Deakin
\par E-mail: connor.deakin\@digipen.edu
\par Project: X-Craft
\date 04/02/2017
\brief
  Contains the implementation of the OpenGLError class.
*/
/*****************************************************************************/

#include "OpenGLError.h"

/*****************************************************************************/
/*!
\brief
  This is exactly the same as the constructor for the base Error class. It
  takes in the name of the file and function where the Error occured.

\param file
  The name of the file that the OpenGLError was thrown from.
\param function
  The name of the function that the OpenGLError was thrown from.
*/
/*****************************************************************************/
OpenGLError::OpenGLError(const char * file, const char * function)
: Error(file, function)
{}

/*****************************************************************************/
/*!
\brief
  Given the error code of the OpenGL error that was returned from glGetError,
  this function will add a line to the Error log with the name of the
  error code.

\param code
  The error code returned from glGetError.
*/
/*****************************************************************************/
void OpenGLError::Code(GLenum code)
{
  Add("> OPENGL ERROR CODE");
  switch (code)
  {
    case GL_INVALID_ENUM: 
      Add("GL_INVALID_ENUM"); break;
    case GL_INVALID_VALUE: 
      Add("GL_INVALID_VALUE"); break;
    case GL_INVALID_OPERATION:
      Add("GL_INVALID_OPERATION"); break;
    case GL_INVALID_FRAMEBUFFER_OPERATION:
      Add("GL_INVALID_FRAMEBUFFER_OPERATION"); break;
    case GL_OUT_OF_MEMORY:
      Add("GL_OUT_OF_MEMORY"); break;
    case GL_STACK_UNDERFLOW:
      Add("GL_STACK_UNDERFLOW"); break;
    case GL_STACK_OVERFLOW:
      Add("GL_STACK_OVERFLOW"); break;
  }
}