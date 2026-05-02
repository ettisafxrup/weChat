# Let the .o files into a folder named object and the executable into a folder named release. Each time we compile, we will make a version of that file like weChat1.0, weChat1.1, etc. We will also add a clean command to remove all the .o files and the release folder. write a simple makefile in following this. one line makefile

BUILD_FILE = build_log.txt
VERSION = 1
SUB_VERSION = $(shell cat $(BUILD_FILE) || echo 0)

all: release/weChat_v$(VERSION).$(SUB_VERSION)
	@echo $$(expr $$(cat $(BUILD_FILE) 2>/dev/null || echo 0) + 1) > $(BUILD_FILE)

release/weChat_v$(VERSION).$(SUB_VERSION): object/main.o | release/
	g++ object/main.o -o $@.exe

object/main.o: main.cpp | object/
	g++ -c main.cpp -Iinclude -o object/main.o

object/:
	mkdir -p object/

release/:
	mkdir -p release/

clean:
	rm -rf object/ release/ 
