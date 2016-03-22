# plant
#
SRCS_$(d) :=
SRCS_$(d) += main.c
SRCS_$(d) += gid.c
SRCS_$(d) += mode.c
SRCS_$(d) += uid.c

OBJS_$(d) := $(SRCS_$(d):%.c=%.o)
DEPS_$(d) := $(OBJS_$(d):%.o=%.d)

TARGET_$(d) := plant

plant: LIBS := $(LIBS_$(d))
plant: $(OBJS_$(d))
	$(LD) $(LDFLAGS) -o $@ $^ $(LIBS)

$(OBJS_$(d)): module.mk Makefile

CLEAN_FILES += $(OBJS_$(d)) $(DEPS_$(d))
NUKE_FILES += $(TARGET_$(d))

-include $(DEPS_$(d))
