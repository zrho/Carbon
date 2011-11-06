# Definitions - Paths and Files
SUBPROJECT				:= libposix

SOURCE_DIR 				:= ./src
HEADER_DIR				:= ./inc
BUILD_DIR				:= ./build

TARGET_DIR 				:= ../build/user
TARGET_BIN_DIR			:= $(TARGET_DIR)/bin
TARGET_HEADER_DIR		:= $(TARGET_DIR)/inc
TARGET_STATIC			:= $(TARGET_BIN_DIR)/$(SUBPROJECT).a
TARGET_SHARED			:= $(TARGET_BIN_DIR)/$(SUBPROJECT).so

# Definitions - Toolchain
ASM						:= nasm
ASM_FLAGS				:= -f elf64

CC						:= gcc
CC_FLAGS				:= -m64 -ffreestanding -Wall -mcmodel=large
CC_FLAGS				+= -I$(TARGET_HEADER_DIR)/ -I$(HEADER_DIR)/

AR						:= ar
AR_FLAGS				:= 

LD						:= ld
LD_FLAGS				:= -z max-page-size=0x1000

# Object Files

OBJECTS					:= $(patsubst $(SOURCE_DIR)/%.c,$(BUILD_DIR)/%.o,$(shell find $(SOURCE_DIR) -name "*.c"))
OBJECTS					+= $(patsubst $(SOURCE_DIR)/%.s,$(BUILD_DIR)/%.o,$(shell find $(SOURCE_DIR) -name "*.s"))

# Targets
all: static shared

build: $(OBJECTS)

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.s
	@ echo " AS " $<
	@ mkdir -p $(@D)
	@ $(ASM) $(ASM_FLAGS) $< -o $@
	
$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.c
	@ echo " CC " $<
	@ mkdir -p $(@D)
	@ $(CC) $(CC_FLAGS) -c $< -o $@
	
static: build headers
	@ echo " AR " $(TARGET_STATIC)
	@ rm -f $(TARGET_STATIC)
	@ $(AR) $(AR_FLAGS) r $(TARGET_STATIC) $(OBJECTS)
	
shared: build headers
	@ echo " LD " $(TARGET_SHARED)
	@ $(LD) $(LD_FLAGS) -shared $(OBJECTS) -o $(TARGET_SHARED)
	
headers:
	@ cp -R $(HEADER_DIR)/* $(TARGET_HEADER_DIR)/
	
clean:
	@ rm -R $(BUILD_DIR)/*