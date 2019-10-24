all: first

first: first.c
	gcc -Werror -Wall -lm -fsanitize=address first.c -o first

clean:
	rm -rf first
