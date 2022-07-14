#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int increment_time(int time);
void display_time_with_meridiem(int time);
void display_time_without_meridiem(int time);
void display_alarm(int alarm_time);

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

	bool meridiem = false; // if true, display time with meridiem info (AM/PM)

	bool alarm_on = true; // booolean to determine if alarm will trigger
	int alarm_time = 85410; // time value at which alarm triggers if alarm_on is true
	bool alarm_triggered = false; // bool to determine if alarm has been triggered

	/* main clock loop */
	while (1) {
		if (alarm_triggered) {
			if (meridiem) {
				display_time_with_meridiem(alarm_time);
			} else {
				display_time_without_meridiem(alarm_time);
			}
		} else {
			if (meridiem) {
				display_time_with_meridiem(time);
			} else {
				display_time_without_meridiem(time);
			}
		}

		time = increment_time(time);

		if (alarm_on && time == alarm_time) {
			alarm_triggered = true;
		}

		sleep(1); // pause for 1 second
	}

	return 0;
}

