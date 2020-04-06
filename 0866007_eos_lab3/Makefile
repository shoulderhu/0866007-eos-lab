CXX = arm-unknown-linux-gnu-g++
CFLAGS = -g -Wall

LINK = /opt/arm-unknown-linux-gnu/arm-unknown-linux-gnu/lib/
INCLUDE = /opt/arm-unknown-linux-gnu/arm-unknown-linux-gnu/include/
INCLUDE2 = /home/lab616/microtime/linux/include/

all: lab3

%.o: %.c
	$(CXX) -c $< -L $(LINK) -I $(INCLUDE) -I $(INCLUDE2) $(CFLAGS)

%.o: %.cpp
	$(CXX) -c $< -L $(LINK) -I $(INCLUDE) -I $(INCLUDE2) $(CFLAGS)

lab3: lab3.o
	$(CXX) -o $@ $^ $(CFLAGS)

clean:
	rm -rf lab3 lab3.o

.PHONY: all clean