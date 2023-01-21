#include "fsm.h"
#if 0

int Fsm::send_event(int event)
{
    switch(current_status) {

    case WAIT_FOR_WAKEUP:
        if(event == CMD_ASR_START || event == EVENT_WAKEUP) {
            current_status = WAIT_FOR_SPEECH_START;
        }

        break;

    case WAIT_FOR_SPEECH_START:
        if(event == EVENT_SPEECH_END_TIMEOUT || event == EVENT_RESET) {
            current_status = WAIT_FOR_WAKEUP;
        }
        if(event == EVENT_SPEECH_BEGIN) {
            current_status = SPEECHING;
        }

        break;

    case SPEECHING:
        if(event == EVENT_WAKEUP) {
            current_status = WAIT_FOR_SPEECH_START;
        }
        if(event == EVENT_SPEECH_END || event == EVENT_SPEECH_END_TIMEOUT || event == EVENT_ASR_ERROR \
            || event == EVENT_RESET) {
                current_status = WAIT_FOR_WAKEUP;
        }
        break;

    }
    return 0;
}

int Fsm::get_status()
{
    return current_status;
}

int Fsm::notice_status()
{
    return 0;
}

int Fsm::init()
{
    current_status = CMD_ASR_START;

    return 0;
}

Fsm::Fsm() {}
Fsm::~Fsm() {}


#endif