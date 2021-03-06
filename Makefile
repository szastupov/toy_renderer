SDL_CFLAGS := $(shell pkg-config sdl --cflags)
SDL_LIBS := $(shell pkg-config sdl --libs)

CFLAGS += $(SDL_CFLAGS) -Wall -MD -ggdb
LDLIBS += $(SDL_LIBS)

CXXFLAGS += $(CFLAGS)

all: demo

demo: main.o transform.o canvas.o
	$(LINK.cc) $^ $(LOADLIBES) $(LDLIBS) -o $@

clean:
	rm -f *.o *.d demo

-include *.d
