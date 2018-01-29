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

#define mk_rb(name, len) \
    char _##name_buff[len + 1];\
    rbuff_t  name = {_##name_buff, 0, 0, len + 1};

#endif // RINGBUFFER_H
