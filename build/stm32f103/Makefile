MCPU = cortex-m3
ARMTHMODE = thumb
ARM_ARCH = cortexM

SOC = STM32F103RBTX_FLASH
BOARD = stm32f103

STFLASH = st-flash

OUTNAME = iiijin_rtos

USR_DEFINE = -DSTM32F103xB -DHSE_VALUE=8000000U

CC = arm-none-eabi-gcc
AS = arm-none-eabi-as
LD = arm-none-eabi-gcc
OC = arm-none-eabi-objcopy
OD = arm-none-eabi-objdump

PRJ_HOME = .
OUTPUT_DIR = $(PRJ_HOME)/out

LINKER_SCRIPT = $(PRJ_HOME)/build/$(BOARD)/$(SOC).ld

MAP_FILE = $(OUTPUT_DIR)/$(OUTNAME).map
SYM_FILE = $(OUTPUT_DIR)/$(OUTNAME).sym

ASM_SRCS = $(wildcard $(PRJ_HOME)/boot/$(BOARD)/*.S)
ASM_OBJS = $(patsubst $(PRJ_HOME)/boot/$(BOARD)/%.S, $(OUTPUT_DIR)/%.os, $(ASM_SRCS))

VPATH = boot/$(BOARD) 				\
        hal/$(BOARD)	        	\
        hal/$(BOARD)/drivers    	\
        lib							\
        lib/$(ARM_ARCH)				\
        kernel

C_SRCS  = $(notdir $(wildcard $(PRJ_HOME)/boot/$(BOARD)/*.c))
C_SRCS += $(notdir $(wildcard $(PRJ_HOME)/hal/$(BOARD)/*.c))
C_SRCS += $(notdir $(wildcard $(PRJ_HOME)/hal/$(BOARD)/drivers/*.c))
C_SRCS += $(notdir $(wildcard $(PRJ_HOME)/lib/*.c))
C_SRCS += $(notdir $(wildcard $(PRJ_HOME)/lib/$(ARM_ARCH)/*.c))
C_SRCS += $(notdir $(wildcard $(PRJ_HOME)/kernel/*.c))

C_OBJS = $(patsubst %.c, $(OUTPUT_DIR)/%.o, $(C_SRCS))

INC_DIRS  = -I boot/$(BOARD)    		\
            -I include 					\
            -I hal	   					\
            -I hal/$(BOARD)	    		\
            -I hal/$(BOARD)/drivers	    \
            -I lib						\
            -I lib/$(ARM_ARCH)			\
            -I .                        \
            -I kernel

COMMON_FLG = -mcpu=$(MCPU) -m$(ARMTHMODE)

CCFLAGS = -g
CCFLAGS += -Os -Wall -fdata-sections -ffunction-sections
CCFLAGS += $(USR_DEFINE) $(INC_DIRS)
CCFLAGS += $(COMMON_FLG) 

CFLAGS = -c -g -std=c11 
CFLAGS += -Os -Wall -fdata-sections -ffunction-sections
CFLAGS += $(USR_DEFINE) $(INC_DIRS)
CFLAGS += $(COMMON_FLG) 

LDFLAGS = $(COMMON_FLG) -Wl,--cref,--gc-sections
LDFLAGS += -lc -lm -lnosys

iiijin = $(OUTPUT_DIR)/$(OUTNAME).axf
iiijin_bin = $(OUTPUT_DIR)/$(OUTNAME).bin

.PHONY: all clean run debug gdb

all: $(iiijin)

burn: $(iiijin)
	sudo stm32flash -w $(iiijin) -v -g 0x0 /dev/ttyACM0

flash: $(iiijin_bin)
	$(STFLASH) --reset  write $(iiijin_bin) 0x08000000

clean:
	@echo "Delete.. " $(OUTPUT_DIR) 
	@rm -fr $(OUTPUT_DIR) 
	
$(iiijin): $(ASM_OBJS) $(CC_OBJS) $(C_OBJS)
	@echo "Linking.."
	@echo "Generate binary.. " $(iiijin_bin)
	@$(LD) $(ASM_OBJS) $(CC_OBJS) $(C_OBJS) -o $(iiijin) -n -T $(LINKER_SCRIPT) -Wl,-Map=$(MAP_FILE) $(LDFLAGS)
	@$(OD) -t $(iiijin) > $(SYM_FILE)
	@$(OD) -d $(iiijin) > $(iiijin).dump
	@$(OC) -O binary $(iiijin) $(iiijin_bin)
	
$(OUTPUT_DIR)/%.os: %.S
	@echo "compile " $<
	@mkdir -p $(shell dirname $@)
	@$(CC) $(CFLAGS) -o $@ $<
	@$(OD) -d $@ > $@.dump
	
$(OUTPUT_DIR)/%.o: %.c
	@echo "compile " $<
	@mkdir -p $(shell dirname $@)
	@$(CC) $(CFLAGS) -o $@ $<
	@$(OD) -d $@ > $@.dump
