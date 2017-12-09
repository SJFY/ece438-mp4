FLAGS = -W -Wall
#CC = g++ -std=c++0x -g
CC = g++ -std=c++11 -g

LINKLIBS = -lm

# all: linkstate distvec
all: csma

.PHONY: all clean

csma: csma.cpp
	$(CC) csma.cpp -o csma $(LINKLIBS)

# distvec: distvec.cpp
# 	$(CC) distvec.cpp -o distvec $(LINKLIBS)

clean:
	$(RM) *.o csma output.txt transmissions.txt nodes.txt
	# $(RM) *.o linkstate distvec
	
