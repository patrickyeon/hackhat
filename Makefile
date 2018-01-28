.PHONY: clean flash test

ARM_PREFIX = arm-none-eabi-
OPENCM3_DIR = libopencm3

CC = gcc
LD = gcc
RM = rm
OBJCOPY = objcopy
MKDIR = mkdir -p

CFLAGS = -g -std=c99

ARM_ARCH_FLAGS = -mthumb -mcpu=cortex-m0
ARM_CFLAGS = -I$(OPENCM3_DIR)/include -DSTM32F0 $(ARM_ARCH_FLAGS) $(CFLAGS)
#ARM_CFLAGS += -fno-common -ffunction-sections -fdata-sections
ARM_LDLIBS = -lopencm3_stm32f0
ARM_LDSCRIPT = $(OPENCM3_DIR)/lib/stm32/f0/stm32f04xz6.ld
ARM_LDFLAGS = -L$(OPENCM3_DIR)/lib --static -nostartfiles -T$(ARM_LDSCRIPT)
ARM_LDFLAGS += $(ARM_ARCH_FLAGS)

SRCDIR = src/
TESTDIR = test/
BUILDDIR = build/
UNITYDIR = Unity/
TARGETDIR = $(BUILDDIR)target/
NATIVEDIR = $(BUILDDIR)native/
NATIVETESTDIR = $(NATIVEDIR)test/
RESULTDIR = $(BUILDDIR)results/

# I don't like it, but until something better works out
_dummy := $(shell $(MKDIR) $(TARGETDIR) $(NATIVEDIR) $(NATIVETESTDIR) $(RESULTDIR))

SRCFILES = $(wildcard $(SRCDIR)*.c)
TESTFILES = $(wildcard $(TESTDIR)*.c)

RESULTS = $(patsubst $(TESTDIR)test_%.c,$(RESULTDIR)%.txt,$(TESTFILES))

main: $(patsubst $(SRCDIR)%.c,$(TARGETDIR)%.o,$(SRCFILES))
	$(ARM_PREFIX)$(CC) $(ARM_CFLAGS) -c main.c -o $(TARGETDIR)main.o
	$(ARM_PREFIX)$(LD) $(ARM_LDFLAGS) $(TARGETDIR)*.o\
			$(ARM_LDLIBS) -o $(TARGETDIR)main.elf
	$(ARM_PREFIX)$(OBJCOPY) -Obinary $(TARGETDIR)main.elf $(TARGETDIR)main.bin
	$(ARM_PREFIX)$(OBJCOPY) -Oihex $(TARGETDIR)main.elf $(TARGETDIR)main.hex

test: $(RESULTS)
	cat $^

$(RESULTDIR)%.txt: $(NATIVETESTDIR)%.o
	-./$< > $@ 2>&1

$(NATIVETESTDIR)%.o:: $(TESTDIR)test_%.c $(SRCDIR)%.c
	$(CC) -I$(UNITYDIR)src $(TESTDIR)test_$*.c $(SRCDIR)$*.c $(UNITYDIR)src/unity.c -o $@

$(TARGETDIR)%.o:: $(SRCDIR)%.c
	$(ARM_PREFIX)$(CC) $(ARM_CFLAGS) -c $< -o $@

$(NATIVEDIR)%.o:: $(SRCDIR)%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	find $(BUILDDIR) -type f -delete

flash: main
	sudo $(ARM_PREFIX)gdb $(TARGETDIR)main.elf\
			-ex "target extended-remote /dev/ttyBMPgdb"\
			-ex "monitor swdp_scan"\
			-ex "attach 1"\
			-ex "load"\
			-ex "run"
