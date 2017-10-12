# 
# simple make file to build the binary
#

# -Wno-write-strings

# Source directory name.
SRCDIR = ./
STATIC_LIBS = $(SRCDIR)/lib/*.a

# Build directory absolute path.
BINDIR = ./

# g++ options
CPPFLAGS += -std=c++11 -Wall -Wno-write-strings -g -lpthread

DYNAMIC_LIBS = -ldl -lrt -lssl -lcrypto

#.PHONY: 

.DEFAULT_GOAL := all

all: binary

sourcFiles := $(SRCDIR)/*.cpp

binary: $(sourcFiles) $(SRCDIR)/*.h
	g++ $(sourcFiles) $(CPPFLAGS) $(DYNAMIC_LIBS) $(STATIC_LIBS) -o mcafee-test-app

clean:
	rm mcafee-test-app

