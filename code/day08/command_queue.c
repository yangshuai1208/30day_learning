
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define QUEUE_CAPACITY 8U

typedef enum
{
    CMD_OPEN,
    CMD_GRAB,
    CMD_RELEASE,
    CMD_STOP
} CommandType;

typedef struct
{
    uint8_t seq;
    CommandType type;
} Command;

typedef struct
{
    Command buffer[QUEUE_CAPACITY];
    uint8_t head;
    uint8_t count;
} CommandQueue;

bool command_queue_take_next(
    CommandQueue *queue,
    Command *out,
    uint8_t *discarded)
{
    // 1. 检查指针
    if (queue == NULL || out == NULL || discarded == NULL)
    {
        return false;
    }

    // 2. 检查队列状态
    if (queue->head >= QUEUE_CAPACITY ||
        queue->count > QUEUE_CAPACITY)
    {
        return false;
    }

    *discarded = 0U;

    // 3. 空队列
    if (queue->count == 0U)
    {
        return false;
    }

    // 4. 搜索最早的STOP
    // 如果没有STOP，默认取队首
    uint8_t take_offset = 0U;

    for (uint8_t i = 0U; i < queue->count; ++i)
    {
        uint8_t index = (uint8_t)(
            (queue->head + i) % QUEUE_CAPACITY
        );

        if (queue->buffer[index].type == CMD_STOP)
        {
            take_offset = i;
            break;
        }
    }

    // 5. 取出选中的命令
    uint8_t take_index = (uint8_t)(
        (queue->head + take_offset) % QUEUE_CAPACITY
    );

    *out = queue->buffer[take_index];

    // 6. 记录丢弃数量
    *discarded = take_offset;

    // 7. 更新队首和数量
    queue->head = (uint8_t)(
        (take_index + 1U) % QUEUE_CAPACITY
    );

    queue->count = (uint8_t)(
        queue->count - take_offset - 1U
    );

    return true;
}
