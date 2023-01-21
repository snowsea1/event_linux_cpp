#include "voice_service.h"


void Voice_service::voice_service_init()
{

    /* Init config */
    config.config_init();

    base_loop.base_loop_init();

    // fsm.init();

    audio_manage.audio_init();
}

Voice_service::Voice_service()
{

}


Voice_service::~Voice_service()
{

}