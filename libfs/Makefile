# Target library
lib     := libfs.a
objs    := fs.o disk.o
CC      := gcc
AR      := ar
CFLAGS  := -Wall -Wextra -MMD
AFLAGS  := rcs

all: $(lib)

ifneq ($(V),1)
Q = @
endif

all: $(lib)
deps := $(patsubst %.o,%.d,$(objs))
-include $(deps)

# Should run ar rcs libfs.a fs.o
$(lib): $(objs)
	@echo "AR       $@"
	$(Q)$(AR) $(AFLAGS) $@ $^

%.o: %.c
	@echo "CC       $@"
	$(Q)$(CC) $(CFLAGS) -c -o $@ $<

clean:
	@echo "CLEAN"
	$(Q)rm -f $(lib) $(objs) $(deps)

## TODO: Phase 1
