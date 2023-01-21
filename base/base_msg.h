#ifndef _BASE_MSG_H_
#define _BASE_MSG_H_

#include "base_common.h"





/* ygdc callback data */
typedef struct {
    int     data;
    char    *str;
} base_data_rsp_t;
 

/* cloud request */
/*********************************************************************/
typedef struct {
    int type;
    int data[100];
} base_common_req_t;








typedef void (*prot_rsp_cb_t)(int, const void*, void*);
typedef struct {
    int                        type;

    union {
        base_common_req_t      common_req;
    } msg;

    prot_rsp_cb_t              cb_func;
    const void*                priv;             /* priv will be passed back in cb_func */
} base_request_t;




#endif //_BASE_MSG_H_