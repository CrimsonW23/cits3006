CFLAGS = -g -Wall
OUT_FILE_NAME_HACKER = hacker
SRCS_HACKER = hacker.c
OUT_FILE_NAME_VICTIM = victim
SRCS_VICTIM = victim.c

CC = gcc

all: hacker victim

hacker: ${SRCS_HACKER} 
	${CC} ${CFLAGS} -o ${OUT_FILE_NAME_HACKER} ${SRCS_HACKER} 

victim: ${SRCS_VICTIM} 
	${CC} ${CFLAGS} -o ${OUT_FILE_NAME_VICTIM} ${SRCS_VICTIM} 

clean:
	rm -f *.o *.out ${OUT_FILE_NAME_HACKER} ${OUT_FILE_NAME_VICTIM}


