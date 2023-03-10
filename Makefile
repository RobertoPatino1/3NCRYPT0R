
EXEC = 3NCRYPT0R
DEPS = resources/blowfish.h



$(EXEC): main.o resources/blowfish.o $(DEPS)
	./resources/init.sh;gcc -static  -o $@ main.o resources/blowfish.o $(DFLAGS) resources/libsha1.a

resources/%.o: %.c $(DEPS)
	gcc -c $< $(DFLAGS)

.PHONY:  sanitize debug clean  

debug: DFLAGS = -g
debug: clean $(EXEC)


sanitize: DFLAGS = -fsanitize=address,undefined
sanitize: clean $(EXEC)

clean:
	rm -rf $(EXEC) *.o *.enc *.dec
	rm -rf *_files




	
