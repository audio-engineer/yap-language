# Load .env file
ifneq (,$(wildcard ./.env))
include .env
export
endif

TARGET := yap-lang.prg
PLATFORM := c128
BUILD_TYPE ?= Debug

SRC_DIR := src
ifeq ($(BUILD_TYPE),Release)
BUILD_DIR := build-$(PLATFORM)-release
else
BUILD_DIR := build-$(PLATFORM)-debug
endif
TESTS_DIR := tests

SOURCES := $(wildcard $(SRC_DIR)/*.c)
HEADERS := $(wildcard $(SRC_DIR)/*.h)
ASSEMBLY := $(wildcard $(SRC_DIR)/*.asm)
TESTS := $(wildcard $(TESTS_DIR)/*.c)

CC65_BIN_PATH := $(CC65_PATH)/bin
CC65_LIB_PATH := $(CC65_PATH)/lib

CC65 := $(CC65_BIN_PATH)/cc65
CA65 := $(CC65_BIN_PATH)/ca65
LD65 := $(CC65_BIN_PATH)/ld65

WARNINGS := const-comparison,$\
error,$\
no-effect,$\
pointer-sign,$\
remap-zero,$\
return-type,$\
struct-param,$\
unknown-pragma,$\
unreachable-code,$\
unused-func,$\
unused-label,$\
unused-param,$\
unused-var,$\
const-overflow

CFLAGS := -O -t $(PLATFORM) -W $(WARNINGS)
ifeq ($(BUILD_TYPE),Release)
CFLAGS += -DNDEBUG
endif
AFLAGS :=
LDFLAGS := -t $(PLATFORM)
ifeq ($(BUILD_TYPE),Debug)
LDFLAGS += -m $(BUILD_DIR)/yap-lang.map
endif

ASMFILES := $(patsubst %.c,$(BUILD_DIR)/%.s,$(notdir $(SOURCES)))
OBJFILES := $(patsubst %.c,$(BUILD_DIR)/%.o,$(notdir $(SOURCES)))
ASMOBJFILES := $(patsubst %.asm,$(BUILD_DIR)/%.o,$(notdir $(ASSEMBLY)))

.PHONY: all clean lint format test

all: $(BUILD_DIR)/$(TARGET)

# Disable built-in rules
%.o: %.c

# Don't delete object and assembly files
.PRECIOUS: $(BUILD_DIR)/%.o $(BUILD_DIR)/%.s

# All assembly files depend on all headers: Trigger total rebuild when a header changes
$(ASMFILES): $(HEADERS)

$(BUILD_DIR):
	mkdir -p $@

$(BUILD_DIR)/%.s: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC65) $(CFLAGS) -o $@ $<

$(BUILD_DIR)/%.s: $(SRC_DIR)/%.asm
	cp $< $@

$(BUILD_DIR)/%.o: $(BUILD_DIR)/%.s
	$(CA65) $(AFLAGS) $<

$(BUILD_DIR)/$(TARGET): $(OBJFILES) $(ASMOBJFILES)
	$(LD65) -o $@ $(LDFLAGS) $^ $(CC65_LIB_PATH)/$(PLATFORM).lib

clean:
	$(RM) -r $(BUILD_DIR)/*

lint:
	./llvm-tools lint

format:
	./llvm-tools format

test:
	./llvm-tools test
