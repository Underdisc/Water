/* All content(c) 2017 - 2018 DigiPen(USA) Corporation, all rights reserved. */
/*****************************************************************************/
/*!
\file Framer.cpp
\author Connor Deakin
\par E-mail: connor.deakin\@digipen.edu
\par Project: Graphics
\date 2017/10/23
\brief
  Implementation of the Framer utility class.
*/
/*****************************************************************************/
#include <chrono>
#include <thread>

#include "Time.h"
#include "Framer.h"

// macros
#define MILLISINS 1000.0f // milliseconds in a second

// static initialization
bool Framer::_locked = false;
float Framer::_targetFrameTime = 0.0f;
float Framer::_startTime = 0.0f;
float Framer::_timeSinceFPSCalculation = 0.0f;
float Framer::_timeSinceFrameUsageCalculation = 0.0f;
float Framer::_waitTimeFPSCalculation = 1.0f;
float Framer::_waitTimeFrameUsageCalculation = 0.2f;
float Framer::_averageFPS = 0.0f;
float Framer::_averageFrameUsage = 0.0f;
std::vector<float> Framer::_frameTimes;
std::vector<float> Framer::_frameUsages;

/*****************************************************************************/
/*!
\brief
  Call this at the start of a frame to save the current time in the Framer.
*/
/*****************************************************************************/
void Framer::Start()
{
  _startTime = Time::TotalTimeExact();
}

/*****************************************************************************/
/*!
\brief
  Call this at the end of a frame to lock the frame if locking is enabled and
  save time values associated with the frame.
*/
/*****************************************************************************/
void Framer::End()
{
  float end_time = Time::TotalTimeExact();
  float time_passed = end_time - _startTime;
  //saving frame usage
  if (_locked)
    _frameUsages.push_back(time_passed / _targetFrameTime);
  else
    // 100% of the frame is used in unlocked mode
    _frameUsages.push_back(1.0f);
  // block thread to hit target fps
  if(_locked && time_passed < _targetFrameTime){
    // time to wait in seconds 
    float s_ttw = _targetFrameTime - time_passed;
    float ms_ttw = s_ttw * MILLISINS;
    // waiting
    std::this_thread::sleep_for(std::chrono::milliseconds((int)ms_ttw));
    //updating time passed
    time_passed += s_ttw;
  }
  // save time pased
  _frameTimes.push_back(time_passed);
  _timeSinceFPSCalculation += time_passed;
  _timeSinceFrameUsageCalculation += time_passed;
  // calculate average fps if necessary
  if(_timeSinceFPSCalculation >= _waitTimeFPSCalculation)
    CalculateAverageFPS();
  // calculate average frame usage if necessary
  if (_timeSinceFrameUsageCalculation >= _waitTimeFrameUsageCalculation)
    CalculateAverageFrameUsage();
}

/*****************************************************************************/
/*!
\brief
  Unlocks the FPS. Stopes the Framer from locking at the end of a frame.
*/
/*****************************************************************************/
void Framer::Unlock()
{
  _locked = false;
}

/*****************************************************************************/
/*!
\brief
  Enables frame locking. Once called, the Framer will lock frame rate at 
  the desired fps at the end of each frame.

\param fps
  The fps the Framer will try to maintain.
*/
/*****************************************************************************/
void Framer::Lock(int fps)
{
  _locked = true;
  _targetFrameTime = 1.0f / (float)fps;
}

/*****************************************************************************/
/*!
\brief
  Returns the average FPS over the last duration specified by 
  _waitTimeForAverageCalculation.

\return The average FPS.
*/
/*****************************************************************************/
float Framer::AverageFPS()
{
  return _averageFPS;
}

/*****************************************************************************/
/*!
\brief
  Returns the average frame usage over the last duration specified by
  _waitTimeForAverageCalculation. The frame usage is a value in the range
  [0, 1]. It describes the percentage of the frame time used in relation to
  the target frame time (frame time used / target frame time).

\return The average frame usage.
*/
/*****************************************************************************/
float Framer::AverageFrameUsage()
{
  return _averageFrameUsage;
}

/*****************************************************************************/
/*!
\brief
  Calculates the average fps with the current values stored in _frameTimes.
*/
/*****************************************************************************/
void Framer::CalculateAverageFPS()
{
  // finding average fps
  float total = 0.0f;
  for(const float & frame_time : _frameTimes)
    total += frame_time;
  float average_frame_time = total / (float)_frameTimes.size();
  _averageFPS = 1.0f / average_frame_time;
  // resetting values
  _frameTimes.clear();
  _timeSinceFPSCalculation = 0.0f;
}

/*****************************************************************************/
/*!
\brief
  Calculates the average frame usage with the current values stored in 
  _frameUsages.
*/
/*****************************************************************************/
void Framer::CalculateAverageFrameUsage()
{
  // finding average frame usage
  float total = 0.0f;
  for (const float & frame_usage_percentage : _frameUsages)
    total += frame_usage_percentage;
  _averageFrameUsage = total / _frameUsages.size();
  // resetting values
  _frameUsages.clear();
  _timeSinceFrameUsageCalculation = 0.0f;
}
