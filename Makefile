HELP = includes/functions/help.h

crypt: main.o help.o
	gcc -o crypt main.o help.o $(HELP)

main.o: main.c $(HELP)
	gcc -c main.c 

help.o: implmt/help.c help.h
	gcc -c implmt/help.c



.PHONY: clean
clean:
	rm *.o
	rm *.out
	rm crypt
