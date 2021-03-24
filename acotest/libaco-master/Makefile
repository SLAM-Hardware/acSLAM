CC 			= gcc
INSTALL		= /usr/bin/install
LIB_SHARED  = libaco.so
LIB_DIR		= /usr/local/lib
SOURCE 		= aco.c acosw.S
all:$(LIB_SHARED)

$(LIB_SHARED):$(SOURCE)
	$(CC) -g -O3 $(SOURCE) -fPIC -shared -o $(LIB_SHARED)

install:$(LIB_SHARED)
	$(INSTALL) -c -m 755 $(LIB_SHARED)  $(LIB_DIR)
	
clean:
	rm -rf libaco.so
