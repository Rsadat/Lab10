OPTS=-fno-pie -no-pie -fno-builtin -Wall -Wextra -Wno-unused-parameter -Wno-unused-variable -Wno-unused-but-set-variable -Werror -std=c17 -Wpedantic -O0 -g


all: lab10


lab10: lab10.o main.o
	gcc $^ -o $@ $(OPTS)


lab10.o: lab10.c lab10.h
	gcc $< -c -o $@ $(OPTS)


main.o: main.c lab10.h
	gcc $< -c -o $@ $(OPTS)


clean:
	rm -f *.o lab10
