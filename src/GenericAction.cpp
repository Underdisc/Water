//////////////////////////////////////////////////////////////////////////////
/// @file GenericAction.cpp
/// @author Connor Deakin
/// @email connor.deakin@digipen.edu
/// @date 2017-07-09
///
/// @brief Contains the implementation of the GenericAction.
///////////////////////////////////////////////////////////////////////////////

#include "GenericAction.h"

// static initializations
std::list<GenericAction *> GenericAction::m_AllActions = 
  std::list<GenericAction *>();

//////////////////////////////////////////////////////////////////////////////
/// @brief Constructor for the GenericAction.
///////////////////////////////////////////////////////////////////////////////
GenericAction::GenericAction() : m_Done(false)
{}

//////////////////////////////////////////////////////////////////////////////
/// @brief Updates All existing Actions.
///////////////////////////////////////////////////////////////////////////////
void GenericAction::UpdateAll()
{
  // needs to be modified iterator invalidation - using list for now
  // Set an Actions to destory list.
  // not many actions at once
  std::list<GenericAction *>::iterator it = m_AllActions.begin();
  std::list<GenericAction *>::iterator it_e = m_AllActions.end();
  // updating all actions
  while (it != it_e) {
    (*it)->Update();
    if ((*it)->m_Done) {
      delete (*it);
      m_AllActions.erase(it);
    }
    ++it;
  }
}

//////////////////////////////////////////////////////////////////////////////
/// @brief Destroys all existing Actions whether they are done or not.
///////////////////////////////////////////////////////////////////////////////
void GenericAction::DestroyAll()
{
  std::list<GenericAction *>::iterator it = m_AllActions.begin();
  std::list<GenericAction *>::iterator it_e = m_AllActions.end();
  // deleting all actions
  while (it != it_e) {
    delete (*it);
    ++it;
  }
  m_AllActions.clear();
}