# === Tools ===
CC = powerpc-eabi-gcc
LD = powerpc-eabi-ld

# === Paths ===
HOSHIDIR  := externals/hoshi
SRCDIR    := src
LIBDIR    := $(HOSHIDIR)/Lib
BUILDDIR  := build
BINDIR    := out
PACKDIR   := $(HOSHIDIR)/packtool

# === Compiler Flags ===
INCLUDES := -I$(HOSHIDIR)/include -I$(LIBDIR) -I$(SRCDIR)

CFLAGS := -O1 -mcpu=750 -meabi -msdata=none -mhard-float -ffreestanding \
          -fno-unwind-tables -fno-exceptions -fno-asynchronous-unwind-tables \
          -fno-merge-constants -ffunction-sections -fdata-sections \
          $(INCLUDES)

LDFLAGS := -r -T$(PACKDIR)/link.ld

# === Output ===
MODTYPE    := gbFunction
LINKED_O   := $(BUILDDIR)/linked.o
TARGET_BIN := $(BINDIR)/mod_template.bin

# === Source Discovery ===
SRC_C  := $(shell find $(SRCDIR) -type f -name '*.c')
SRC_S  := $(shell find $(SRCDIR) -type f \( -name '*.s' -o -name '*.S' \))
LIB_C  := $(shell find $(LIBDIR) -type f -name '*.c')
LIB_S  := $(shell find $(LIBDIR) -type f \( -name '*.s' -o -name '*.S' \))

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
	python $(PACKDIR)/main.py $< -m $(MODTYPE) -o $@

# === Clean ===
clean:
	rm -rf $(BUILDDIR) $(BINDIR)

# === Debug Target (optional) ===
print-debug:
	@echo "SRC_C = $(SRC_C)"
	@echo "LIB_C = $(LIB_C)"
	@echo "SRC_S = $(SRC_S)"
	@echo "LIB_S = $(LIB_S)"
	@echo
	@echo "OBJECTS = $(OBJECTS)"

.PHONY: all clean print-debug
