DO	!= echo -n > deps.mk

DEBUGFLAGS	!= [ $(RELEASE) ] && echo "-O2 -DNODEBUG" || echo "-O0 -g -DDEBUG"
CFLAGS		= -Wall -Wextra
DEPFLAGS	= -MT $@ -MMD -MP -MF $@.d
INCLUDEFLAGS	= -Isrc
COMPILEFLAGS	=
LINKFLAGS	= -lm

all: exgt

# default values
CROSS_COMPILE	?=

# common programs
CC		= gcc

SOURCES		:=

include src/source.mk

COMPILE		= $(CROSS_COMPILE)$(CC) $(DEBUGFLAGS)\
		  $(CFLAGS) $(DEPFLAGS) $(COMPILEFLAGS) $(INCLUDEFLAGS)

OBJS		!= ./scripts/gen-deps --sources "$(SOURCES)"

include deps.mk

.PHONY: format
format:
	find . -iname '*.[ch]' |\
		xargs -n 10 -P 0 uncrustify -c uncrustify.conf --no-backup -F -

.PHONY: license
license:
	./scripts/license

.PHONY: docs
docs:
	./scripts/warn-undocumented
	doxygen docs/doxygen.conf

exgt: $(OBJS)
	$(COMPILE) $(OBJS) -o $@

.PHONY: clean
clean:
	$(RM) -r build exgt deps.mk

.PHONY: clean_docs
clean_docs:
	$(RM) -r docs/output
