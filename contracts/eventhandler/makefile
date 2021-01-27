CC=eosio-cpp
CFLAGS=-I. -I../../include --abigen
SOURCES := $(shell find . -name '*.cpp')
BUILDDIR=build
CONTRACTNAME=eventhandler

# https://stackoverflow.com/a/18137056/2274525
MKFILE_PATH := $(abspath $(lastword $(MAKEFILE_LIST)))
CURRENT_DIR := $(patsubst %/,%,$(dir $(MKFILE_PATH)))

build:
	mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) -o $(BUILDDIR)/$(CONTRACTNAME).wasm $(SOURCES)

deploy: build
	cleos set contract $(CONTRACTNAME) $(CURRENT_DIR)/$(BUILDDIR) $(CONTRACTNAME).wasm $(CONTRACTNAME).abi -p $(CONTRACTNAME)@active

clean:
	rm -fr $(BUILDDIR)

