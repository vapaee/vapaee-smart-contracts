CC=eosio-cpp
CFLAGS=-I. -I../../include --abigen
SOURCES := $(shell find . -name '*.cpp')
BUILDDIR=build
CONTRACTNAME=boardgamebox


build:
	mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) -o $(BUILDDIR)/$(CONTRACTNAME).wasm $(SOURCES)


clean:
	rm -fr $(BUILDDIR)

