#ifndef _AUDIO_MANAGE_H_
#define _AUDIO_MANAGE_H_



class Audio_manage {

  public:
   Audio_manage();
   ~Audio_manage();


    void audio_init();
    void audio_input(void *data, int len);

  private:


};








#endif //_AUDIO_MANAGE_H_