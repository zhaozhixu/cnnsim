.SUFFIXES:

SRCDIR = src
TESTDIR = test

.PHONY: all cnnsim test clean info

all: cnnsim

cnnsim:
	@(cd $(SRCDIR) && make)

test:
	@(cd $(TESTDIR) && make)

clean:
	@(cd $(SRCDIR) && make clean);\
	(cd $(TESTDIR) && make clean)

info:
	@echo "Available make targets:"
	@echo "  all: make executable binary"
	@echo "  cnnsim: make executable binary"
	@echo "  test: make tests"
	@echo "  clean: clean all object files"
	@echo "  info: show this infomation"
