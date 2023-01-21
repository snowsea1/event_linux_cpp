#include "base_loop.h"
#include "voice_service.h"
#include "ring_buffer.h"

static base_tmr_t tmr_test = {0};
static base_tmr_t tmr_test1 = {0};

static Ring_buffer *ring_buf = NULL;
static uint8_t voice_buffer[100] = {0};
static ring_buffer_t rb = {0};

 
static Base_loop *base_loop_p = NULL;
static Config *config_p = NULL;
 
/**
 * func: test timer
 * parm:
 * return:
 */
void timer_handler(int timer_id, void* data)
{
    printf("lyh add -> This is a timer...\n");


    static int i = 0;
    uint8_t buf[20] = {0};
    sprintf((char *)buf, "%s%d", "hello", i++);

    ring_buf->RB_Write_String(&rb, buf, sizeof(buf));

    base_loop_p->base_start_tmr(base_loop_p->base_tmr_hdl, &tmr_test, timer_handler, 1000);
}

void timer_handler1(int timer_id, void* data)
{
    printf("lyh add -> This is a timer1...\n");

    uint8_t buf[20] = {0};

    ring_buf->RB_Read_String(&rb, buf, 20);
    printf("lyh add -> buf = %s\n", buf);

    base_loop_p->base_start_tmr(base_loop_p->base_tmr_hdl, &tmr_test1, timer_handler1, 2000);
}



void test_msg1(int data)
{
    printf("lyh add -> data = %d\n", data);

    string doa = config_p->config_read("DOA");
    printf("lyh add -> doa = %s\n", doa.c_str());

    string AudioRollback = config_p->config_read("AudioRollback");
    printf("lyh add -> AudioRollback = %d\n", atoi(AudioRollback.c_str()));

    config_p->config_write("DOA", "2");
    doa = config_p->config_read("DOA");
    printf("lyh add -> doa = %s\n", doa.c_str());

    return;
}

int base_test_handler(base_common_req_t* req)
{
    printf("lyh add -> enter base_test_handler\n");

    switch(req->type) {
    case CMD_ASR_START:
        test_msg1(req->data[0]);
        break;

    
    default:
        break;
    }

    return 0;
}



 
/**
 * func:
 * parm:
 * return:
 */
int main()
{
    Voice_service voice_service;
    voice_service.voice_service_init();

    base_loop_p = &voice_service.base_loop;
    config_p = &voice_service.config;

    base_loop_p->base_loop_init();
    base_loop_p->base_loop_callback(1, base_test_handler);

    base_common_req_t test_req;
    test_req.type = CMD_ASR_START;
    test_req.data[0] = 100;
    base_loop_p->base_post_request_async(1, &test_req, sizeof(test_req), NULL, NULL);

    sleep(3);

    test_req.type = CMD_ASR_START;
    test_req.data[0] = 99;
    base_loop_p->base_post_request_async(1, &test_req, sizeof(test_req), NULL, NULL);


    /* test timer */
    Ring_buffer ring_buffer;
    ring_buf = &ring_buffer;
    ring_buf->RB_Init(&rb, (uint8_t *)voice_buffer, sizeof(voice_buffer));
    base_loop_p->base_start_tmr(base_loop_p->base_tmr_hdl, &tmr_test, timer_handler, 1000);
    base_loop_p->base_start_tmr(base_loop_p->base_tmr_hdl, &tmr_test1, timer_handler1, 2000);



    /* tesf fsm */
    Fsm fsm;
    fsm.Reset();

    fsm.Advance(1);
    printf("\ncurrent status is %d\n", fsm.CurrentState());

    fsm.Advance(3);
    printf("\ncurrent status is %d\n", fsm.CurrentState());

    fsm.Advance(7);
    printf("\ncurrent status is %d\n", fsm.CurrentState());


    while(1) {
        sleep(1);

        /* test base request event */
    }
}