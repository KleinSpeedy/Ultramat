#ifndef QUEUE_H
#define QUEUE_H

#include "proto/commands.pb.h"

#define QUEUE_MAX_SIZE 32

typedef struct msg_q
{
    int head;
    int tail;

    Message messages[QUEUE_MAX_SIZE];
} out_msg_q;

typedef struct resp_q
{
    int head;
    int tail;

    Response responses[QUEUE_MAX_SIZE];
} in_resp_q;

int out_msg_q_init(out_msg_q *q);
int in_resp_q_init(in_resp_q *q);

int out_msg_q_push(out_msg_q *const q, const Message *msg);
int in_resp_q_push(in_resp_q *const q, const Response *resp);

int out_msg_q_pop(out_msg_q *const q, Message *const msg);
int in_resp_q_pop(in_resp_q *const q, Response *const resp);

int out_msg_q_is_empty(const out_msg_q *const q);

#endif // QUEUE_H
