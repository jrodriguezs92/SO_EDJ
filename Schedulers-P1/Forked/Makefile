
all: src

lib:
		$(MAKE) -C lib
src: lib	
		$(MAKE) -C src

clean:
		$(MAKE) -C lib clean
		$(MAKE) -C src clean
		clear

.PHONY: all lib src clean