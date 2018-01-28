ARM_PREFIX = arm-none-eabi-
OPENCM3_DIR = ./libopencm3

CC = gcc
LD = gcc
RM = rm
OBJCOPY = objcopy

ARM_ARCH_FLAGS = -mthumb -mcpu=cortex-m0

ARM_CFLAGS = -I$(OPENCM3_DIR)/include -DSTM32F0 $(ARM_ARCH_FLAGS) -g
#ARM_CFLAGS += -fno-common -ffunction-sections -fdata-sections
ARM_LDLIBS = -lopencm3_stm32f0
ARM_LDSCRIPT = $(OPENCM3_DIR)/lib/stm32/f0/stm32f04xz6.ld
ARM_LDFLAGS = -L$(OPENCM3_DIR)/lib --static -nostartfiles -T$(ARM_LDSCRIPT)
ARM_LDFLAGS += $(ARM_ARCH_FLAGS)

main:
	$(ARM_PREFIX)$(CC) $(ARM_CFLAGS) -c main.c -o main.o
	$(ARM_PREFIX)$(CC) $(ARM_CFLAGS) -c gpio.c -o gpio.o
	$(ARM_PREFIX)$(CC) $(ARM_CFLAGS) -c usb.c -o usb.o
	$(ARM_PREFIX)$(LD) $(ARM_LDFLAGS) main.o gpio.o usb.o $(ARM_LDLIBS) -o main.elf
	$(ARM_PREFIX)$(OBJCOPY) -Obinary main.elf main.bin
	$(ARM_PREFIX)$(OBJCOPY) -Oihex main.elf main.hex

clean:
	$(RM) -f *.d *.o *.elf *.map *.bin *.hex

flash: main
	sudo $(ARM_PREFIX)gdb main.elf -ex "target extended-remote /dev/ttyBMPgdb"\
			-ex "monitor swdp_scan"\
			-ex "attach 1"\
			-ex "load"\
			-ex "run"
