#include <stdio.h>
#include <stdlib.h>

void print_digit(int digit);

void
print_digit(int num)
{
	if (!(num == 1 || num == 4)) {
		printf(" - ");
	}

	printf("\n");

	if (num == 0 || num == 4 || num >= 7) {
		printf("| |");
	} else if (num == 5 || num == 6) {
		printf("|  ");
	} else if (num >= 1 && num <= 3) {
		printf("  |");
	}

	printf("\n");

	if (!(num == 0 || num == 1 || num == 7)) {
		printf(" - ");
	}

	printf("\n");

	if (num == 0 || num == 6 || num == 8) {
		printf("| |");
	} else if (num == 2) {
		printf("|  ");
	} else {
		printf("  |");
	}

	printf("\n");

	if (!(num == 1 || num == 4 || num == 7)) {
		printf(" - ");		
	}

	printf("\n");

	return;
}

/* main for testing */
int
main(int argc, char *argv[])
{
	if (argc < 2) return 1;

	print_digit(atoi(argv[1]));

	return 0;
}

