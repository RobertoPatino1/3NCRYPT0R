EXEC = secret
DEPS = blowfish.h

# Target to compile final exec file
$(EXEC): main.o blowfish.o $(DEPS)
        gcc -static  -o $@ main.o blowfish.o $(DFLAGS) libsha1.a

%.o: %.c $(DEPS)
        gcc -c $< $(DFLAGS)

.PHONY: sanitize debug clean
# Compiles using -g for debugging purpuses
debug: DFLAGS = -g
debug: clean $(EXEC)

sanitize: DFLAGS = -fsanitize=address,undefined
sanitize: clean $(EXEC)

clean:
        rm -rf $(EXEC) *.o *.enc *.dec
        rm -rf archivos_*
