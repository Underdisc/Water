/*****************************************************************************/
/*!
\file Time.cpp
\author Zach Dolph / Connor Deakin
\par E-mail: connor.deakin\@digipen.edu
\par Project: X-Craft
\date 13/01/2017
\brief
  Contains the implementation of the Time class. Look in the class header fie
  for more information.
*/
/*****************************************************************************/
//TODO: UPDATE TO GET RID OF SDLTIMER. USE STD::CHRONO
#include <SDL\SDL_timer.h>

#include "Time.h"

// static initializations
float Time::m_TimeScale = 1.0f;
float Time::m_DeltaTime = 0.0f;
float Time::m_DeltaTimeScaled = 0.0f;
float Time::m_TotalTime = 0.0f;
float Time::m_TotalTimeScaled = 0.0f;
int Time::m_DeltaTicks = 0;
int Time::m_Ticks = 0;
int Time::m_TicksPrev = 0;
std::vector<Time::Stopwatch *> Time::m_Stopwatches;

/*!
\brief Updates all of the values within the Time class. This should be called
  at the start of every frame so all other functionality is based off of the 
  correct times.
*/
void Time::Update()
{
  m_Ticks = SDL_GetTicks();
  m_DeltaTicks = m_Ticks - m_TicksPrev;
  m_DeltaTime = (float)m_DeltaTicks / (float)1000;
  m_DeltaTimeScaled = m_DeltaTime * m_TimeScale;
  m_TotalTime += m_DeltaTime;
  m_TotalTimeScaled += m_DeltaTimeScaled;
  m_TicksPrev = m_Ticks;
  for (Stopwatch * stopwatch : m_Stopwatches) {
    stopwatch->Update();
  }
}

/*!
\brief Gets the current value of m_DeltaTime.
\return The value of m_DeltaTime.
*/
float Time::DT()
{
  return m_DeltaTime;
}

/*!
\brief Returns the scaled delta time (in seconds) from the previous frame.
\return The current value of m_DeltaTimeScaled.
*/
float Time::DTScaled()
{
  return m_DeltaTimeScaled;
}

/*!
\brief Returns total time (in seconds) that has passed since program start.
\return The current value of m_TotalTime.
*/
float Time::TotalTime()
{
  return m_TotalTime;
}

/*!
\brief Returns the total scaled time (in seconds) that has passed.
\return The current value of m_TotalTimeScaled.
*/
float Time::TotalTimeScaled()
{
  return m_TotalTimeScaled;
}

/*!
\brief Finds the exact amount of time passed upon being called.
\return The exact amount of time passed.
*/
float Time::TotalTimeExact()
{
  int total_ticks = SDL_GetTicks();
  float total_time = (float)total_ticks / 1000.0f;
  return total_time;
}

/*!
\brief Stopwatch constructor. Stopwatches start with a timescale of 1.0.
\param start Determine whether the stopwatch should instantly start.
*/
Time::Stopwatch::Stopwatch(bool start) :
  m_Paused(!start), m_TimeScale(1.0f), 
  m_DeltaTime(0.0f), m_TotalTime(0.0f)
{
  Time::m_Stopwatches.push_back(this);
}

/*!
\brief Returns the time that has passed for this stopwatch.
\return The delta time for this stopwatch.
*/
float Time::Stopwatch::DeltaTime()
{
  return m_DeltaTime;
}

/*!
\brief The total time that has passed for this Stopwatch.
\return The Stopwatche's total time. 
*/
float Time::Stopwatch::TotalTime()
{
  return m_TotalTime;
}

/*!
\brief Updates the Stopwatche's time according to the current timescale if
  it is not currently paused.
*/
void Time::Stopwatch::Update()
{
  if (!m_Paused) {
    m_DeltaTime = m_DeltaTime * m_TimeScale;
    m_TotalTime += m_DeltaTime;
  }
}

