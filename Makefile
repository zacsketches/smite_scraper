# Based on tutorial at http://www.cs.colby.edu/maxwell/courses/tutorials/maketutor/
# also on this post https://stackoverflow.com/questions/5950395/makefile-to-compile-multiple-c-programs

IDIR = ./include
SOURCE_DIR= ./src
BIN_DIR= ./bin
TEST_DIR= ./tests
CC=clang++
CFLAGS=-std=c++11 -I$(IDIR) 

CURL_LIB=-L/opt/local/lib -lcurl -lidn -lssl -lcrypto -lssl -lcrypto -lz

all: smite_scraper

tests: cleanup

smite_scraper: main.o
	$(CC) -o $@ $< $(CFLAGS) $(CURL_LIB)

main.o: $(IDIR)/smite.h
	$(CC) -c $(SOURCE_DIR)/main.cpp $(CFLAGS) 

clean:
	rm *.o

#************************************************************************
#*                         TESTS
#************************************************************************
cleanup: cleanup.o
	$(CC) -o $(BIN_DIR)/$@ $< $(CFLAGS) $(CURL_LIB)
	
cleanup.o: $(IDIR)/smite.h
	$(CC) -c $(TEST_DIR)/cleanup.cpp $(CFLAGS) 
