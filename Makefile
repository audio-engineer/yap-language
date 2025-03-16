# Load .env file
ifneq (,$(wildcard ./.env))
include .env
export
endif

# Detect ClangTidy
ifneq ($(shell command -v clang-tidy-20 2>/dev/null),)
CLANG_TIDY := clang-tidy-20
else ifneq ($(shell command -v clang-tidy-19 2>/dev/null),)
CLANG_TIDY := clang-tidy-19
else ifneq ($(shell command -v $(LLVM_BIN_PATH)/clang-tidy 2>/dev/null),)
CLANG_TIDY := $(LLVM_BIN_PATH)/clang-tidy
else
$(error "ClangTidy could not be detected")
endif

# Detect ClangFormat
ifeq ($(NO_CLANG_FORMAT),true)
CLANG_FORMAT :=
else ifneq ($(shell command -v clang-format-20 2>/dev/null),)
CLANG_FORMAT := clang-format-20
else ifneq ($(shell command -v clang-format-19 2>/dev/null),)
CLANG_FORMAT := clang-format-19
else ifneq ($(shell command -v $(LLVM_BIN_PATH)/clang-format 2>/dev/null),)
CLANG_FORMAT := $(LLVM_BIN_PATH)/clang-format
else
$(error "ClangFormat could not be detected")
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

SOURCES := $(wildcard $(SRC_DIR)/*.c)
HEADERS := $(wildcard $(SRC_DIR)/*.h)
ASSEMBLY := $(wildcard $(SRC_DIR)/*.asm)

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
LDFLAGS := -t $(PLATFORM) -m $(BUILD_DIR)/yap-lang.map

ASMFILES := $(patsubst %.c,$(BUILD_DIR)/%.s,$(notdir $(SOURCES)))
OBJFILES := $(patsubst %.c,$(BUILD_DIR)/%.o,$(notdir $(SOURCES)))
ASMOBJFILES := $(patsubst %.asm,$(BUILD_DIR)/%.o,$(notdir $(ASSEMBLY)))

.PHONY: all clean lint format

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
	$(CLANG_TIDY) --config-file .clang-tidy --extra-arg=-std=c99 --extra-arg=-D__CC65__ --extra-arg=-DNNEAR $(SOURCES) $(HEADERS)
	$(CLANG_TIDY) --config-file .clang-tidy -p build-native-release $(SOURCES) $(HEADERS)

format:
	$(CLANG_FORMAT) -i $(SOURCES) $(HEADERS)
