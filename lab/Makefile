.PHONY: all clean new send show

###
### Common definitions
###

# Project
PROJECT_NAME = RTS-Lab
PROJECT_DIR  = .

# Paths
MD407  = /dev/ttyUSB0
RTSEND = $(PROJECT_DIR)/rtsend
RTSHOW = $(PROJECT_DIR)/rtshow

# if ARM GNU Toolchain is not on PATH, specify full path including trailing
# slash, e.g. $(HOME)/Downloads/arm-gnu-toolchain/bin/
TOOLCHAIN_PATH =

# Directories
BUILD_DIR = $(PROJECT_DIR)/build
OBJ_DIR   = $(BUILD_DIR)/obj
SRC_DIRS  = $(PROJECT_DIR) $(PROJECT_DIR)/driver/src
INC_DIRS  = $(PROJECT_DIR)/driver/inc $(PROJECT_DIR)/device/inc
OBJ_DIRS  = $(addprefix $(OBJ_DIR)/, $(SRC_DIRS))

# Source files are automatically detected in directories defined by SRC_DIRS
SRCS = $(foreach d, $(SRC_DIRS), $(wildcard $(d)/*.c) $(wildcard $(d)/*.s))
OBJS = $(SRCS:%=$(OBJ_DIR)/%.o)
DEPS = $(OBJS:.o=.d)

# Target output
TARGET_ELF = $(BUILD_DIR)/$(PROJECT_NAME).elf
TARGET_S19 = $(BUILD_DIR)/$(PROJECT_NAME).s19

# Target RAM
LINKER_SCRIPT = $(PROJECT_DIR)/md407-ram.x

# Compiler
CC   = $(TOOLCHAIN_PATH)arm-none-eabi-gcc
POST = $(TOOLCHAIN_PATH)arm-none-eabi-objcopy

# Flags
CFLAGS += -O0 \
          -MMD \
          -Wall \
          -mthumb \
          -mcpu=cortex-m4 \
          -mfloat-abi=hard \
          -mfpu=fpv4-sp-d16 \
          -D STM32F40_41xxx \
          $(addprefix -I, $(INC_DIRS))

LDFLAGS += -specs=nano.specs \
           -nostartfiles \
           -mthumb \
           -mfloat-abi=hard \
           -mfpu=fpv4-sp-d16 \
           -mcpu=cortex-m4 \
           -T $(LINKER_SCRIPT) \
           -lm

POSTFLAGS += -S -O srec

###
### Main target
###

all: $(TARGET_S19)

###
### Intermediate targets
###

# System-defined targets
$(TARGET_S19): $(TARGET_ELF)
	$(POST) $(POSTFLAGS) $< $@

$(TARGET_ELF): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

$(OBJS): | $(OBJ_DIRS)

$(OBJ_DIRS):
	mkdir -p $@

$(OBJ_DIR)/%.o: %
	$(CC) $(CFLAGS) -c $< -o $@

###
### Utilities
###

show:
	$(RTSHOW) $(MD407)

send: all
	$(RTSEND) $(MD407) $(BUILD_DIR)/$(PROJECT_NAME).s19

###
### Clean
###

clean:
	$(RM) -r $(BUILD_DIR)

new: clean all

-include $(DEPS)
