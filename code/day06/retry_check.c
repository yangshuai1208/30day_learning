#include <stdbool.h>
#include <stddef.h>  
#include <stdint.h>

typedef enum
{
    RETRY_INVALID = 0,
    RETRY_NO_PENDING,
    RETRY_WAIT,
    RETRY_SEND_AGAIN,
    RETRY_GIVE_UP
} RetryResult;

typedef struct
{
    uint32_t last_send_tick;
    uint8_t retry_count;
    bool awaiting_ack;
} RetryState;

RetryResult retry_check(
    const RetryState *state,
    uint32_t now_tick,
    uint32_t timeout_ms,
    uint8_t max_retries)
{
    // 请自行实现
    if(state==NULL||timeout_ms==0)
    {
        return RETRY_INVALID;
    }

    if(!state->awaiting_ack)
    {
        return RETRY_NO_PENDING;
    }

    if(now_tick - state->last_send_tick < timeout_ms)
    {
        return RETRY_WAIT;
    }

    if(state->retry_count >= max_retries)
    {
        return RETRY_GIVE_UP;
    }

    return RETRY_SEND_AGAIN;
}