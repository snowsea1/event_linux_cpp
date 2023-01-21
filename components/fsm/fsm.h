#ifndef _FSM_H_
#define _FSM_H_

#include "base_common.h"


#if 0
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
    WAIT_FOR_WAKEUP = 1,
    WAIT_FOR_SPEECH_START = 2,
    SPEECHING = 3,

} status_type_t;




class Fsm
{
public:
    Fsm();
    ~Fsm();

    int init();
    int send_event(int event);
    int get_status();

private:
    int current_status;
    int notice_status();

};


#endif


#endif // _FSM_H_