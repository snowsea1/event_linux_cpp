#ifndef _BASE_TIMER_H_
#define _BASE_TIMER_H_
 

#include "base_common.h"
#include "list.h"
 
#define MS_PER_TICK                 1         /* milli-seconds per tick */
 
typedef unsigned long long base_ms_t;
typedef void* base_tmr_hdl_t;
 
typedef void (*timer_func_t)(int timer_id, void* data);
 
/**
 * timer struct data
*/
typedef struct {
    list_head_t    self;
    int            timer_id;
    base_ms_t      val;           /* how many ticks? */
    base_ms_t      expire;
    int            repeat;
    timer_func_t   fn;
    void           *data;
} base_tmr_t;
 
#if 0
/**
 * start timer without data
 *
*/
#define base_start_tmr(hdl, tmr, func, to)      \
    do {                                        \
        if (base_timer_is_running(tmr)) {       \
            base_del_timer(tmr);                \
        }                                       \
        (tmr)->val = to;                        \
        (tmr)->timer_id = 0;                    \
        (tmr)->data = NULL;                     \
        (tmr)->fn = func;                       \
        (tmr)->repeat = 0;                      \
        base_add_timer(hdl, (tmr));             \
    } while (0)




#define base_start_tmr_with_data(hdl, tmr, func, to, d) \
    do {                                        \
        if (base_timer_is_running(tmr)) {       \
            base_del_timer(tmr);                \
            printf("\n\nwarning %s: timer already running\n\n", __FUNCTION__); \
        }                                       \
        (tmr)->val = to;                        \
        (tmr)->timer_id = 0;                    \
        (tmr)->data = d;                        \
        (tmr)->fn = func;                       \
        (tmr)->repeat = 0;                      \
        base_add_timer(hdl, (tmr));             \
    } while (0)

#endif 

// extern void base_task_timer_update(void);

class Base_timer {

  public:
    Base_timer();
    ~Base_timer();
    base_tmr_hdl_t base_timer_init(void);
    void base_timer_deinit(base_tmr_hdl_t);



    void base_proc_timer(base_tmr_hdl_t);

    void base_add_timer(base_tmr_hdl_t, base_tmr_t *);
    void base_re_add_timer(base_tmr_hdl_t hdl, base_tmr_t *timer);
    void base_del_timer(base_tmr_t *);
    int base_timer_is_running(base_tmr_t *);
    int base_get_next_timeout(base_tmr_hdl_t, base_ms_t *);
    void base_time_update(base_tmr_hdl_t);

  private:

    pthread_mutex_t lock_timer = {0};
    base_ms_t base_timer_expire(base_tmr_hdl_t, base_tmr_t *);
};

 
#endif