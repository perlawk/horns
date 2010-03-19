ifeq ("${MSYSTEM}","MINGW32")
	READLINE=
	OUT=horns.exe
else
	READLINE=-lreadline
	OUT=./horns
endif

default: horns test

horns: horns.l horns.y node.h node.c libhorns.h libhorns.c test.h test.c main.h main.c
	flex -I horns.l
	bison -d horns.y -o y.tab.c
	gcc -o $(OUT) -O2 -Wall main.c test.c lex.yy.c y.tab.c libhorns.c node.c -lfl -ly -lm $(READLINE)
	strip $(OUT)

test: horns
	$(OUT) -t

clean:
	-rm $(OUT)
	-rm y.tab.c
	-rm y.tab.h
	-rm lex.yy.c
