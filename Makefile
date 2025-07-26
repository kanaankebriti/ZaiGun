################################################################################
# Unified Makefile: Build for Linux, Windows (MinGW), and Nintendo Switch
#
# Fixes:
# - Correctly builds a valid .nro file for Nintendo Switch.
# - Implements the proper two-stage build process for Switch:
#   1. Compile source files (.c, .cpp) to object files (.o).
#   2. Link object files (.o) into an executable (.elf).
#   3. Package the .elf, RomFS, icon, and NACP into the final .nro file.
# - Uses compiler/linker flags from the original working Makefile for stability.
# - Adds automatic NACP generation for the Switch build.
# - Standardizes CFLAGS and CXXFLAGS across all platforms.
# - Sets the correct output file extension (.exe for Windows, .nro for Switch).
# - Updated to use `elf2nro` for modern devkitPro versions.
################################################################################

# ------------------------------
# Project Configuration
# ------------------------------
TARGET      := zaigun
APP_AUTHOR  := "KenKeb"
APP_VERSION := "1.0.0"

# ------------------------------
# PLATFORM Detection
# ------------------------------
# Default to Linux if not specified. Override with `make PLATFORM=windows`, etc.
PLATFORM ?= $(shell uname -s | tr '[:upper:]' '[:lower:]' | sed 's/mingw.*/windows/')
ifeq ($(PLATFORM),)
    PLATFORM := linux
endif

# ------------------------------
# Paths and Sources
# ------------------------------
SRC_DIR     := source
BUILD_DIR   := build/$(PLATFORM)
INCLUDE_DIRS:= include/raylib
ROMFS_DIR   := romfs
ICON_FILE   := icon.jpg # Must be a 256x256 JPG in the project root

