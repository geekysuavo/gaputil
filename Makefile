
# compiler configuration.
CC=gcc
CFLAGS=-g -O2 -fPIC -Wall -Wformat -Wno-strict-aliasing
LDLIBS=-lm
LDFLAGS=

# installer configuration.
INSTALL=install
PREFIX=/usr/local
BINDIR=$(PREFIX)/bin
MANDIR=$(PREFIX)/share/man/man1

# julia configuration.
JL_CMD='println(joinpath(JULIA_HOME,Base.DATAROOTDIR,"julia"))'
JL_SHARE=$(shell julia -e $(JL_CMD))
CFLAGS+= $(shell $(JL_SHARE)/julia-config.jl --cflags)
LDLIBS+= $(shell $(JL_SHARE)/julia-config.jl --ldlibs)
LDFLAGS+= $(shell $(JL_SHARE)/julia-config.jl --ldflags)

# binaries and objects to compile and link.
BIN=gaputil rejutil
MAN=gaputil.1 rejutil.1
OBJS=tup.o seq.o srt.o rej.o term.o qrng.o

# suffixes used in the build.
.SUFFIXES: .c .o

# all: default target.
all: check-julia $(BIN)

# gaputil: first executable linkage target.
gaputil: $(OBJS) gaputil.o
	@echo " LD $@"
	@$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

# rejutil: second executable linkage target.
rejutil: $(OBJS) rejutil.o
	@echo " LD $@"
	@$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

# .c.o: compilation target.
.c.o:
	@echo " CC $^"
	@$(CC) $(CFLAGS) -c $^ -o $@

# install: installation target.
install: all
	@echo " INSTALL $(BIN)"
	@$(INSTALL) -d $(BINDIR)
	@$(INSTALL) -d $(MANDIR)
	@$(INSTALL) $(BIN) $(BINDIR)
	@$(INSTALL) $(MAN) $(MANDIR)

# clean: built file removal target.
clean:
	@echo " CLEAN"
	@rm -f $(BIN) $(addsuffix .o,$(BIN)) $(OBJS)

# again: repeat/rebuild compilation target.
again: clean all

# check-julia: target to check that a julia distribution exists.
check-julia:
	@echo " CHECK julia"
	@julia -v >/dev/null

# fixme: no-op target to check for fixme statements.
fixme:
	@echo " FIXME"
	@grep -RHni --color fixme *.[ch1] || echo " None found"

# lines: no-op target to perform a source code line count.
lines:
	@echo " WC"
	@wc -l *.[ch1]

