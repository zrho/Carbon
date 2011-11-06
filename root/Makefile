# Definitions - Paths and Files
SUBPROJECT				:= root

SOURCE_DIR 				:= ./src
HEADER_DIR				:= ./inc
BUILD_DIR				:= ./build

TARGET_DIR 				:= ../build/user
TARGET_BIN_DIR			:= $(TARGET_DIR)/bin
TARGET_HEADER_DIR		:= $(TARGET_DIR)/inc
TARGET_BIN				:= $(TARGET_BIN_DIR)/$(SUBPROJECT).bin

LIBRARIES				:= $(TARGET_BIN_DIR)/libposix.a
LIBRARIES				+= $(TARGET_BIN_DIR)/libc.a

# Definitions - Toolchain
ASM						:= nasm
ASM_FLAGS				:= -f elf64

CC						:= gcc
CC_FLAGS				:= -m64 -ffreestanding -Wall -mcmodel=large
CC_FLAGS				+= -I$(TARGET_HEADER_DIR)/ -I$(HEADER_DIR)/
CC_FLAGS				+= -D__AMD64__ -D__DEBUG__
    						
LD						:= ld
LD_FLAGS				:= -z max-page-size=0x1000
LD_FILE					:= link.ld

# Object Files

OBJECTS					:= $(patsubst $(SOURCE_DIR)/%.c,$(BUILD_DIR)/%.o,$(shell find $(SOURCE_DIR) -name "*.c"))
OBJECTS					+= $(patsubst $(SOURCE_DIR)/%.s,$(BUILD_DIR)/%.o,$(shell find $(SOURCE_DIR) -name "*.s"))

# Targets
all: build link headers

build: $(OBJECTS)

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.s
	@ echo " AS " $<
	@ mkdir -p $(@D)
	@ $(ASM) $(ASM_FLAGS) $< -o $@
	
$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.c
	@ echo " CC " $<
	@ mkdir -p $(@D)
	@ $(CC) $(CC_FLAGS) -c $< -o $@
	
link: build
	@ echo " LD " $(TARGET_BIN)
	@ $(LD) $(LD_FLAGS) -T $(LD_FILE) -o $(TARGET_BIN) $(OBJECTS) $(LIBRARIES)
	
headers:
	@ #cp -R $(HEADER_DIR)/* $(TARGET_HEADER_DIR)/
	
clean:
	@ rm -R $(BUILD_DIR)/*