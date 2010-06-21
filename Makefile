CXXFLAGS+=-Wall -MD -ggdb
LDFLAGS=-lGL -lGLU -lglut

game: main.o
	$(LINK.cc) $^ -o $@

clean:
	rm -f *.o *.d game

-include *.d
