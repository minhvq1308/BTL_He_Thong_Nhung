.RECIPEPREFIX = >
TARGET  := smarthome
BUILD   := build
CC      := arm-none-eabi-gcc
OBJCOPY := arm-none-eabi-objcopy
SIZE    := arm-none-eabi-size

VENDOR  := hal/vendor
FRT     := middlewares/FreeRTOS

APP_SRCS := main.c $(wildcard bsp/src/*.c) $(wildcard hal/src/*.c) $(wildcard app/src/*.c)
SPL_SRCS := $(addprefix $(VENDOR)/SPL/src/, misc.c stm32f10x_rcc.c stm32f10x_gpio.c \
            stm32f10x_adc.c stm32f10x_tim.c stm32f10x_spi.c stm32f10x_dma.c stm32f10x_usart.c)
FRT_SRCS := $(FRT)/tasks.c $(FRT)/queue.c $(FRT)/list.c $(FRT)/event_groups.c \
            $(FRT)/portable/GCC/ARM_CM3/port.c $(FRT)/portable/MemMang/heap_4.c

C_SRCS  := $(APP_SRCS) $(SPL_SRCS) $(FRT_SRCS)
ASM_SRC := bsp/startup/startup_stm32f10x_md.s

OBJS := $(addprefix $(BUILD)/,$(notdir $(C_SRCS:.c=.o))) \
        $(addprefix $(BUILD)/,$(notdir $(ASM_SRC:.s=.o)))
VPATH := $(sort $(dir $(C_SRCS) $(ASM_SRC)))

INCLUDES := -Ibsp/inc -Ihal/inc -Iapp/inc -I$(VENDOR)/CMSIS -I$(VENDOR)/SPL/inc \
            -I$(FRT)/include -I$(FRT)/config -I$(FRT)/portable/GCC/ARM_CM3
DEFS    := -DSTM32F10X_MD -DUSE_STDPERIPH_DRIVER -DHSE_VALUE=8000000
CPU     := -mcpu=cortex-m3 -mthumb
CFLAGS  := $(CPU) -Os -g -Wall -std=gnu99 -ffunction-sections -fdata-sections \
           $(DEFS) $(INCLUDES) -MMD -MP
ASFLAGS := $(CPU) -g
LDFLAGS := $(CPU) -Tbsp/ld/stm32f103c8_flash.ld -Wl,--gc-sections \
           -Wl,-Map=$(BUILD)/$(TARGET).map --specs=nano.specs --specs=nosys.specs \
           -Wl,--print-memory-usage

.PHONY: all clean size flash flash-stlink flash-uart

all: $(BUILD)/$(TARGET).elf $(BUILD)/$(TARGET).bin $(BUILD)/$(TARGET).hex size

$(BUILD)/%.o: %.c | $(BUILD)
> $(CC) $(CFLAGS) -c $< -o $@
$(BUILD)/%.o: %.s | $(BUILD)
> $(CC) $(ASFLAGS) -c $< -o $@
$(BUILD)/$(TARGET).elf: $(OBJS)
> $(CC) $(OBJS) $(LDFLAGS) -o $@
$(BUILD)/%.bin: $(BUILD)/%.elf
> $(OBJCOPY) -O binary $< $@
$(BUILD)/%.hex: $(BUILD)/%.elf
> $(OBJCOPY) -O ihex $< $@
$(BUILD):
> mkdir -p $@

size: $(BUILD)/$(TARGET).elf
> $(SIZE) $<

flash: $(BUILD)/$(TARGET).elf
> openocd -f openocd.cfg -c "program $< verify reset exit"
flash-stlink: $(BUILD)/$(TARGET).bin
> st-flash --reset write $< 0x8000000
SERIAL ?= /dev/ttyUSB0
flash-uart: $(BUILD)/$(TARGET).bin
> stm32flash -w $< -v -g 0x08000000 -b 57600 $(SERIAL)

clean:
> rm -rf $(BUILD)
-include $(wildcard $(BUILD)/*.d)
