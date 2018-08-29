#ifndef USB_H
#define USB_H

#include <stdlib.h>

void usb_init(void);
void usb_poll(void);

int usb_read(uint8_t *buff, size_t len);
int usb_write(uint8_t *buff, size_t len);

#endif // USB_H
