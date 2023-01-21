#ifndef _FSM1_H_
#define _FSM1_H_

#include <string.h>

#define EVENT_MAX_SUPPORT       256

typedef enum {
    CMD_ASR_START = 1,
    EVENT_SPEECH_BEGIN = 2,
    EVENT_SPPECH_BEGIN_TIMEOUT = 3,
    EVENT_SPEECH_END = 4,
    EVENT_SPEECH_END_TIMEOUT = 5,
    EVENT_WAKEUP = 6,
    EVENT_ASR_ERROR = 7,
    EVENT_RESET = 8,

} event_type_t;

typedef enum {
    WAIT_FOR_WAKEUP = 0,
    WAIT_FOR_SPEECH_START = 1,
    SPEECHING = 2,
    STATUS_END,

} status_type_t;


class State {
  public:
    State();
    int current_state;
    State *transition[EVENT_MAX_SUPPORT];

  
};

struct TransGraph // use triple to describe map
{
    int current_state;
    int input_char;
    int next_state;
};

class Fsm {
  public:
    Fsm();
    void Reset();         // move to start state
    void Advance(int event); // advance one transition
    int CurrentState();

  private:
    State *p_current; // &s1, &s2, ..., &s6; NULL ==> doom
    State state[STATUS_END];   // 2 states, state[0] is end state
};

#endif //_FSM1_H_