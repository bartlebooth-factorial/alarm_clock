#include <curses.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int increment_time(int time);
void display_time_with_meridiem(int time, bool alarm_on);
void display_time_without_meridiem(int time, bool alarm_on);
void display_alarm_with_meridiem(int alarm_time);
void display_alarm_without_meridiem(int alarm_time);

int
increment_time(int time)
{
	if (time == 86399)
		return 0;
	return time + 1;
}

void
display_time_with_meridiem(int time, bool alarm_on)
{
	int hours = time / (60 * 60);
	int minutes = (time / 60) % 60;
	int seconds = time % 60;
	char meridiem;
	char alarm_status;

	if (hours > 12) {
		meridiem = 'P';
		hours -= 12;
	} else {
		meridiem = 'A';
	}

	if (alarm_on) {
		alarm_status = 'A';
	} else {
		alarm_status = 'V';
	}

	mvprintw(10, 10, "%0.2d:%0.2d:%0.2d %cM (%c)\n", hours, minutes, seconds, meridiem, alarm_status);
}

void
display_time_without_meridiem(int time, bool alarm_on)
{
	int hours = time / (60 * 60);
	int minutes = (time / 60) % 60;
	int seconds = time % 60;
	char alarm_status;

	if (alarm_on) {
		alarm_status = 'A';
	} else {
		alarm_status = 'V';
	}
	
	mvprintw(10, 10, "%0.2d:%0.2d:%0.2d (%c)\n", hours, minutes, seconds, alarm_status);
}

void
display_alarm_with_meridiem(int alarm_time)
{
	attron(A_BLINK);
	display_time_with_meridiem(alarm_time, true);
	attroff(A_BLINK);
}

void
display_alarm_without_meridiem(int alarm_time)
{
	attron(A_BLINK);
	display_time_without_meridiem(alarm_time, true);
	attroff(A_BLINK);
}

int
main(int argc, char *argv[])
{
	int time = 85400; // 24hr time in seconds, max val = 86400

	bool meridiem = false; // if true, display time with meridiem info (AM/PM)

	bool alarm_on = true; // booolean to determine if alarm will trigger
	int alarm_time = 85410; // time value at which alarm triggers if alarm_on is true
	bool alarm_triggered = false; // bool to determine if alarm has been triggered

	WINDOW *stdscr = initscr();
	if (stdscr == NULL)
		return 1;

	noecho();
	curs_set(FALSE);
	cbreak();
	nodelay(stdscr, TRUE);
	int key;

	int run_count = 0; // counter to keep track of how many loops before 1 second has passed
	
	/* main clock loop */
	while (1) {
		clear();
		if (alarm_triggered) {
			if (meridiem) {
				display_alarm_with_meridiem(alarm_time);
			} else {
				display_alarm_without_meridiem(alarm_time);
			}
		} else {
			if (meridiem) {
				display_time_with_meridiem(time, alarm_on);
			} else {
				display_time_without_meridiem(time, alarm_on);
			}
		}
		refresh();

		if (run_count == 9) {
			time = increment_time(time);
			run_count = 0;
		} else {
			run_count++;
		}

		if (alarm_on && time == alarm_time) {
			alarm_triggered = true;
		}

		key = getch();

		if (key == 113) { // q: quit
			goto EXIT;
		} else if (key == 109) { // m: toggle meridiem
			if (meridiem == false) {
				meridiem = true;
			} else {
				meridiem = false;
			}
		} else if (key == 97) { // a: toggle alarm_on
			if (alarm_on) {
				alarm_on = false;
			} else {
				alarm_on = true;
			}
		} else if (key == 32) { // space: untrigger a triggered alarm
			if (alarm_triggered) {
				alarm_triggered = false;
			}
		}

		usleep(1e5); // pause for 1/10 second
	}

EXIT:
	endwin();

	return 0;
}

