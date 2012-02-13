#
# Makefile 
# 

CC=g++
CFLAGS=-g -Wall
PROGRAM=plm
FILES=plm.cpp isalpha.cpp pl_error.cpp

$(PROGRAM):$(FILES) 
		$(CC) $(CFLAGS) -o $(PROGRAM) $(FILES)

clean:
		rm -f $(PROGRAM)
.PHONY: clean
