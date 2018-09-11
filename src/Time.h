/* All content(c) 2016 - 2017 DigiPen(USA) Corporation, all rights reserved. */
/*****************************************************************************/
/*!
\file Time.h
\author Connor Deakin
\par E-mail: connor.deakin\@digipen.edu
\par Project: X-Craft
\date 13/01/2017
\brief
  Contains the interface for the static Time class. A class used to track time
  values that are associated with the running program.
*/
/*****************************************************************************/
#ifndef TIME_H
#define TIME_H

#include <vector>

/*****************************************************************************/
/*!
\class Time
\brief
  This static class is responsible for managing the values associated with frame
  times and program ticks. It will store the amount of time passed since it was
  previously updated and other important information related to program time.

\par Important Notes
  - Update this class at the very start of each frame so all other functionality
    uses the proper time values.
*/
/*****************************************************************************/
class Time
{
public:
  static void Update();
  static float DT();
  static float DTScaled();
  static float TotalTime();
  static float TotalTimeScaled();
  static float TotalTimeExact();
  //! The speed factor by which time is experienced. For example, a time scale
  // of 0.5f means m_TotalTimeScaled will increase by the half the rate that
  // m_TotalTime will increase by.
  static float m_TimeScale;
public:
  /***************************************************************************/
  /*!
  \class Stopwatch
  \brief
    Stopwatches are time instances that keep track of their own total time.
    They can be used to pause time and the total time passed for the Stopwatch
    will not change. They can also be used to speed up, slowdown, or reverse
    time using the timeScale value that is applied to the delta time of a
    single frame.

  \par Important Notes
    - Once you create a Stopwatch, you do not need to update it. The time
      class will take care of that assuming that the stopwatch you are using
      lasts longer than a single frame.
  */
  /***************************************************************************/
  class Stopwatch
  {
  public:
    Stopwatch(bool start);
    float DeltaTime();
    float TotalTime();
    //! Identifies whether the Stopwatch is currently paused.
    bool m_Paused;
    //! The value that normal delta time is scaled by to track the Stopwatch's
    // total time.
    float m_TimeScale;
  private:
    void Update();
    //! The time that passed for this stopwatch during the previous frame.
    float m_DeltaTime;
    //! The total time that the Stopwatch has existed for.
    float m_TotalTime;
    //! Time will update all instances of Stopwatch.
    friend Time;
  };
private:
  Time() {}
  //! The difference of time (in seconds) from the Time classes Update before
  // the most recent Update to the most recent Update.
  //\par Diagram
  //  - (Previous Update) ---------> (Most Recent Update)
  //  - The arrow represents _deltaTime in seconds.
  static float m_DeltaTime;
  //! The delta time for a frame scaled by m_TimeScale.
  static float m_DeltaTimeScaled;
  //! The total time that has passed (in seconds) since the program was
  // launched.
  static float m_TotalTime;
  //! The total apparent time (m_DeltaTimeScaled is added during each frame),
  // passed during the program lifefime.
  static float m_TotalTimeScaled;
  //! The same as _deltaTime, but this is the number of Ticks passed rather
  // than the number of seconds
  static int m_DeltaTicks;
  //! The total number of ticks the program has experienced since the program
  // started running.
  static int m_Ticks;
  //! The total number of ticks that had been experienced until the previous
  // update. The differnce between _ticks and _ticksPrev is the value of
  // _deltaTicks.
  static int m_TicksPrev;
  //! The stopwatches that are currently being used
  static std::vector<Stopwatch *> m_Stopwatches;
};

#endif // TIME_H
