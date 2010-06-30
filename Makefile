CXXFLAGS := -Wall -MD -ggdb $(shell sdl-config --cflags)
LDFLAGS := $(shell sdl-config --libs)

game: main.o transform.o canvas.o
	$(LINK.cc) $^ -o $@

clean:
	rm -f *.o *.d game

-include *.d
