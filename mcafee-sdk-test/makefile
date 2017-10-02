# 
# simple make file to build the binary
#

# Source directory name.
SRCDIR = ./
LIBS = $(SRCDIR)/lib/*.a

# Build directory absolute path.
BINDIR = ./


#.PHONY: 

.DEFAULT_GOAL := all

all: binary

sourcFiles := $(SRCDIR)/*.cpp

binary: $(sourcFiles) $(SRCDIR)/*.h
	g++ $(sourcFiles) -std=c++11 -Wall -g -lpthread -ldl -lrt -lssl -lcrypto $(LIBS) -o mcafee-test-app

clean:
	rm mcafee-test-app

