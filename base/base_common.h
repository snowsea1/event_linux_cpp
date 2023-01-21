#ifndef _BASE_COMMON_H_
#define _BASE_COMMON_H_
 
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <error.h>
#include <errno.h>
#include <time.h>
#include <signal.h>
#include <iostream>
#include <map>
#include <fstream>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
// #include <mqueue.h>


using namespace std;

#define RET_OK          0 
#define RET_ERR         -1

#define BASE_DFLT_IMPL  __attribute__((weak))



#define LOGE            printf






 
#endif //_BASE_COMMON_H_