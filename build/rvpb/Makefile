ARCH = armv7-a
MCPU = cortex-a8

TARGET = rvpb
ARM_ARCH = cortexAR

CC = arm-none-eabi-gcc
AS = arm-none-eabi-as
LD = arm-none-eabi-gcc
OC = arm-none-eabi-objcopy
OD = arm-none-eabi-objdump

OUTDIR=./out

LINKER_SCRIPT = ./build/rvpb/iiijin.ld
MAP_FILE = $(OUTDIR)/iiijin.map
SYM_FILE = $(OUTDIR)/iiijin.sym

ASM_SRCS = $(wildcard ./boot/$(TARGET)/*.S)
ASM_OBJS = $(patsubst ./boot/$(TARGET)/%.S, $(OUTDIR)/%.os, $(ASM_SRCS))

VPATH = boot/$(TARGET) 			\
        hal/$(TARGET)	\
        lib				\
	lib/$(ARM_ARCH)		\
        kernel

C_SRCS  = $(notdir $(wildcard ./boot/$(TARGET)/*.c))
C_SRCS += $(notdir $(wildcard ./hal/$(TARGET)/*.c))
C_SRCS += $(notdir $(wildcard ./lib/*.c))
C_SRCS += $(notdir $(wildcard ./lib/$(ARM_ARCH)/*.c))
C_SRCS += $(notdir $(wildcard ./kernel/*.c))
C_OBJS = $(patsubst %.c, $(OUTDIR)/%.o, $(C_SRCS))

INC_DIRS  = -I boot/$(TARGET)	\
            -I include 			\
            -I hal	   			\
            -I hal/$(TARGET)	\
            -I lib				\
            -I lib/$(ARM_ARCH)				\
            -I kernel

CFLAGS = -c -g -std=c11 -mthumb-interwork

LDFLAGS = -nostartfiles -nostdlib -nodefaultlibs -static -lgcc

iiijin = $(OUTDIR)/iiijin.axf
iiijin_bin = $(OUTDIR)/iiijin.bin

.PHONY: all clean run debug gdb

all: $(iiijin)

clean:
	@rm -fr $(OUTDIR)
	
run: $(iiijin)
	qemu-system-arm -M realview-pb-a8 -kernel $(iiijin) -nographic
	
debug: $(iiijin)
	qemu-system-arm -M realview-pb-a8 -kernel $(iiijin) -nographic -S -gdb tcp::1234,ipv4
	
gdb:
	gdb-multiarch

kill:
	kill -9 `ps aux | grep 'qemu' | awk 'NR==1{print $$2}'`
	
$(iiijin): $(ASM_OBJS) $(C_OBJS) $(LINKER_SCRIPT)
	$(LD) -n -T $(LINKER_SCRIPT) -o $(iiijin) $(ASM_OBJS) $(C_OBJS) -Wl,-Map=$(MAP_FILE) $(LDFLAGS)
	$(OD) -t $(iiijin) > $(SYM_FILE)
	$(OC) -O binary $(iiijin) $(iiijin_bin)
	
$(OUTDIR)/%.os: %.S
	mkdir -p $(shell dirname $@)
	$(CC) -march=$(ARCH) -mtune=$(MCPU) -marm $(INC_DIRS) $(CFLAGS) -o $@ $<
	
$(OUTDIR)/%.o: %.c
	mkdir -p $(shell dirname $@)
	$(CC) -march=$(ARCH) -mtune=$(MCPU) -marm $(INC_DIRS) $(CFLAGS) -o $@ $<
	
