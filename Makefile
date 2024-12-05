CC = mpicc
SRCS = ./src/*.c
INCDIR = ./src
OPTS = -O3
EXEC = nbody

all: clean release

# Build for release
release:
	$(CC) $(SRCS) $(OPTS) -I$(INCDIR) -o $(EXEC) -lm

# Build for debugging
debug:
	$(CC) $(SRCS) -I$(INCDIR) -o $(EXEC) -lm -g

# Clean up the build
clean:
	rm -f $(EXEC)
	rm -rf nbody.dSYM
