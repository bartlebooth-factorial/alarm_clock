#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int increment_time(int time);
void display_time_with_meridiem(int time);
void display_time_without_meridiem(int time);

int
increment_time(int time)
{
	if (time == 86399)
		return 0;
	return time + 1;
}

void
display_time_with_meridiem(int time)
{
	int hours = time / (60 * 60);
	int minutes = (time / 60) % 60;
	int seconds = time % 60;
	char meridiem;

	if (hours > 12) {
		meridiem = 'P';
		hours -= 12;
	} else {
		meridiem = 'A';
	}

	printf("%0.2d:%0.2d:%0.2d %cM\n", hours, minutes, seconds, meridiem);
}

void
display_time_without_meridiem(int time)
{
	int hours = time / (60 * 60);
	int minutes = (time / 60) % 60;
	int seconds = time % 60;

	printf("%0.2d:%0.2d:%0.2d\n", hours, minutes, seconds);
}

int
main(int argc, char *argv[])
{
	int time = 85400; // 24hr time in seconds, max val = 86400

	while (1) {
		display_time_with_meridiem(time);
		time = increment_time(time);
		sleep(1); // pause for 1 second
	}

	return 0;
}

