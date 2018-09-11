/* All content(c) 2016 - 2017 DigiPen(USA) Corporation, all rights reserved. */
/*****************************************************************************/
/*!
\file Error.h
\author Connor Deakin
\par E-mail: connor.deakin\@digipen.edu
\date 30/09/2016
\brief
  Contains the interface for error management. The feature included here are
  the following.
  - Error exception class
  - RootError exception class
  - ErrorLog for writing Error exceptions
  At the top of Error.cpp, you will find parameters that can be modified for
  a specific project.
*/
/*****************************************************************************/

#ifndef ERROR_H
#define ERROR_H

#include <string>
#include <vector>
#include <fstream>

/*****************************************************************************/
/*!
\class Error
\brief
  The Error class is to be used for exception based error management. When
  creating an error class, you must supply the name of the file where the error
  occured and which function it occured in in order to print the error
  information when the Error is caught.
*/
/*****************************************************************************/
class Error
{
public:
  Error(const char * file, const char * function);
  void Add(const char * info);
  void Add(const std::string & info);
  friend std::ostream & operator<<(std::ostream & os, const Error & error);
private:
  //! The name of the file in which an Error was thrown.
  std::string _file;
  //! The name of the function in which an Error was thrown.
  std::string _function;
  //! Contains a detailed description of what error occured.
  std::vector<std::string> _log;
};

/*****************************************************************************/
/*!
\class RootError
\brief
  The RootError exception class is intended for errors that should terminate
  the program. A root exception should never be caught inside of any core
  code. It should only be caught in main.
*/
/*****************************************************************************/
class RootError : public Error
{
public:
  RootError(const char * file, const char * function);
};

/*****************************************************************************/
/*!
\class ErrorLog
\brief
  This will write errors to the files specified at the top of Error.cpp. Error
  instances will be written to ROOTERROR_LOG_FILENAME or ERROR_LOG_FILENAME
  depending on their type (Error/RootError).

\par Important Notes
  - No writing will occur if the files fail to open.
*/
/*****************************************************************************/
class ErrorLog
{
public:
  static void Clean();
  static void Write(const Error & error);
  static void Write(const RootError & root_error);
private:
  ErrorLog() {}
  //! Tracks whether an error has been written to file yet or not.
  static bool _errorWritten;
  //! Tracks whether a root error has been written to file yet or not.
  static bool _rootErrorWritten;
  //! The file that Error instances will be written to.
  static std::string _errorFilename;
  //! The file that RootError instances will be written to.
  static std::string _rootErrorFilename;
  //! Stream used for file writing.
  static std::ofstream _errorLog;
};


#endif // !ERROR_H