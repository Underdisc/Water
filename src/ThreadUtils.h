#pragma once
#include <mutex>
#include <condition_variable>
#include <functional>

class Barrier
{
public:
  Barrier()
  {}
  Barrier(int total_threads) :
    m_KnockedDown(false), m_ThreadsLeft(total_threads), m_TotalThreads(total_threads)
  {}
  void WaitForAllThreads(std::function<void()> single_callback, 
    bool knock_down)
  {
    if(knock_down)
    {
      m_KnockedDown = true;
      m_CV.notify_all();
    }
    if(!m_KnockedDown)
    {
      std::unique_lock<std::mutex> lock(m_Mutex);
      --m_ThreadsLeft;
      if (m_ThreadsLeft == 0)
      {
        single_callback();
        m_ThreadsLeft = m_TotalThreads;
        m_CV.notify_all();
      }
      else
      {
        m_CV.wait(lock);
      }
    }
  }
  std::mutex m_Mutex;
  std::condition_variable m_CV;
  bool m_KnockedDown;
  int m_ThreadsLeft;
  int m_TotalThreads;
};