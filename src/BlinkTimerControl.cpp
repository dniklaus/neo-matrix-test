/*
 * BlinkTimerControl.cpp
 *
 *  Created on: 27.04.2014
 *      Author: niklausd
 */

#include "Arduino.h"
#include <limits.h>

#include "Timer.h"
#include "BlinkTimerControl.h"

BlinkTimerControl::BlinkTimerControl(TimerAdapter* timerAdapter, unsigned int blinkTimeMillis)
: m_timer(new Timer(timerAdapter, Timer::IS_RECURRING))
, m_currentBlinkTimeMillis(blinkTimeMillis)
, m_newBlinkTimeMillis(m_currentBlinkTimeMillis)
{
  if (0 != m_timer)
  {
    m_timer->startTimer(m_currentBlinkTimeMillis);
  }
}

BlinkTimerControl::~BlinkTimerControl()
{
  delete m_timer; m_timer = 0;
}

void BlinkTimerControl::incrementBlinkTime()
{
  if (m_newBlinkTimeMillis < ULONG_MAX)
  {
    m_newBlinkTimeMillis++;
  }
  if (!isTimerRunning())
  {
    startTimer();
  }
}

void BlinkTimerControl::decrementBlinkTime()
{
  if (m_newBlinkTimeMillis > 0)
  {
    m_newBlinkTimeMillis--;
  }
  if (!isTimerRunning())
  {
    startTimer();
  }
}

void BlinkTimerControl::timeExpired()
{
  if (m_newBlinkTimeMillis != m_currentBlinkTimeMillis)
  {
    startTimer();
  }
}

void BlinkTimerControl::startTimer()
{
  if (0 != m_timer)
  {
    // (re-)start the timer with new interval value
    m_timer->startTimer(m_newBlinkTimeMillis);
  }
  m_currentBlinkTimeMillis = m_newBlinkTimeMillis;
  Serial.print("New timer interval [ms]: ");
  Serial.println(m_currentBlinkTimeMillis);
}

bool BlinkTimerControl::isTimerRunning()
{
  bool isRunning = false;
  if (0 != m_timer)
  {
    isRunning = m_timer->isRunning();
  }
  return isRunning;
}

