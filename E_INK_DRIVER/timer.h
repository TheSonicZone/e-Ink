//                  Header file for Timer Library
//-------------------------------------------------------
#ifndef TIMER_H_
#define TIMER_H_

// Definitions
//--------------------

unsigned int TIME_MEASURE;

// Function Prototypes
//--------------------
void Delay(unsigned int delay);
void InitTimerSystem(void);
void CallInISR(void);



#endif /* TIMER_H_ */
