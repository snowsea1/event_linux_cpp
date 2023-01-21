#include "base_loop.h"


using namespace std;

/*
 *  timer trigger base_req_handler
 */
void Base_loop::base_task_timer_update(void)
{
    base_post_request_async(999, NULL, 0, NULL, NULL);
}

void Base_loop::base_start_tmr(base_tmr_hdl_t hdl, base_tmr_t *tmr, timer_func_t func, base_ms_t to)
{
    if (base_timer.base_timer_is_running(tmr)) {
        base_timer.base_del_timer(tmr);
    }
    (tmr)->val = to;
    (tmr)->timer_id = 0;
    (tmr)->data = NULL;
    (tmr)->fn = func;
    (tmr)->repeat = 0;
    base_timer.base_add_timer(hdl, (tmr));

    base_task_timer_update();
}


void Base_loop::base_start_tmr_with_data(base_tmr_hdl_t hdl, base_tmr_t *tmr, timer_func_t func, base_ms_t to, void *d)
{
    if (base_timer.base_timer_is_running(tmr)) {
        base_timer.base_del_timer(tmr);
    }
    (tmr)->val = to;
    (tmr)->timer_id = 0;
    (tmr)->data = d;
    (tmr)->fn = func;
    (tmr)->repeat = 0;
    base_timer.base_add_timer(hdl, (tmr));

    base_task_timer_update();
}


/*********************************************************************************
 * memory pool
*********************************************************************************/
void Base_loop::base_buffer_pool_init(void)
{
    memset(base_buffer_pool, 0, sizeof(base_buffer_pool));
}

base_buffer_t *Base_loop::base_buffer_pool_alloc(void)
{
    for (unsigned int index = 0; index < BASE_POOL_ITEMS; index++) {
        if (!base_buffer_pool[index].occupied) {
            base_buffer_pool[index].occupied = true;
            return &base_buffer_pool[index];
        }
    }
    return NULL;
}

void Base_loop::base_buffer_pool_release(base_buffer_t *base_buffer)
{
    base_buffer->occupied = false;
}

int Base_loop::base_queue_create(int *pqhandle, int q_num, int item_size)
{
    return 0;

}


int Base_loop::base_queue_send(base_buffer_t **base_buffer)
{
    messageQueue.push(*base_buffer);

    return 0;
}

/**
* func: receive data from queue
*/
int Base_loop::base_queue_receive(base_buffer_t **base_buffer, unsigned long long wait)
{
    void *buf = NULL;

    messageQueue.pop(buf, wait);

    *base_buffer = (base_buffer_t *)buf;


    return (*base_buffer != NULL) ? 0 : -1;
}


int Base_loop::base_proc_request(base_request_t* req, void* rsp)
{
    // printf("\nExecute BASE request: %d\n\n", req->type);

    callback_t event_handler = (callback_t )(event_callbak[req->type]);
    if(event_handler != NULL) {
        event_handler( (base_common_req_t *)&(req->msg.common_req));
    }
    else {
        // printf("lyh add -> event_handler is NULL\n");
    }

    return 0;
}


int Base_loop::base_loop_callback(int module_id, callback_t handler)
{
    event_callbak[module_id] = (void *)(handler);

    return 0;
}

/**
 * @func:  post msg to base queue
 * @parm: 
 * @return {*}
 */ 
int Base_loop::base_post_request_async(int module_id, const void* req, unsigned int size, prot_rsp_cb_t routine, const void* priv)
{
    // printf("enter post_request_async\n");

    /* malloc memory from pool */
    base_buffer_t *base_buffer = base_buffer_pool_alloc();
    if (NULL == base_buffer) {
        printf("No base buffer, req:%d is dropped!\r\n", module_id);
        return -1;
    }
    base_request_t* base_req = (base_request_t *)(base_buffer->buffer);

    /* data input */
    base_req->type = module_id;
    if (req) {
        memcpy(&(base_req->msg), req, size);
    }
    base_req->cb_func = routine;
    base_req->priv = priv;

    /* join queue */
    // printf("lyh add -> base_buffer = %p\n", base_buffer);
    return base_queue_send(&base_buffer);
}


/**
 * func: pthread, trans
 * parm:
 * return:
 */
void *Base_loop::base_req_handler(void *arg)
{
    int rc = 0;
    int err = 0;
    base_request_t* req = NULL;
    base_ms_t val = 0;
    unsigned long long wait = 0;

    union {
        base_data_rsp_t   cfg_rsp;
    } rsp;

    Base_loop *base_p = (Base_loop *)arg;

    printf("\n ---> EVENT LOOP started, wait msg ...\n");

    for (;;) {
        rc = base_p->base_timer.base_get_next_timeout(base_p->base_tmr_hdl, &val);

        if (rc < 0) {
            wait = ARCH_OS_WAIT_FOREVER;
        } else if (val == 0) {
            wait = ARCH_OS_NO_WAIT;
        } else {
            wait = val;
        }

        base_buffer_t* base_buffer = NULL;

        // printf("lyh add -> val = %llu, wait time tick = %llu\n", val, wait);
        rc = base_p->base_queue_receive(&base_buffer, wait);
        // printf("lyh add -> rc = %d, base_buffer1 = %p\n", rc, base_buffer);

        /* update current time after wakeup */
        base_p->base_timer.base_time_update(base_p->base_tmr_hdl);
        base_p->base_timer.base_proc_timer(base_p->base_tmr_hdl);

        if (rc == 0) {

            /* receive data from queue */
            req = (base_request_t*)(base_buffer->buffer);
            bzero(&rsp, sizeof(rsp));

            /* execute event request */
            err = base_p->base_proc_request(req, (void*)&rsp);

            /* execute callback */
            if (req->cb_func) {
                req->cb_func(err, &rsp, (void*)req->priv);
            }

            base_p->base_buffer_pool_release(base_buffer);
        }
    }
}


int Base_loop::base_loop_init()
{
    int rc = 0;

    /* Init timer */
    base_tmr_hdl = base_timer.base_timer_init();
    if (!base_tmr_hdl) {
        printf("Failed to init base timer control.\r\n");
        return -1;
    }


    /* Init memory pool */
    base_buffer_pool_init();

    /* create  Queue */
    printf("lyh add -> start base queue\n");
    int ret = base_queue_create(&(base_cb.req_queue), BASE_MAX_REQUSTS, sizeof(base_request_t *));
    if (ret < 0) {
        printf("[base]create queue failed!\r\n");
        return -1;
    }

    printf("lyh add -> start base task\n");
    pthread_t p_hander = 0;
    pthread_create(&p_hander, NULL, base_req_handler, (void *)this);

    return rc;
}


Base_loop::Base_loop()
{

}

Base_loop::~Base_loop()
{

}