/* All content(c) 2016 - 2017 DigiPen(USA) Corporation, all rights reserved. */
/*****************************************************************************/
/*!
\file
\author Connor Deakin
\par E-mail: connor.deakin\@digipen.edu
\par Project: Graphics
\date 04/02/2017
\brief
  Contains the interface for an OpenGLError. This class inherits from the Error
  class. It is an Error class specifically built for OpenGL errors.
*/
/*****************************************************************************/

#include <GL/glew.h>

#include "Error.h"

/*****************************************************************************/
/*!
\class OpenGLError
\brief
  This is an extension of the error class. It is intended to be used when
  catching OpenGL errors that are returned from glGetError. This inherits
  from Error, so as long as an Error is caught, an OpenGLError will also be
  caught.
*/
/*****************************************************************************/
class OpenGLError : public Error
{
  public:
    OpenGLError(const char * file, const char * function);
    void Code(GLenum code);
};

//! Shorter syntax for throwing a OpenGLError as an exception.
#define OPENGLERRORCHECK(file, function, message, error_code)\
if (error_code) {\
  OpenGLError error(file, function);\
  error.Code(error_code);\
  error.Add("> DESCRIPTION");\
  error.Add(message);\
  throw(error);\
}
