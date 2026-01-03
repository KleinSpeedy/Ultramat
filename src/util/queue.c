#include "util/queue.h"
#include "proto/commands.pb.h"

#include <string.h>

#define Q_IS_FULL(q) (((q->head + 1) % QUEUE_MAX_SIZE) == q->tail)
#define Q_IS_EMPTY(q) (q->tail == q->head)

int out_msg_q_is_empty(const out_msg_q *const q)
{
    return Q_IS_EMPTY(q);
}

int out_msg_q_init(out_msg_q *q)
{
    q->tail = 0;
    q->head = 0;

    memset(&q->messages, 0, Message_size * QUEUE_MAX_SIZE);

    return 0;
}

int in_resp_q_init(in_resp_q *q)
{
    q->tail = 0;
    q->head = 0;

    memset(&q->responses, 0, Response_size * QUEUE_MAX_SIZE);

    return 0;
}

int out_msg_q_push(out_msg_q *const q, const Message *msg)
{
    if(!q || !msg)
        return -1;

    if(Q_IS_FULL(q))
        return -1;

    memcpy(&q->messages[q->head], msg, sizeof(*msg));
    q->head = (q->head + 1) % QUEUE_MAX_SIZE;
    return 0;
}

int in_resp_q_push(in_resp_q *const q, const Response *resp)
{
    if(!q || !resp)
        return -1;

    if(Q_IS_FULL(q))
        return -1;

    memcpy(&q->responses[q->head], resp, sizeof(*resp));
    q->head = (q->head + 1) % QUEUE_MAX_SIZE;
    return 0;
}

int out_msg_q_pop(out_msg_q *const q, Message *const msg)
{
    if(!q || !msg)
        return -1;

    if(Q_IS_EMPTY(q))
        return -1;

    memcpy(msg, &q->messages[q->tail], sizeof(*msg));
    q->tail = (q->tail + 1) % QUEUE_MAX_SIZE;
    return 0;
}

int in_resp_q_pop(in_resp_q *const q, Response *const resp)
{
    if(!q || !resp)
        return -1;

    if(Q_IS_EMPTY(q))
        return -1;

    memcpy(resp, &q->responses[q->tail], sizeof(*resp));
    q->tail = (q->tail + 1) % QUEUE_MAX_SIZE;
    return 0;
}