# Automatically find all source files
SRCS := $(wildcard $(SRC_DIR)/*.c) $(wildcard $(SRC_DIR)/*.cpp) $(wildcard $(SRC_DIR)/*.s)
# Generate object file paths based on source files
OBJS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(filter %.c,$(SRCS)))
OBJS += $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(filter %.cpp,$(SRCS)))
OBJS += $(patsubst $(SRC_DIR)/%.s,$(BUILD_DIR)/%.o,$(filter %.s,$(SRCS)))

# Generate include flags
INCLUDES := $(foreach dir,$(INCLUDE_DIRS),-I$(dir))

# ------------------------------
# Set Target Extension
# ------------------------------
ifeq ($(PLATFORM),windows)
    EXT := .exe
else ifeq ($(PLATFORM),switch)
    EXT := .nro
else # linux
    EXT :=
endif
BUILD_TARGET := $(BUILD_DIR)/$(TARGET)$(EXT)


# ----------------------------------------------------------
# Platform-specific Toolchains, Flags, and Rules
# ----------------------------------------------------------

ifeq ($(PLATFORM),linux)
# --- LINUX ---
    CC       := gcc
    CXX      := g++
    CFLAGS   := -g -Ofast -std=c89 -Wall -Wextra $(INCLUDES)
    CXXFLAGS := -g -Ofast -std=c++11 -Wall -Wextra $(INCLUDES)
    ASFLAGS  := -g
    LDFLAGS  := -Llib/amd64_linux -lraylib

link_rule = $(CXX) $(OBJS) -o $@ $(LDFLAGS)

else ifeq ($(PLATFORM),windows)
# --- WINDOWS (MinGW) ---
    CC       := x86_64-w64-mingw32-gcc
    CXX      := x86_64-w64-mingw32-g++
    CFLAGS   := -g -Ofast -std=c11 -Wall $(INCLUDES)
    CXXFLAGS := -g -Ofast -std=c++11 -Wall $(INCLUDES)
    ASFLAGS  := -g
    LDFLAGS  := -Llib/amd64_windows -lraylib -lopengl32 -lgdi32 -lwinmm -static

link_rule = $(CXX) $(OBJS) -o $@ $(LDFLAGS)

else ifeq ($(PLATFORM),switch)
# --- NINTENDO SWITCH ---
    # Ensure DEVKITPRO is set
    ifeq ($(strip $(DEVKITPRO)),)
        $(error "Please set DEVKITPRO environment variable (e.g., export DEVKITPRO=/opt/devkitpro)")
    endif

    # Toolchain definitions
    DEV_BIN  := $(DEVKITPRO)/devkitA64/bin
    CC       := $(DEV_BIN)/aarch64-none-elf-gcc
    CXX      := $(DEV_BIN)/aarch64-none-elf-g++
    # Use elf2nro for modern devkitPro, which replaced nrotool
    NROTOOL  := $(DEVKITPRO)/tools/bin/elf2nro
    NACPTOOL := $(DEVKITPRO)/tools/bin/nacptool

    # Output file definitions for the two-stage build
    TARGET_ELF := $(BUILD_DIR)/$(TARGET).elf
    NACP_FILE  := $(BUILD_DIR)/$(TARGET).nacp

    # Flags based on your original working Makefile
    ARCH     := -march=armv8-a+crc+crypto -mtune=cortex-a57 -mtp=soft -fPIE
    CFLAGS   := -g -Wall -Ofast -ffunction-sections $(ARCH) -D__SWITCH__ $(INCLUDES) -I$(DEVKITPRO)/portlibs/switch/include -I$(DEVKITPRO)/libnx/include
    CXXFLAGS := $(CFLAGS) -fno-rtti -fno-exceptions -std=gnu++17
    ASFLAGS  := -g $(ARCH)
    LDFLAGS  := -specs=$(DEVKITPRO)/libnx/switch.specs -g $(ARCH) -Wl,-Map,$(BUILD_DIR)/$(TARGET).map \
                -Llib/arm64_switch \
                -L$(DEVKITPRO)/portlibs/switch/lib \
                -L$(DEVKITPRO)/libnx/lib \
                -lraylib -lEGL -lGLESv2 -lglapi -ldrm_nouveau -lnx -lm

    # Flags for elf2nro
    NROFLAGS := --romfsdir=$(ROMFS_DIR)
    ifneq ("$(wildcard $(ICON_FILE))","")
        NROFLAGS += --icon=$(CURDIR)/$(ICON_FILE)
    endif

    # Define the link rule as a multi-stage process for Switch
    # This creates the .elf, then the .nacp, then packages them into the .nro
    # IMPORTANT: All recipe lines inside a 'define' block MUST start with a TAB character.
    define link_rule
	@echo "  LD       $(notdir $(TARGET_ELF))"
	$(CXX) $(OBJS) -o $(TARGET_ELF) $(LDFLAGS)
	@echo "  NACP     $(notdir $(NACP_FILE))"
	$(NACPTOOL) --create "$(TARGET)" "$(APP_AUTHOR)" "$(APP_VERSION)" $(NACP_FILE)
	@echo "  NRO      $(notdir $@)"
	# Correct syntax for elf2nro: elf2nro <elf> <nro> [options]
	$(NROTOOL) $(TARGET_ELF) $@ --nacp=$(NACP_FILE) $(NROFLAGS)
    endef
endif

# ------------------------------
# Main Targets
# ------------------------------
.PHONY: all linux windows switch clean

all: $(BUILD_TARGET) link_assets
	@echo "\nSuccessfully built $(BUILD_TARGET) for $(PLATFORM)"

linux:
	@$(MAKE) PLATFORM=linux all

windows:
	@$(MAKE) PLATFORM=windows all

switch:
	@$(MAKE) PLATFORM=switch all

# ------------------------------
# Generic Linking Rule
# This calls the platform-specific 'link_rule' defined above.
# ------------------------------
$(BUILD_TARGET): $(OBJS)
	@mkdir -p $(dir $@)
	$(link_rule)

# ------------------------------
# Generic Compilation Rules
# ------------------------------
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@echo "  CC       $(notdir $<)"
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@echo "  CXX      $(notdir $<)"
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.s
	@echo "  AS       $(notdir $<)"
	@mkdir -p $(dir $@)
	$(CC) $(ASFLAGS) -c $< -o $@

# ------------------------------
# Assets Linking/Copying
# ------------------------------
.PHONY: link_assets

link_assets:
ifeq ($(PLATFORM),linux)
	@ln -sfn $(CURDIR)/$(ROMFS_DIR) $(BUILD_DIR)/$(ROMFS_DIR)
else ifeq ($(PLATFORM),windows)
	@if not exist "$(subst /,\,$(BUILD_DIR))\$(ROMFS_DIR)" ( mklink /D "$(subst /,\,$(BUILD_DIR))\$(ROMFS_DIR)" "$(subst /,\,$(CURDIR))\$(ROMFS_DIR)" )
else ifeq ($(PLATFORM),switch)
	@echo "Assets in $(ROMFS_DIR)/ will be packaged into the NRO."
endif

# ------------------------------
# Clean
# ------------------------------
clean:
	@echo "Cleaning build files..."
	@rm -rf build
	@echo "Done."

