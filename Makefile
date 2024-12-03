# Compilation flags
CXXFLAGS = -g3 -Wall -fmessage-length=0 #-Werror

# Object files (add any new object files here)
OBJS = wordshooter.o util.o #Board.o util.o #game.o

# Libraries to link
LIBS = -L/usr/X11R6/lib -lm -lpthread -L/usr/X11R6/lib -L/sw/lib -L/usr/sww/lib -L/usr/sww/bin -L/usr/sww/pkg/Mesa/lib -lglut -lGLU -lGL -lSOIL -lX11 -lfreeimage -lSDL2 -lSDL2_mixer -lsndfile

# Output target
TARGET = word-shooter

# Build the target
$(TARGET): $(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS)

# Default target to build and run the program
all: $(TARGET) run

# Run the built executable
run: ./$(TARGET)

# Clean up object files and target
clean:
	rm -f $(OBJS) $(TARGET)
