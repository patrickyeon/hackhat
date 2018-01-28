.PHONY: clean flash test

ARM_PREFIX = arm-none-eabi-
OPENCM3_DIR = libopencm3

CC = gcc
LD = gcc
RM = rm
OBJCOPY = objcopy
MKDIR = mkdir -p

ARM_ARCH_FLAGS = -mthumb -mcpu=cortex-m0

ARM_CFLAGS = -I$(OPENCM3_DIR)/include -DSTM32F0 $(ARM_ARCH_FLAGS) -g -std=c99
#ARM_CFLAGS += -fno-common -ffunction-sections -fdata-sections
ARM_LDLIBS = -lopencm3_stm32f0
ARM_LDSCRIPT = $(OPENCM3_DIR)/lib/stm32/f0/stm32f04xz6.ld
ARM_LDFLAGS = -L$(OPENCM3_DIR)/lib --static -nostartfiles -T$(ARM_LDSCRIPT)
ARM_LDFLAGS += $(ARM_ARCH_FLAGS)

SRCDIR = src/
BUILDDIR = build/
OBJDIR = $(BUILDDIR)objs/

SRCFILES = $(wildcard $(SRCDIR)*.c)

main: $(patsubst $(SRCDIR)%.c,$(OBJDIR)%.o,$(SRCFILES))
	$(ARM_PREFIX)$(CC) $(ARM_CFLAGS) -c main.c -o $(OBJDIR)main.o
	$(ARM_PREFIX)$(LD) $(ARM_LDFLAGS) $(OBJDIR)*.o\
			$(ARM_LDLIBS) -o $(OBJDIR)main.elf
	$(ARM_PREFIX)$(OBJCOPY) -Obinary $(OBJDIR)main.elf $(OBJDIR)main.bin
	$(ARM_PREFIX)$(OBJCOPY) -Oihex $(OBJDIR)main.elf $(OBJDIR)main.hex

$(OBJDIR)%.o:: $(SRCDIR)%.c $(OBJDIR)
	$(ARM_PREFIX)$(CC) $(ARM_CFLAGS) -c $< -o $@

$(OBJDIR):
	$(MKDIR) $(OBJDIR)

clean:
	$(RM) -rf $(BUILDDIR)

flash: main
	sudo $(ARM_PREFIX)gdb $(OBJDIR)main.elf\
			-ex "target extended-remote /dev/ttyBMPgdb"\
			-ex "monitor swdp_scan"\
			-ex "attach 1"\
			-ex "load"\
			-ex "run"
