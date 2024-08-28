# $Id$

CC = x86_64-w64-mingw32-gcc
CFLAGS = -I $(PWD)/raylib/include -O2
LDFLAGS = -L $(PWD)/raylib/lib
LIBS = -lraylib
EXEC = .exe
