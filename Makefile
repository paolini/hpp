CC_OPT = ${CFLAGS}

DEST_DIR = /usr/local/bin

VERSION = "1.2"

TAR_FILES = hpp.C hpp.h usage.html.it index.html.it .hpp default.hpp usage.hpp index.hpp gif.h gif.C jpeg.C jpeg.h README

all: hpp html

html:: index.html usage.html

index.html: index.it.html
	ln -sf index.it.html $@

usage.html: usage.it.html
	ln -sf usage.it.html $@

index.it.html: index.hpp default.hpp
	./hpp index.hpp

usage.it.html: usage.hpp default.hpp
	./hpp usage.hpp

gif.o: gif.C gif.h
	g++ -c ${CC_OPT} gif.C

jpeg.o: jpeg.C jpeg.h
	g++ -c ${CC_OPT} jpeg.C

hpp: hpp.o gif.o jpeg.o
	g++ -O2 hpp.o gif.o jpeg.o -o $@

hpp.o: hpp.h hpp.C
	g++ -c ${CC_OPT} hpp.C 

install: hpp
	install ${INSTALL_PROGRAM} hpp ${DEST_DIR}

tar: clear
	tar czf ../hpp-$(VERSION).tgz -C.. hpp-$(VERSION)
	@echo tar written on file ../hpp-$(VERSION).tgz

#tar: hpp-1.0.tgz

#hpp-1.0.tgz: ${TAR_FILES}
#	tar -czhf $@ ${TAR_FILES}

clean:
	rm -fr *.o *~ hpp

clear: clean
