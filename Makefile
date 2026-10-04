CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -Wpedantic -g

SOURCES := src/lunar.c examples/basic.c
OBJECTS := build/lunar.o build/basic.o

.PHONY: all check objects clean help
.DELETE_ON_ERROR:

# Will keep the initial scaffold usable before the database functions can be linked
all: check

check:
    $(CC) $(CPPFLAGS) -Iinclude $(CFLAGS) -Werror -fsyntax-only $(SOURCES)

objects: $(OBJECTS)

build/lunar.o: src/lunar.c include/lunar.h | build
    $(CC) $(CPPFLAGS) -Iinclude $(CFLAGS) -c $< -o $@

build/basic.o: examples/basic.c include/lunar.h | build
    $(CC) $(CPPFLAGS) -Iinclude $(CFLAGS) -c $< -o $@

build:
    mkdir -p $@

clean:
    $(RM) $(OBJECTS)

help:
    @printf '%s\n' \
      'make          Check C syntax and treat warnings as errors.' \
      'make objects  Compile object files into build/ without linking.' \
      'make clean    Remove generated object files.' \
      'make help     Show these targets.' \
      'Override CC, CPPFLAGS, or CFLAGS on the command line.' \
      'After changing flags, run make clean before make objects.'
