ifeq ($(strip $(DEVKITPPC)),)
$(error "Please set DEVKITPPC in your environment.")
endif

# === Tools ===
CC = $(DEVKITPPC)/bin/powerpc-eabi-gcc
LD = $(DEVKITPPC)/bin/powerpc-eabi-ld
PYTHON 	:= python

# === Paths ===
HOSHIDIR  := externals/hoshi
SRCDIR    := examples/basics/src
LIBDIR    := $(HOSHIDIR)/Lib
BUILDDIR  := build
BINDIR    := out
PACKDIR   := $(HOSHIDIR)/packtool

# === Compiler Flags ===
INCLUDES := -I$(HOSHIDIR)/include -I$(LIBDIR) -I$(SRCDIR)

CFLAGS := -O1 -mcpu=750 -meabi -msdata=none -mhard-float -ffreestanding \
          -fno-unwind-tables -fno-exceptions -fno-asynchronous-unwind-tables \
          -fno-merge-constants -ffunction-sections -fdata-sections -MMD -MP \
          $(INCLUDES)

LDFLAGS := -r -T$(PACKDIR)/link.ld

# === Output ===
MODTYPE    := gbFunction
LINKED_O   := $(BUILDDIR)/linked.o
TARGET_BIN := $(BINDIR)/mod_template.bin

# === Source Discovery ===
SRC_C := $(wildcard $(SRCDIR)/**/*.c)
LIB_C := $(wildcard $(LIBDIR)/**/*.c)
SRC_S := $(wildcard $(SRCDIR)/**/*.[sS])
LIB_S := $(wildcard $(LIBDIR)/**/*.[sS])

ALL_C := $(SRC_C) $(LIB_C)
ALL_S := $(SRC_S) $(LIB_S)
ALL_SOURCES := $(ALL_C) $(ALL_S)

# === Object Mapping ===
OBJ_C := $(patsubst %.c,$(BUILDDIR)/%.o,$(ALL_C))
OBJ_S := $(patsubst %.s,$(BUILDDIR)/%.o,$(patsubst %.S,$(BUILDDIR)/%.o,$(ALL_S)))
OBJECTS := $(OBJ_C) $(OBJ_S)

# === Default Target ===
all: $(TARGET_BIN)

# === Compile Rules ===
$(BUILDDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILDDIR)/%.o: %.s
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILDDIR)/%.o: %.S
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# === Link All to Relocatable Object ===
$(LINKED_O): $(OBJECTS)
	@mkdir -p $(dir $@)
	$(LD) $(LDFLAGS) -o $@ $^

# === Pack Final Binary ===
$(TARGET_BIN): $(LINKED_O)
	@mkdir -p $(BINDIR)
	$(PYTHON) $(PACKDIR)/main.py $< -m $(MODTYPE) -o $@

# === Clean ===
clean:
	rm -rf $(BUILDDIR) $(BINDIR)

-include $(OBJECTS:.o=.d)

# === Debug Target (optional) ===
print-debug:
	@echo "SRC_C = $(SRC_C)"
	@echo "LIB_C = $(LIB_C)"
	@echo "SRC_S = $(SRC_S)"
	@echo "LIB_S = $(LIB_S)"
	@echo
	@echo "OBJECTS = $(OBJECTS)"

.PHONY: all clean print-debug
