#include <time.h>
#include <stdlib.h>
#include "base_timer.h"
 
typedef struct tmr_cb {
    list_head_t         tmr_list;
    volatile base_ms_t  base_sys_msec;
    volatile base_ms_t  base_tmr_wait;
} base_tmr_cb_t;
 

/* t1 is earlier than t2 */
#define time_before(t1, t2) ((long long )((t1)-(t2)) <= 0)
#define timer_before(tmr1, tmr2) (time_before(tmr1->expire, tmr2->expire))
 
/**
 * Update the current time.
 * Because all timer's expiring time is relative to current time, so we must
 * update current time after each time-consuming operations, e.g. epoll_wait.
 */
void Base_timer::base_time_update(base_tmr_hdl_t hdl)
{
    static struct timespec tm = {0};
 
    /* get system time, and save to timer */
    // rc = clock_gettime(CLOCK_MONOTONIC, &tm);
    int rc = clock_gettime(CLOCK_REALTIME, &tm);
    if (rc < 0) abort();

    // printf("lyh add -> tv_sec = %ld, tv_nsec=%ld\n", tm.tv_sec, tm.tv_nsec);
    ((base_tmr_cb_t*)hdl)->base_sys_msec = (base_ms_t)(tm.tv_sec * 1000) + (base_ms_t)(tm.tv_nsec)/(1000*1000);

    // printf("base_sys_msec = %llu\n", ((base_tmr_cb_t*)hdl)->base_sys_msec);

    return;
}
 
/**
 * Create a timer list
 */
base_tmr_hdl_t Base_timer::base_timer_init()
{
    base_tmr_cb_t* thdl = (base_tmr_cb_t *)malloc(sizeof(base_tmr_cb_t));
 
    if (!thdl) return NULL;
 
    /* create timer list */
    INIT_LIST_HEAD(&(thdl->tmr_list));
    /* update timer */
    base_time_update((base_tmr_hdl_t)thdl);
    
    thdl->base_tmr_wait = 0xFFFFFFFF;


    pthread_mutex_init(&lock_timer, NULL);

 
    return (base_tmr_hdl_t)thdl;
}
 
/**
 * Delete a timer list
 */
void Base_timer::base_timer_deinit(base_tmr_hdl_t hdl)
{
    base_tmr_t *tmr, *tmp;

    list_for_each_entry_safe(tmr, tmp, &(((base_tmr_cb_t*)hdl)->tmr_list), self, base_tmr_t) {
        list_del(&(tmr->self));
    }

    free(hdl);
}
 
/**
 * Place the timer into timer queue.
 */
void Base_timer::base_add_timer(base_tmr_hdl_t hdl, base_tmr_t* timer)
{
    base_tmr_t* tmr = NULL;
    base_tmr_cb_t* tcb = (base_tmr_cb_t*)hdl;

    timer->expire = ((base_tmr_cb_t*)hdl)->base_sys_msec + timer->val * MS_PER_TICK;
    INIT_LIST_HEAD(&(timer->self));

    pthread_mutex_lock(&lock_timer);

    list_for_each_entry(tmr, &(((base_tmr_cb_t*)hdl)->tmr_list), self, base_tmr_t) {
        if (timer_before(timer, tmr)) {
            break;
        }
    }

    list_add_tail(&(timer->self), &(tmr->self));

    pthread_mutex_unlock(&lock_timer);

    if (time_before(timer->expire, tcb->base_tmr_wait) && (timer->expire != tcb->base_tmr_wait)) {
        //printf("sync ytmr new%lld wait%lld\r\n", timer->expire, tcb->base_tmr_wait);
        tcb->base_tmr_wait = timer->expire;
        // base_task_timer_update();
    }
}


/*
 * timer is running, return 1, other return 0
 */
int Base_timer::base_timer_is_running(base_tmr_t *timer)
{
    if (timer->self.next != NULL && timer->self.prev != NULL) {
        return 1;
    }
    return 0;
}


/*
 * return the remain time. unit: ms
 */
base_ms_t Base_timer::base_timer_expire(base_tmr_hdl_t hdl, base_tmr_t *timer)
{
    base_tmr_cb_t* tcb = (base_tmr_cb_t*)hdl;

    if (base_timer_is_running(timer)) {
        if (time_before(timer->expire, tcb->base_sys_msec)) {
            return 0;
        } else {
            return (timer->expire - tcb->base_sys_msec);
        }
    }

    return 0;
}
 
void Base_timer::base_del_timer(base_tmr_t* timer)
{
    pthread_mutex_lock(&lock_timer);
    if (timer->self.next != NULL && timer->self.prev != NULL) {
        list_del(&(timer->self));
    }
    pthread_mutex_unlock(&lock_timer);
}


 
/**
 * Do callbacks for all the expired timer, restart the timer
 * if it's repeatitive.
 */
void Base_timer::base_proc_timer(base_tmr_hdl_t hdl)
{
    base_tmr_cb_t* tcb = (base_tmr_cb_t*)hdl;
    base_tmr_t *tmr;

    for (;;) {
        if (list_empty(&(tcb->tmr_list))) {
            break;
        }

        tmr = list_first_entry(&(tcb->tmr_list), base_tmr_t, self);

        if (time_before(tmr->expire, tcb->base_sys_msec)) {
            base_del_timer(tmr);
            if (tmr->repeat)
                base_add_timer(hdl, tmr);
            tmr->fn(tmr->timer_id, tmr->data);
        } else {
            break;
        }
    }
}

 
 
/**
 * Find out how much time can we sleep before we need to
 * wake up to handle the timer.
 */
int Base_timer::base_get_next_timeout(base_tmr_hdl_t hdl, base_ms_t* tick)
{
    // printf("lyh add -> enter base_get_next_timeout\n");
    base_tmr_cb_t* tcb = (base_tmr_cb_t*)hdl;
    base_tmr_t* tmr;

    if (list_empty(&(tcb->tmr_list))) {
        return -1;
    }

    tmr = list_first_entry(&(tcb->tmr_list), base_tmr_t, self);

    // printf("lyh add -> expire = %llu, base_sys_msec = %llu\n", tmr->expire, tcb->base_sys_msec);
    if (time_before(tmr->expire, tcb->base_sys_msec)) {
        *tick = 0;
    } else {
        *tick = tmr->expire - tcb->base_sys_msec;
        // *tick = tmr->expire;
    }
    tcb->base_tmr_wait = tmr->expire;

    return 0;
}


Base_timer::Base_timer()
{}

Base_timer::~Base_timer(){}