raiipp: raiipp.flex.c raiipp.bison.h raiipp.bison.c
	$(CC) -ggdb3 -o raiipp raiipp.flex.c raiipp.bison.h raiipp.bison.c -ll

raiipp.flex.c: raiipp.lex raiipp.bison.h
	flex -o raiipp.flex.c raiipp.lex

raiipp.bison.h: raiipp.y
	bison -d -o raiipp.bison.c raiipp.y

raiipp.bison.c: raiipp.y
	bison -d -o raiipp.bison.c raiipp.y

clean:
	rm -f raiipp.bison.* raiipp.flex.* *~ raiipp
