CC=x86_64-linux-musl-gcc

CPPFLAGS=-std=c++14
CFLAGS=-I .
LDLIBS=-static -lstdc++

ODIR=out/objs

DEPS=$(wildcard *.hpp)
_OBJS=main.o $(subst .hpp,.o,$(DEPS))
OBJS=$(_OBJS:%=$(ODIR)/%)

all: main

main: $(OBJS)
	$(CC) -o out/futoshiki-solver $^ $(LDLIBS)

$(ODIR)/%.o: %.cpp $(DEPS)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $< $(LDLIBS)

clean:
	rm -f $(OBJS)
	rm -f out/futoshiki-solver
