# https://stackoverflow.com/a/17845120/2274525

TOPTARGETS := build clean

CONTRACTS := $(wildcard */.)

build: $(CONTRACTS)

clean: $(CONTRACTS)
	
$(CONTRACTS):
	$(MAKE) -C $@ $(MAKECMDGOALS)

.PHONY: $(TOPTARGETS) $(CONTRACTS)
	