PRODUCT=IcpTest

CFLAGS= -pedantic -ggdb -O0 -std=c++11

SOURCES=$(wildcard *.cpp)
        

HEADERS=$(wildcard *.h)
        
EXTRA_LIBS=-pthread -lSDL2 

CC=g++
CCC=arm-linux-gnueabihf-g++

all: $(PRODUCT)

$(PRODUCT): $(SOURCES) $(HEADERS) Makefile
	@$(CC) $(CFLAGS) $(SOURCES) -o  $@ $(EXTRA_LIBS)

clean:
	@rm -rf $(PRODUCT)

test: $(PRODUCT)
	@./$(PRODUCT)

pi: $(PRODUCT)
	@$(CCC) $(CFLAGS) $(SOURCES) -o  $@ $(EXTRA_LIBS)
