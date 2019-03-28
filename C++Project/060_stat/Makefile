CFLAGS=-std=gnu99 -pedantic -Wall
mystat:mystat.o
	gcc -o mystat mystat.o
%.o: %.c
	gcc $(CFLAGS) -c $<
.PHONY: clean
clean:
	rm -f mystat *.o *.c~ *.h~
