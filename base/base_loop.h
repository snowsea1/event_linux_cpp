#ifndef _BASE_LOOP_H_
#define _BASE_LOOP_H_
 
#include "base_timer.h"
#include "base_msg.h"
#include "base_common.h"
#include "base_queue.h"
#include "config.h"
#include "fsm.h"
#include "fsm1.h"


 
#define ARCH_OS_WAIT_FOREVER            (0xFFFFFFFF)
#define ARCH_OS_NO_WAIT                 IPC_NOWAIT
 


#define Q_NAME                  "/base_q"
#define Q_MSG_TYPE              10

#define BASE_MAX_REQUSTS        10
#define BASE_POOL_ITEMS         (BASE_MAX_REQUSTS)


typedef int (*callback_t)(base_common_req_t* data);
 
/**
 *  memory pool data
 */
typedef struct {
    int occupied;       // memory is occupied
    unsigned char buffer[sizeof(base_request_t)];

} base_buffer_t;

/**
 *  base queue
 */
typedef struct {
    int  req_queue;
} base_cb_t;


/* queue message */
typedef struct q_msg {
    long mtype;
    unsigned char mdata[sizeof(base_request_t *)];
}q_msg_t;
 

typedef struct {
    list_head_t header;
    int event;
    callback_t fn;
    void *self;

} base_event_cb_t;


class Base_loop {

  public:
    Base_loop();
    ~Base_loop();

    /* inti base timer */
    Base_timer base_timer;
    base_tmr_hdl_t base_tmr_hdl;

    int base_loop_init(); /* 初始化 base事件 */
    int base_loop_callback(int module_id, callback_t handler); /* 回调注册； event:  */
    int base_post_request_async(int module_id, const void* req, unsigned int size, prot_rsp_cb_t routine, const void* priv);

    void base_start_tmr(base_tmr_hdl_t hdl, base_tmr_t *tmr, timer_func_t func, base_ms_t to);
    void base_start_tmr_with_data(base_tmr_hdl_t hdl, base_tmr_t *tmr, timer_func_t func, base_ms_t to, void *d);


  private:


    base_buffer_t base_buffer_pool[BASE_POOL_ITEMS];
    base_cb_t base_cb;

    ThreadQueue<void *> messageQueue;
    map<int, void *> event_callbak;

    int base_queue_receive(base_buffer_t **base_buffer, unsigned long long wait);
    int base_queue_send(base_buffer_t **base_buffer);
    int base_queue_create(int *pqhandle, int q_num, int item_size);
    void base_buffer_pool_release(base_buffer_t *base_buffer);
    base_buffer_t *base_buffer_pool_alloc(void);
    void base_buffer_pool_init(void);
    void base_task_timer_update(void);
    
    int base_proc_request(base_request_t* req, void* rsp);

    static void *base_req_handler(void* arg);

};
 


#endif  //_BASE_LOOP_H_