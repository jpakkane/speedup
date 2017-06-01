
CPPFLAGS += -O3
LDLIBS += -lstdc++

main: main.o speedup.o

all: main

clean:
	rm main.o main speedup.o

.PHONY: all clean
