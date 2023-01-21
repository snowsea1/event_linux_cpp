#ifndef _VOICE_SERVICE_H_
#define _VOICE_SERVICE_H_

#include "base_loop.h"
#include "base_common.h"
#include "config.h"
#include "audio_manage.h"


class Voice_service {

  public:
    Voice_service();
    ~Voice_service();

    Base_loop base_loop;

    /* init config */
    Config config;

    /* init Fsm */
    Fsm fsm;

    /* audio manage */
    Audio_manage audio_manage;

    void voice_service_init();

  private:

};
















#endif //_VOICE_SERVICE_H_