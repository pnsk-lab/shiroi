# $Id: generic.mk 1 2024-08-28 08:10:28Z nishi $

CC = cc
CFLAGS = -I /usr/local/include -O2
LDFLAGS = -L /usr/local/lib
LIBS = -lraylib -lpthread -lm
