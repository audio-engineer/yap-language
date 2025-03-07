# Detect ClangTidy
ifeq ($(shell which clang-tidy-20 2>/dev/null),)
    ifeq ($(shell which clang-tidy 2>/dev/null),)
        CLANG_TIDY ?= $(LLVM_BIN_PATH)/clang-tidy
    else
        CLANG_TIDY = clang-tidy
    endif
else
    CLANG_TIDY = clang-tidy-20
endif

# Load .env file
ifneq (,$(wildcard ./.env))
    include .env
    export
endif

TARGET = yap-lang.prg
PLATFORM = c128

SRC_DIR := src
BUILD_DIR := build-$(PLATFORM)-release

SOURCES := $(wildcard $(SRC_DIR)/*.c)
HEADERS := $(wildcard $(SRC_DIR)/*.h)

CC65 = $(CC65_PATH)/bin/cc65
CA65 = $(CC65_PATH)/bin/ca65
LD65 = $(CC65_PATH)/bin/ld65

CC65_LIB = $(CC65_PATH)/lib

CC = $(CC65)

WARNINGS = const-comparison,$\
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

CFLAGS = -O -t $(PLATFORM) -W $(WARNINGS)
AFLAGS =
LDFLAGS	= -t $(PLATFORM) -m $(BUILD_DIR)/yap-lang.map

ASMFILES := $(patsubst %.c,$(BUILD_DIR)/%.s,$(notdir $(SOURCES)))
OBJFILES := $(patsubst %.c,$(BUILD_DIR)/%.o,$(notdir $(SOURCES)))

.PHONY: all clean lint format

all: $(BUILD_DIR)/$(TARGET)

# Disable built-in rules
%.o: %.c

# Don't delete object and assembly files
.PRECIOUS: $(BUILD_DIR)/%.o $(BUILD_DIR)/%.s

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Custom rules
$(BUILD_DIR)/%.s: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC65) $(CFLAGS) -o $@ $<

$(BUILD_DIR)/%.o: $(BUILD_DIR)/%.s
	$(CA65) $(AFLAGS) $<

$(BUILD_DIR)/$(TARGET): $(OBJFILES)
	$(LD65) -o $@ $(LDFLAGS) $^ $(CC65_LIB)/$(PLATFORM).lib

clean:
	$(RM) -r $(BUILD_DIR)/*

lint:
	$(CLANG_TIDY) --config-file .clang-tidy --extra-arg=-std=c99 --extra-arg=-D__CC65__ $(SOURCES) $(HEADERS)
	$(CLANG_TIDY) --config-file .clang-tidy -p build-native-release $(SOURCES) $(HEADERS)

format:
	$(LLVM_BIN_PATH)/clang-format -i $(SOURCES) $(HEADERS)
