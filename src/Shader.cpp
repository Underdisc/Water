/* All content(c) 2016 - 2017 DigiPen(USA) Corporation, all rights reserved. */
/*****************************************************************************/
/*!
\file Shader.cpp
\author Connor Deakin
\par E-mail: connor.deakin\@digipen.edu
\par Project: Graphics
\date 27/09/2016
\brief
  Contains the implementation of the Shader class.
*/
/*****************************************************************************/

#include <iostream>
#include <fstream>

#include "OpenGLError.h"

#include "Shader.h"

//! The size of the buffer (in bytes) that is used to store the errors
// encountered during the shader link and compile steps.
#define ERROR_BUFFER_SIZE 512

/*****************************************************************************/
/*!
\brief
  The constructor for a shader. Given the path to the vertex and fragment shader
  files from the executable directory, the constructor will compile and link
  the shaders.

\param vertex_file
  The path to the vertex shader from the executable.

\param fragment_file
  The path to the fragment shader from the executable.
*/
/*****************************************************************************/
Shader::Shader(const std::string & vertex_file, 
               const std::string & fragment_file) :
_vertexFile(vertex_file), _fragmentFile(fragment_file)
{
  try
  {
    //compile shaders
    GLuint vshader = CompileShader(vertex_file, GL_VERTEX_SHADER);
    GLuint fshader = CompileShader(fragment_file, GL_FRAGMENT_SHADER);
    //link shaders
    CreateProgram(vshader, fshader);
  }
  catch (Error & error) 
  { 
    error.Add("<Shader Files Involved>");
    error.Add(vertex_file.c_str());
    error.Add(fragment_file.c_str());
    ErrorLog::Write(error);
  }
}

/*****************************************************************************/
/*!
\brief
  Will find the location of an attribute given the name of the attribute.
  Writes an error to the ErrorLog if the attribute is not found.

\par Important Notes
  - Call Use() on the shader instance before calling this function.

\param name
  The name of the attribute being searched for.

\return The GLuint attribute location.
*/
/*****************************************************************************/
GLuint Shader::GetAttribLocation(const std::string & name)
{
  GLuint attribute_location = glGetAttribLocation(_programID, name.c_str());
  if (attribute_location == -1) {
    Error error("Shader.cpp", "GetUniformLocation");
    error.Add("An attribute was not found.");
    error.Add("<Attribute name>");
    error.Add(name.c_str());
    error.Add("<Shader Files Involved>");
    error.Add(_vertexFile); error.Add(_fragmentFile);
    ErrorLog::Write(error);
  }
  return attribute_location;
}

/*****************************************************************************/
/*!
\brief
  Will find the location of an uniform given the name of the uniform. Writes
  an error to the ErrorLog if the uniform is not found.

\par Important Notes
  - Call Use() on the shader instance before calling this function.

\param name
  The name of the uniform being searched for.

\return The GLuint uniform location.
*/
/*****************************************************************************/
GLuint Shader::GetUniformLocation(const std::string & name)
{
  GLuint uniform_location = glGetUniformLocation(_programID, name.c_str());
  if (uniform_location == -1) {
    Error error("Shader.cpp", "GetUniformLocation");
    error.Add("An uniform was not found.");
    error.Add("<Uniform name>");
    error.Add(name.c_str());
    error.Add("<Shader Files Involved>");
    error.Add(_vertexFile); error.Add(_fragmentFile);
    ErrorLog::Write(error);
  }
  return uniform_location;
}

/*****************************************************************************/
/*!
\brief
  This will return the program ID that OpenGL uses to identify the shader
  program. This ID is to be used when making OpenGL calls that manipulate this
  shader.

\return The OpenGL program ID.
*/
/*****************************************************************************/
GLuint Shader::ID() const
{
  return _programID;
}

/*****************************************************************************/
/*!
\brief
  Will use the shader program created by the shader class for all OpenGL
  rendering steps that are called after the Shader is activated. Make sure to 
  call this before executing any drawing code.
*/
/*****************************************************************************/
void Shader::Use() const
{
  glUseProgram(_programID);
}

