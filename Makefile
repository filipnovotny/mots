CC = gcc
CFLAGS = -Wall
DEPS = defines.h helpers.h
OBJ = main.o helpers.o defines.o

%.o: %.cpp $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

mots: $(OBJ)
	gcc $(CFLAGS) -o $@ $^ -lstdc++ -lm

clean: 
	rm -f *.o mots
