//////////////////////////////////////////////////////////////////////////////
/// @file Action.hh
/// @author Connor Deakin
/// @email connor.deakin@digipen.edu
/// @date 2017-07-09
///
/// @brief Contains the interface and implementation of the Action class.
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include "GenericAction.h"

//! The types ease types that can be used for Actions. More can be added
// upon request.
enum ACTIONTYPE
{
  LINEAR, QUADIN, QUADOUT, QUADOUTIN,
  NUMTYPES
};

//////////////////////////////////////////////////////////////////////////////
/// @brief Actions allow for value fading over time. Given a the value that
/// will be changed, its starting value, ending value, the amount of time it
/// must change over, and an ease type, the value will change over the given
/// time frame with the appropriate ease type.
///
/// @par Important Notes
/// - The type of the value must have the + and * operations defined.
/// - Actions can only be created with the Action<T>::Create function.
///////////////////////////////////////////////////////////////////////////////
/// Wants: strenght parameter for giving the action different highs and lows.
template <typename T>
class Action : public GenericAction
{
public:
  static void Create(T & value, T start, T end, float time, ACTIONTYPE type);
private:
  Action(T & value, T & start, T & end, float time, ACTIONTYPE type);
  virtual void Update();
  void Linear(float time_passed);
  void QuadOut(float time_passed);
  void QuadIn(float time_passed);
  void QuadOutIn(float time_passed);
  //! The value that will change over time.
  T & m_Value;
  //! The starting position of the value.
  T m_Start;
  //! The middle position of the value.
  T m_Mid;
  //! The ending position of the value.
  T m_End;
  //! The distance that is being traveled.
  T m_Travel;
  //! Half of the distance that is being traveled.
  T m_HalfTravel;
  //! The time at which the Action began.
  float m_StartTime;
  //! The amount of time the Action must take.
  float m_Time;
  //! Half of the amount of time the Action must take.
  float m_HalfTime;
  //! The ease type of the Action.
  ACTIONTYPE m_Type;
};

#include "Time.h"

//////////////////////////////////////////////////////////////////////////////
/// @brief Creates an Action that will fade a value over time.
///
/// @tparam T The type of the value that the Action is being created for.
/// @param value The value that will fade over time.
/// @param start Where the value will begin.
/// @param end Where the value will end.
/// @param time The amount of time the action will take.
/// @param type The ease type that the Action will perform on the value.
///////////////////////////////////////////////////////////////////////////////
template<typename T>
inline void Action<T>::Create(T & value, T start, T end, float time, ACTIONTYPE type)
{
  Action<T> * new_action = new Action<T>(value, start, end, time, type);
  m_AllActions.push_back(new_action);
}


//////////////////////////////////////////////////////////////////////////////
/// @brief Instantiates an Action that will fade a value over time.
///
/// @tparam T The type of the value that the Action is being created for.
/// @param value The value that will fade over time.
/// @param start Where the value will begin.
/// @param end Where the value will end.
/// @param time The amount of time the action will take.
/// @param type The ease type that the Action will perform on the value.
///////////////////////////////////////////////////////////////////////////////
template<typename T>
Action<T>::Action(T & value, T & start, T & end, float time, ACTIONTYPE type) :
  m_Value(value), m_Start(start), m_End(end), m_Travel(end - start),
  m_StartTime(Time::TotalTime()), m_Time(time), m_Type(type)
{
  if (m_Type == QUADOUTIN) {
    m_HalfTravel = m_Travel * 0.5f;
    m_HalfTime = m_Time * 0.5f;
    m_Mid = m_Start + m_HalfTravel;
  }
}

//////////////////////////////////////////////////////////////////////////////
/// @brief Updates the value to its new value according to the Action ease
/// type
///
/// @tparam T The type of the value that the Action is acting on.
///////////////////////////////////////////////////////////////////////////////
template<typename T>
void Action<T>::Update()
{
  float time_passed = Time::TotalTime() - m_StartTime;
  // ending action
  if (time_passed >= m_Time) {
    m_Value = m_End;
    m_Done = true;
    return;
  }
  // middle of action - finding ease type
  switch (m_Type)
  {
  case LINEAR: Linear(time_passed);  break;
  case QUADIN: QuadIn(time_passed); break;
  case QUADOUT: QuadOut(time_passed); break;
  case QUADOUTIN: QuadOutIn(time_passed); break;
  }
}

//////////////////////////////////////////////////////////////////////////////
/// @brief Updates the Action value with a linear ease.
///
/// @tparam T The type of the value that the Action is acting on.
/// @param time_passed The amount of time that has passed since creation.
///////////////////////////////////////////////////////////////////////////////
template<typename T>
inline void Action<T>::Linear(float time_passed)
{
  float percentage = (Time::TotalTime() - m_StartTime) / m_Time;
  m_Value = m_Start + percentage * m_Travel;
}

//////////////////////////////////////////////////////////////////////////////
/// @brief Updates the Action value with a QuadOut (x * x) ease.
///
/// @tparam T The type of the value that the Action is acting on.
/// @param time_passed The amount of time that has passed since creation.
///////////////////////////////////////////////////////////////////////////////
template<typename T>
inline void Action<T>::QuadOut(float time_passed)
{
  float percentage = time_passed / m_Time;
  m_Value = m_Start + (percentage * percentage) * m_Travel;
}

//////////////////////////////////////////////////////////////////////////////
/// @brief Updates the Action value with a QuadIn ease. A traditional
/// quadratic that is first flipped, then pushed forward 1 on both the
/// x and y axis.
/// (-1 *(x - 1)^2) + 1 
///
/// @tparam T The type of the value that the Action is acting on.
/// @param time_passed The amount of time that has passed since creation.
///////////////////////////////////////////////////////////////////////////////
template<typename T>
inline void Action<T>::QuadIn(float time_passed)
{
  float percentage = time_passed / m_Time;
  float scaler = ((-percentage + 1.0f) * (percentage - 1.0f)) + 1.0f;
  m_Value = m_Start + scaler * m_Travel;
}

//////////////////////////////////////////////////////////////////////////////
/// @brief Updates the Action value with a QuadOutIn ease. This ease will
/// begin as QuadOut, but halfway through, it will switch to QuadIn.
///
/// @tparam T The type of the value that the Action is acting on.
/// @param time_passed The amount of time that has passed since creation.
///////////////////////////////////////////////////////////////////////////////
template<typename T>
inline void Action<T>::QuadOutIn(float time_passed)
{
  // QuadOut ease
  if (time_passed < m_HalfTime) {
    float perc = time_passed / m_HalfTime;
    float scaler = perc * perc;
    m_Value = m_Start + scaler * m_HalfTravel;
  }
  // QuadIn ease
  else{
    float perc = (time_passed - m_HalfTime) / m_HalfTime;
    float scaler = ((-perc + 1.0f) * (perc - 1.0f)) + 1.0f;
    m_Value = m_Mid + scaler * m_HalfTravel;
  }
}