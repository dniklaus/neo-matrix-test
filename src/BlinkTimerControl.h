/*
 * BlinkTimerControl.h
 *
 *  Created on: 27.04.2014
 *      Author: niklausd
 */

#ifndef BLINKTIMERCONTROL_H_
#define BLINKTIMERCONTROL_H_

class BlinkTimerControl
{
public:
  BlinkTimerControl(TimerAdapter* timerAdapter, unsigned int blinkTimeMillis);
  virtual ~BlinkTimerControl();

  void incrementBlinkTime();
  void decrementBlinkTime();
  void timeExpired();

private:
  void startTimer();
  bool isTimerRunning();

private:
  Timer* m_timer;
  unsigned int m_currentBlinkTimeMillis;
  unsigned int m_newBlinkTimeMillis;

private: // forbidden functions
  BlinkTimerControl& operator = (const BlinkTimerControl& src); // assignment operator
  BlinkTimerControl(const BlinkTimerControl& source);           // copy constructor
};

#endif /* BLINKTIMERCONTROL_H_ */
