//////////////////////////////////////////////////////////////////////////////
/// @file GenericAction.h
/// @author Connor Deakin
/// @email connor.deakin@digipen.edu
/// @date 2017-07-09
///
/// @brief Contains the interface for a GenericAction. All Actions will
/// identify as a GenericAction so all Actions can be acted on with a single
/// call no matter what type the Action operates on.
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include <list>

// pre-declaration
template<typename T>
class Action;

//////////////////////////////////////////////////////////////////////////////
/// @brief All Actions will identify as a GenericAction so all Actions can 
/// be acted on with a single call no matter what type the Action operates on.
///
/// @par Important Notes
/// - Call UpdateAll() once per frame in order to Update all existing Actions.
/// - Call DestroyAll() to destroy all existing actions.
///////////////////////////////////////////////////////////////////////////////
class GenericAction
{
public:
  static void UpdateAll();
  static void DestroyAll();
private:
  GenericAction();
  virtual void Update() = 0;
  //! Determines whether the Action is done or not.
  bool m_Done;
  //! All of the Actions that have been created by the 
  // Action<T>::Create function.
  static std::list<GenericAction *> m_AllActions;
  //! Friendng the Action calss.
  template<typename T>
  friend class Action;
};