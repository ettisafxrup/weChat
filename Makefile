BUILD_FILE = build.log
VERSION = 1
SUB_VERSION = $(shell cat $(BUILD_FILE) 2>/dev/null || echo 0)
RELEASE_DIR = release/weChat_v$(VERSION).$(SUB_VERSION)
EXE = $(RELEASE_DIR)/weChat.exe

all: $(EXE)
	@echo $$(expr $$(cat $(BUILD_FILE) 2>/dev/null || echo 0) + 1) > $(BUILD_FILE)

$(EXE): object/main.o | $(RELEASE_DIR)
	g++ object/main.o -o $@

object/main.o: main.cpp | object/
	g++ -c main.cpp -Iinclude -o object/main.o

object/:
	mkdir -p object/

$(RELEASE_DIR):
	mkdir -p $@

clean:
	rm -rf object/ release/