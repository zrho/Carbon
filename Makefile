.PHONY: all clean iso run
.PHONY: kernel loader libc libposix root

# Definitions - Directories and Files
BUILD_DIR			:= build
ISO					:= $(BUILD_DIR)/boot.iso
ISO_STAGE2			:= boot/grub/stage2_eltorito
ISO_DIR				:= kernel/iso
ISO_BIN_DIR			:= kernel/iso/boot

# Definitions - Tools
MAKE				:= make

EMU					:= bochs
EMU_FLAGS			:= -q

# Targets - Special
iso: all
	@ echo " Copying binaries for ISO..."
	@ cp $(BUILD_DIR)/kernel/bin/kernel.bin $(ISO_BIN_DIR)/
	@ cp $(BUILD_DIR)/kernel/bin/loader.bin $(ISO_BIN_DIR)/
	@ cp $(BUILD_DIR)/user/bin/root.bin $(ISO_BIN_DIR)/
	
	@ echo " Making ISO..."
	@ mkisofs -R -b $(ISO_STAGE2) -no-emul-boot \
	          -boot-load-size 4 -boot-info-table \
	          -o $(ISO) $(ISO_DIR) 

run: iso
	$(EMU) $(EMU_FLAGS)

# Targets - Build
all: kernel loader libc libposix root

kernel:
	@ echo " Building kernel..."
	@ $(MAKE) -C kernel/ all
	
loader: kernel
	@ echo " Building loader..."
	@ $(MAKE) -C loader/ all
	
libc:
	@ echo " Building libc..."
	@ $(MAKE) -C libc/ all
	
libposix: libc
	@ echo " Building libposix..."
	@ $(MAKE) -C libposix/ all
	
root: libc libposix
	@ echo " Building root..."
	@ $(MAKE) -C root/ all
	
# Targets - Clean
clean:
	@ $(MAKE) -C kernel/ clean
	@ $(MAKE) -C loader/ clean
	@ $(MAKE) -C libc/ clean
	@ $(MAKE) -C libposix/ clean
	@ $(MAKE) -C root/ clean
	@ rm -R $(BUILD_DIR)/kernel/inc/*
	@ rm -R $(BUILD_DIR)/kernel/bin/*
	@ rm -R $(BUILD_DIR)/user/inc/*
	@ rm -R $(BUILD_DIR)/user/bin/*