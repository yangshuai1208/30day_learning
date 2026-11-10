#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

bool dma_copy_delta(
    const uint8_t *dma_buf,
    size_t size,
    size_t last_pos,
    size_t new_pos,
    uint8_t *out,
    size_t out_cap,
    size_t *copied)
{
    if(dma_buf==NULL||out==NULL||copied==NULL)
        return false;

if (size == 0 ||
    last_pos >= size ||
    new_pos >= size)
{
    return false;
}

    size_t delta;

    if (new_pos >= last_pos)
    {
    delta = new_pos - last_pos;
    }
    else
    {
    delta = size - last_pos + new_pos;
    }
    if(delta>out_cap)
        return false;

    for(size_t i=0;i<delta;i++)
       out[i] = dma_buf[(last_pos + i) % size];

    *copied=delta;
    return true;
}