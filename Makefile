# Based on tutorial at http://www.cs.colby.edu/maxwell/courses/tutorials/maketutor/
# also on this post https://stackoverflow.com/questions/5950395/makefile-to-compile-multiple-c-programs

IDIR = ./include
SOURCE_DIR= ./src
BIN_DIR= ./bin
CC=clang++
CFLAGS=-std=c++11 -I$(IDIR) 

CURL_LIB=-L/opt/local/lib -lcurl -lidn -lssl -lcrypto -lssl -lcrypto -lz

all: smite_scraper

smite_scraper: main.o
	$(CC) -o $@ $< $(CFLAGS) $(CURL_LIB)

main.o: 
	$(CC) -c $(SOURCE_DIR)/main.cpp $(CFLAGS) 

clean:
	rm *.o
	rm tmp.txt

#************************************************************************
#*                         TESTS
#************************************************************************


