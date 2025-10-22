# SkyArchive Root Makefile
# This delegates to the src/Makefile

.PHONY: all clean run debug release install uninstall help

all:
	cd src && $(MAKE) all

clean:
	cd src && $(MAKE) clean

run:
	cd src && $(MAKE) run

debug:
	cd src && $(MAKE) debug

release:
	cd src && $(MAKE) release

install:
	cd src && $(MAKE) install

uninstall:
	cd src && $(MAKE) uninstall

help:
	cd src && $(MAKE) help
