MACHINE=-m32

ifeq ("${MSYSTEM}","MINGW32")
	READLINE=
	OUT=horns.exe
else
	READLINE=-lreadline
	OUT=./horns
endif

default: horns test

lex.yy.c: horns.l
	flex -I horns.l

horns: lex.yy.c node.h node.c libhorns.h libhorns.c test.h test.c parse.h parse.c main.c
	gcc -o $(OUT) -O2 -Wall $(MACHINE) main.c parse.c lex.yy.c test.c libhorns.c node.c -lfl -lm $(READLINE)
	strip $(OUT)

test: horns
	$(OUT) -t

clean:
	-rm $(OUT)
	-rm lex.yy.c
