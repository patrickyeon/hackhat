#ifndef USB_H
#define USB_H

#include <stdlib.h>

void init_usb(void);
void usb_poll(void);

int usb_read(uint8_t *buff, size_t len);
int usb_write(uint8_t *buff, size_t len);

#endif // USB_H
