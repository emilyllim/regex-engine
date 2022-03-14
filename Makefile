.SUFFIXES: .c .o
CC = gcc
CCFLAGS = -g -pedantic -Wall
EXEC = main
OBJS = main.o nfa.o

${EXEC}: ${OBJS}
	${CC} ${CCFLAGS} -o ${EXEC} ${OBJS}

.c.o:
	${CC} ${CCFLAGS} -c $<

run: ${EXEC}
	./${EXEC}

clean:
	rm -f ${EXEC} ${OBJS}

main.o: main.c nfa.h
nfa.o: nfa.c nfa.h