calculator: calculator.tab.o lex.yy.o funcs.o
	cc -o calculator calculator.tab.o lex.yy.o funcs.o -lm
calculator.tab.o: calculator.tab.c
	cc -c calculator.tab.c
lex.yy.o: lex.yy.c
	cc -c lex.yy.c
funcs.o: funcs.c funcs.h
	cc -c funcs.c 
calculator.tab.c: calculator.y
	bison -d calculator.y
lex.yy.c: calculator.l
	lex calculator.l
clean:
	rm *.tab.c
	rm *.tab.h
	rm *.yy.c
	rm *.o
	rm calculator

install:
	make
	cp calculator /bin
uninstall:
	rm calculator
	rm /bin/calculator

