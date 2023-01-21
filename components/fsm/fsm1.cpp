#include "fsm1.h"


static TransGraph graph[] = {
    {WAIT_FOR_WAKEUP,       CMD_ASR_START,              WAIT_FOR_SPEECH_START}, 
    {WAIT_FOR_WAKEUP,       EVENT_WAKEUP,               WAIT_FOR_SPEECH_START}, 
    {WAIT_FOR_SPEECH_START, EVENT_SPPECH_BEGIN_TIMEOUT, WAIT_FOR_WAKEUP}, 
    {WAIT_FOR_SPEECH_START, EVENT_RESET,                WAIT_FOR_WAKEUP}, 
    {WAIT_FOR_SPEECH_START, EVENT_SPEECH_BEGIN,         SPEECHING},  
    {SPEECHING,             EVENT_WAKEUP,               WAIT_FOR_SPEECH_START},  
    {SPEECHING,             EVENT_SPEECH_END,           WAIT_FOR_WAKEUP},  
    {SPEECHING,             EVENT_SPEECH_END_TIMEOUT,   WAIT_FOR_WAKEUP},  
    {SPEECHING,             EVENT_ASR_ERROR,            WAIT_FOR_WAKEUP},  
    {SPEECHING,             EVENT_RESET,                WAIT_FOR_WAKEUP},  
    {STATUS_END, 0, STATUS_END}
};



State::State()
{
    for (int i = 0; i < EVENT_MAX_SUPPORT; ++i) {
        transition[i] = NULL;
    }
}

Fsm::Fsm()
{
    for (TransGraph *p_tg = graph; p_tg->current_state != STATUS_END; ++p_tg)
        state[p_tg->current_state].transition[p_tg->input_char] = &state[p_tg->next_state];


    for(int i = 0; i < STATUS_END; i++) {
        state[i].current_state = i;
    }

    p_current = NULL;
}

void Fsm::Reset()
{
    p_current = &state[0];
}

void Fsm::Advance(int event)
{
    if (p_current != NULL && p_current->transition[event] != NULL) {
        p_current = p_current->transition[event];
    }
}

int Fsm::CurrentState()
{
    return p_current->current_state;
}
