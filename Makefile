#
# SDCC Makefile for mcs51
#

# include path
INCLUDE :=
INCLUDE += -I./include
INCLUDE += -I./include/core
INCLUDE += -I./include/drivers

# source code(*.c) path
SRCDIR :=
SRCDIR += ./core
SRCDIR += ./drivers
SRCDIR += ./user

# out path
OUTDIR  = ./obj
HEXDIR  = ./hex

LIBDIR  =
# Target and Source
TARGET = $(HEXDIR)/mini-3216

C_SRC = $(foreach dir, $(SRCDIR), \
	$(wildcard $(dir)/*.c && $(shell mkdir -p $(OUTDIR)/$(dir))))
ASM_SRC =

C_OBJ_FILE = $(addprefix $(OUTDIR)/, $(subst ./, , $(C_SRC)))
OBJ = $(C_OBJ_FILE:%.c=%.c.rel)

# Usually SDCC's small memory model is the best choice.
# Memory Model (small, medium, large, huge)
MEM_MODEL = large

# Memory Layout
# PRG Size = 60K Bytes
CODE_SIZE = --code-loc 0x0000 --code-size 61440
# INT-MEM Size = 256 Bytes
IRAM_SIZE = --idata-loc 0x0000 --iram-size 256
# EXT-MEM Size = ~2K Bytes
XRAM_SIZE = --xram-loc 0x0000 --xram-size 1792

# SDCC
CC = sdcc
AS = sdas8051

MCU_MODEL = mcs51

# LIBS    =
# LIBPATH = -L $(LIBDIR)
# DEBUG   = --debug
AFLAGS = -l -s
CFLAGS = $(INCLUDE) -m$(MCU_MODEL) --model-$(MEM_MODEL) \
--out-fmt-ihx --fsigned-char --std-sdcc99 --opt-code-speed $(DEBUG)

LFLAGS = $(LIBPATH) $(LIBS) -m$(MCU_MODEL) --model-$(MEM_MODEL) \
$(CODE_SIZE) $(IRAM_SIZE) $(XRAM_SIZE) --out-fmt-ihx --std-sdcc99 \
 --opt-code-speed $(DEBUG)

S = @

.PHONY: all clean flash

all: $(TARGET).hex

$(TARGET).hex : $(OUTDIR)/.ihx
	$(S) packihx $^ > $@
	@echo "\nMini3216: $(TARGET).hex is ready"

$(OUTDIR)/.ihx : $(OBJ)
	@echo "  LD $(TARGET).ihx"
	$(S) $(CC) -o $@ $(LFLAGS) $^

$(OUTDIR)/%.c.rel : ./%.c
	@echo "  CC $^"
	$(S) $(CC) -o $@ $(CFLAGS) -c $^

$(OUTDIR)/%.asm.rel : ./%.asm
	$(S) $(AS) $(AFLAGS) -o $@ $^

clean:
	$(S) rm -rf $(OUTDIR)/*
	$(S) rm -rf $(TARGET).hex

flash:
	stcgal -P stc15 -o clock_source=internal -t 27000 -b 115200 -p \
	/dev/tty.wchusbserial* $(TARGET).hex