/*****************************************************************************/
/*!
\brief
  Deletes the Shader program. Do this before the shader is de-allocated.
*/
/*****************************************************************************/
void Shader::Purge() const
{
  glDeleteProgram(_programID);
  // error check
  GLenum error_code = glGetError();
  if (error_code) {
    OpenGLError error("Shader.cpp", "Purge");
    error.Add("Encountered while deleting the Shader program.");
    error.Code(error_code);
    throw(error);
  }
}

/*****************************************************************************/
/*!
\brief
  This will compile a single shader. If any errors occur during the compilation
  of the shader, the function will throw an Error.

\param filename
  The path the shader file from the executable directory.
\param type
  The type of shader being compiled. (GL_VERTEX_SHADER, GL_FRAGMENT_SHADER)

\return The ID of the compiled shader.
*/
/*****************************************************************************/
GLuint Shader::CompileShader(const std::string & filename, GLenum type) const
{
  //read from file
  std::string shader_content = ReadShaderFile(filename);
  const GLchar * shader_cstr = shader_content.c_str();
  //create and compile
  GLuint shader = glCreateShader(type);
  glShaderSource(shader, 1, &shader_cstr, nullptr);
  glCompileShader(shader);
  //check for success
  GLint success;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success)
  {
    //print errors
    GLchar errorlog[ERROR_BUFFER_SIZE];
    glGetShaderInfoLog(shader, ERROR_BUFFER_SIZE, nullptr, errorlog);
    Error error("Shader.cpp" ,"CompileShader");
    error.Add("SHADER COMPILE ERROR");
    error.Add(filename.c_str());
    error.Add(errorlog);
    throw(error);
  }
  return shader;
}

/*****************************************************************************/
/*!
\brief
  Reads an entire shader file into a standard string.

\param shader_file
  The path to the shader file that will be read.

\return A standard string containing the contents of the shader file. If the
  file could not be opened, the function will throw an Error.
*/
/*****************************************************************************/
std::string Shader::ReadShaderFile(const std::string & shader_file) const
{
  //opening file
  std::ifstream file(shader_file.c_str());
  if (!file.is_open())
  {
    Error error("Shader.cpp", "ReadShaderFile");
    error.Add("The following Shader file failed to open.");
    error.Add(shader_file.c_str());
    throw(error);
  }
  //copying over data
  std::string content;
  std::string line;
  std::getline(file, line);
  while (!file.eof())
  {
    content.append(line + "\n");
    std::getline(file, line);
  }
  return content;
}

/*****************************************************************************/
/*!
\brief
  Links and creates the shader program given a compiled vertex and fragment 
  shader. If any errors occur during the link step, an exception of type Error
  is thrown. The string contains the linker error generated when the shaders
  were linked.

\param vshader
  The compiled vertex shader ID.
\param fshader
  The compiled fragment shader ID.
*/
/*****************************************************************************/
void Shader::CreateProgram(GLuint vshader, GLuint fshader)
{
  //creating program and linking shaders
  _programID = glCreateProgram();
  glAttachShader(_programID, vshader);
  glAttachShader(_programID, fshader);
  glLinkProgram(_programID);
  //checking for success
  GLint success;
  glGetProgramiv(_programID, GL_LINK_STATUS, &success);
  if (!success)
  {
    //throw error
    GLchar errorlog[ERROR_BUFFER_SIZE];
    glGetProgramInfoLog(_programID, ERROR_BUFFER_SIZE, nullptr, errorlog);
    Error error("Shader.cpp", "CreateProgram");
    error.Add("SHADER LINK ERROR");
    error.Add(errorlog);
    throw(error);
  }
  //deleting shaders
  glDetachShader(_programID, vshader);
  glDetachShader(_programID, fshader);
  glDeleteShader(vshader);
  glDeleteShader(fshader);
}
