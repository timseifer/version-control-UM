#
# Makefile for the UM Test lab
# 
CC = gcc 
#O1
#-O1 

IFLAGS  = -I/comp/40/build/include -I/usr/sup/cii40/include/cii
CFLAGS  = -g -std=gnu99 -Wall -Wextra -Werror -pedantic $(IFLAGS)
LDFLAGS = -g -L/comp/40/build/lib -L/usr/sup/cii40/lib64
LDLIBS  = -lbitpack -l40locality -lcii40 -lm  
LDLIBS_OP = -lbitpack -l40locality -lm -lcii40
EXECS   = writetests um

all: $(EXECS)

um: main.o um_instructions.o um.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS_OP)

writetests: umlabwrite.o umlab.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

# To get *any* .o file, compile its .c file with the following rule.
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(EXECS)  *.o

