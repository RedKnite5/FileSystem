# Target library
lib := libfs.a

all: $(lib)
# Target library
objs    := fs.o disk.o
CC      := gcc
AR      := ar
lib := libfs.a
CFLAGS  := -Wall -MMD
AFLAGS  := rcs

ifneq ($(V),1)
Q = @
endif

all: $(lib)
deps := $(patsubst %.o,%.d,$(objs))
-include $(deps)

# Should run ar rcs libuthread.a queue.o, etc
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
