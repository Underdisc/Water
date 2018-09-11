/* All content(c) 2016 - 2017 DigiPen(USA) Corporation, all rights reserved. */
/*****************************************************************************/
/*!
\file OpenGLContext.cpp
\author Connor Deakin
\par E-mail: connor.deakin\@digipen.edu
\par Project: Graphics
\date 29/09/2017
\brief
  Implementation for the OpenGLContext.
*/
/*****************************************************************************/

#include <GL\glew.h>

#include "Error.h"
#include "Context.h"

#include "OpenGLContext.h"

// static initializations
int OpenGLContext::_width = 0;
int OpenGLContext::_height = 0;
float OpenGLContext::_aspectRatio = 0.0f;
SDL_GLContext OpenGLContext::_context = nullptr;

/*****************************************************************************/
/*!
\brief
  Initializes an OpenGL context within the SDL window created by the Context
  class.
*/
/*****************************************************************************/
void OpenGLContext::Initialize()
{
  // checking context
  if (!Context::Created()) {
    RootError root_error("OpenGLContext.cpp", "Initialize");
    root_error.Add("The SDL context was not created.");
    throw(root_error);
  }
  // creating gl context
  _context = SDL_GL_CreateContext(Context::SDLWindow());
  glewExperimental = GL_TRUE;
  if (glewInit() != GLEW_OK)
  {
    SDL_Quit();
    RootError root_error("main.cpp", "WindowInit");
    root_error.Add("glew initialization failed.");
    throw(root_error);
  }
  // gl settings / version 3.3
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
    SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
  // Clearing error log due to false positive
  glGetError();
  AdjustViewport();

}

/*****************************************************************************/
/*!
\brief
  Destroys the SDL OpenGL context.
*/
/*****************************************************************************/
void OpenGLContext::Purge()
{
  SDL_GL_DeleteContext(_context);
}

/*****************************************************************************/
/*!
\brief Swaps the current OpenGL write buffer to the screen buffer.
*/
/*****************************************************************************/
void OpenGLContext::Swap()
{
  SDL_GL_SwapWindow(Context::SDLWindow());
}

/*!
\brief Get the aspect ratio of the OpenGL context.
\return The Context's aspect ratio.
*/
float OpenGLContext::AspectRatio()
{
  return _aspectRatio;
}

/*!
\brief Get the width of the OpenGL context.
\return The width of the OpenGL context.
*/
int OpenGLContext::Width()
{
  return _width;
}

/*!
\brief Get the height of the OpenGL context.
\return The height of the OpenGL context.
*/
int OpenGLContext::Height()
{
  return _height;
}

/*!
\brief This function should be the callback function that is called when a 
  window resize event occurs.
*/
void OpenGLContext::AdjustViewport()
{
  SDL_GL_GetDrawableSize(Context::SDLWindow(), &_width, &_height);
  _aspectRatio = (float)_width / (float)_height;
  glViewport(0, 0, _width, _height);
}

