#ifndef RINGBUFFER_H
#define RINGBUFFER_H

typedef struct rbuff_t {
    char *buff;
    size_t start;
    size_t end;
    size_t len;
} rbuff_t;

int rb_cap(rbuff_t *rb);
int rb_push(rbuff_t *rb, char *buff, size_t len);
int rb_pop(rbuff_t *rb, char *buff, size_t len);

#endif // RINGBUFFER_H
