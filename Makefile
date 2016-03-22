#

DEFS :=
WARNS := -Wall

INCS :=
OPTS := -O2

CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)gcc
AR = $(CROSS_COMPILE)ar
OBJCOPY = $(CROSS_COMPILE)objcopy

CFLAGS = $(CFLAGS_$(CROSS_COMPILE)) -MD $(INCS) $(OPTS) $(DEFS) $(WARNS)
ASFLAGS = $(ASFLAGS_$(CROSS_COMPILE)) $(INCS) $(DEFS)
LDFLAGS = $(LDFLAGS_$(CROSS_COMPILE))

CLEAN_FILES :=
NUKE_FILES :=

# ------------------------------------------------

d := .
include module.mk
d :=

# ------------------------------------------------

all: $(TARGET_$(d))

clean:
	$(RM) $(CLEAN_FILES)


nuke: clean
	$(RM) $(NUKE_FILES)
