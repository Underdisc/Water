/* All content(c) 2016 - 2017 DigiPen(USA) Corporation, all rights reserved. */
/*****************************************************************************/
/*!
\file Shader.h
\author Connor Deakin
\par E-mail: connor.deakin\@digipen.edu
\par Project: Graphics
\date 27/09/2016
\brief
  Contains the interface for the Shader class which is responsible for
  reading, compiling, linking, and activating shader programs.
*/
/*****************************************************************************/
#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <GL\glew.h>

#include "Error.h"

/*****************************************************************************/
/*!
\class Shader
\brief
  This is responsible for managing everything to do with shaders.
  Given the files that the vertex shader and fragment shader are located in, the
  shader class will compile, link, and use those shaders to create a shader
  program. That shader can then be managed with this object. Contact me if you 
  want to know about how to write and manipulate shaders.
*/
/*****************************************************************************/
class Shader
{
  public:
    Shader(const std::string & vertex_file, const std::string & fragment_file);
    GLuint GetAttribLocation(const std::string & name);
    GLuint GetUniformLocation(const std::string & name);
    GLuint ID() const;
    virtual void Use() const;
    void Purge() const;
  protected:
    //! The ID of the program created after linking the shaders.
    GLuint _programID;
    //! The name of the vertex shader file.
    std::string _vertexFile;
    //! The name of the fragment shader file.
    std::string _fragmentFile;
  private:
    GLuint CompileShader(const std::string & filename, GLenum type) const;
    std::string ReadShaderFile(const std::string & shader_file) const;
    void CreateProgram(GLuint vshader, GLuint fshader);
};

// Use GetAttribLocation instead
// Here for legacy code
//! Throws an error if an attribute was not found in the shader.
#define CHECKATTRIBUTE(attribute, name, vert_file, frag_file)\
if(attribute == -1) {\
  Error error("Shader.h", "CHECKATTRIBUTE");\
  error.Add("The "##name##" attribute was not found.");\
  error.Add("<Shader Files Involved>");\
  error.Add(vert_file); error.Add(frag_file);\
  throw(error);\
}
// Use GetUniformLocation instead
// Here for legacy code
//! Throws an error if an uniform was not found in the shader.
#define CHECKUNIFORM(uniform, name, vert_file, frag_file)\
if(uniform == -1) {\
  Error error("Shader.h", "CHECKUNIFORM");\
  error.Add("The "##name##" uniform was not found.");\
  error.Add("<Shader Files Involved>");\
  error.Add(vert_file); error.Add(frag_file);\
  throw(error);\
}

#endif // SHADER_H
