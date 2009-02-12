
OPTS = -c -O2 -Wall -Wno-non-virtual-dtor

CC = g++

GL_INC = -I/usr/local/glew/include
GL_LIBS = -lglut -lGLEW -lGLU  -lGL -lXmu -lXext -lX11 -lm -lX11 -lXt -lXi -lSM -lICE -L/usr/local/glew/lib
LLDLIBS =  $(GL_LIBS) -L/usr/X11R6/lib
OBJECTS = main.o math.o scene.o vbo.o
TARGET = ../bin/grass_benchmark.linux

all: $(TARGET)

.cpp.o:
		$(CC) $(OPTS) $(GL_INC) $<

.c.o:
		$(CC) $(OPTS) $(GL_INC) $<

$(TARGET):	$(OBJECTS)
		$(CC) -o $(TARGET) $(OBJECTS) $(LLDLIBS)

clean:  
	-rm -f *.o

distclean: clean
	rm -f *~ $(TARGET)
