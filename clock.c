#include <curses.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int increment_time(int time);
int ensure_valid_time(int value);
void display_time_with_meridiem(int time, bool alarm_on, bool snooze_on);
void display_time_without_meridiem(int time, bool alarm_on, bool snooze_on);
void display_alarm(int alarm_time, bool meridiem, bool snoozed);
void display_setting_buffer(int buffer, bool meridiem);

int
increment_time(int time)
{
	if (time == 86399)
		return 0;
	return time + 1;
}

int
ensure_valid_time(int value)
{
	if (value < 0) {
		return value + 86400;
	} else if (value >= 86400) {
		return value - 86400;
	} else {
		return value;
	}
}

void
display_time_with_meridiem(int time, bool alarm_on, bool snooze_on)
{
	int hours = time / (60 * 60);
	int minutes = (time / 60) % 60;
	int seconds = time % 60;
	char meridiem;
	char alarm_status;
	char snooze_status;

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

	if (snooze_on) {
		snooze_status = 'z';
	} else {
		snooze_status = ' ';
	}

	mvprintw(10, 10, "%0.2d:%0.2d:%0.2d %cM (%c) [%c]\n", hours, minutes, seconds, meridiem, alarm_status, snooze_status);
}

void
display_time_without_meridiem(int time, bool alarm_on, bool snooze_on)
{
	int hours = time / (60 * 60);
	int minutes = (time / 60) % 60;
	int seconds = time % 60;
	char alarm_status;
	char snooze_status;

	if (alarm_on) {
		alarm_status = 'A';
	} else {
		alarm_status = 'V';
	}

	if (snooze_on) {
		snooze_status = 'z';
	} else {
		snooze_status = ' ';
	}

	
	mvprintw(10, 10, "%0.2d:%0.2d:%0.2d (%c) [%c]\n", hours, minutes, seconds, alarm_status, snooze_status);
}

void
display_alarm(int alarm_time, bool meridiem, bool snoozed)
{
	attron(A_BLINK);

	if (meridiem) {
		display_time_with_meridiem(alarm_time, true, snoozed);
	} else {
		display_time_without_meridiem(alarm_time, true, snoozed);
	}

	attroff(A_BLINK);
}

void
display_setting_buffer(int buffer, bool meridiem)
{
	int hours = buffer / (60 * 60);
	int minutes = (buffer / 60) % 60;

	if (meridiem) {
		char meridiem;
		if (hours > 12) {
			meridiem = 'P';
			hours -= 12;
		} else {
			meridiem = 'A';
		}
		mvprintw(20, 10, "%0.2d:%0.2d:00 %cM\n", hours, minutes, meridiem);
	} else {
		mvprintw(20, 10, "%0.2d:%0.2d:00\n", hours, minutes);
	}
}

int
main(int argc, char *argv[])
{
	bool power_on = true; // boolean representing power status of clock.
	                      // if false, main loop does nothing but wait for quit or power-toggle input
		
	int time = 85400; // 24hr time in seconds, max val = 86400


	bool meridiem = false; // if true, display time with meridiem info (AM/PM)

	bool alarm_on = true; // booolean to determine if alarm will trigger
	int alarm_time = 85410; // time value at which alarm triggers if alarm_on is true
	bool alarm_triggered = false; // bool to determine if alarm has been triggered
	bool snooze_on = true; // if true, <space> will snooze a triggered alarm
	bool snoozed = false; // true if an alarm has been snoozed and a following alarm is pending
	int next_snooze_alarm_time; // will equal previous snoozed alarm time + snooze_step minutes,
	                            // used to keep track of when a snoozed alarm needs to retrigger
	                            // without altering the set alarm_time
	int snooze_step = 5; // number of minutes later a snoozed alarm will re-trigger

	char is_setting = 'n'; // char to store whether the user is setting time ('t') or alarm_time ('a'),
	                       // or not setting anything ('n')
	int time_set_buffer = 0; // a 24hr timeval in seconds, which user can change
	                         // by hours or minutes while setting time
	int alarm_set_buffer = 0; // a 24hr timeval in seconds, which user can change
	                          // by hours or minutes while setting time

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
		if (power_on) {
			clear();

			if (alarm_triggered) {
				if (snoozed) {
					display_alarm(next_snooze_alarm_time, meridiem, snoozed);
				} else {
					display_alarm(alarm_time, meridiem, snoozed);
				}
			} else {
				if (meridiem) {
					display_time_with_meridiem(time, alarm_on, snooze_on);
				} else {
					display_time_without_meridiem(time, alarm_on, snooze_on);
				}
			}

			if (is_setting == 'a') {
				if (meridiem) {
					display_setting_buffer(alarm_set_buffer, true);
				} else {
					display_setting_buffer(alarm_set_buffer, false);
				}
			} else if (is_setting == 't') {
				if (meridiem) {
					display_setting_buffer(time_set_buffer, true);
				} else {
					display_setting_buffer(time_set_buffer, false);
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
			} else if (snoozed && time == next_snooze_alarm_time) {
				alarm_triggered = true;
			}

			key = getch();

			if (key == 113) { // q: quit
				goto EXIT;
			} else if (key == 111) { // o: toggle power
				clear();
				refresh();
				power_on = false;
			} else if (key == 109) { // m: toggle meridiem
				if (meridiem == false) {
					meridiem = true;
				} else {
					meridiem = false;
				}
			} else if (key == 97) { // a: toggle alarm_on
				if (alarm_on) {
					alarm_on = false;
					alarm_triggered = false;
					if (snoozed) {
						snoozed = false;
					}
				} else {
					alarm_on = true;
				}
			} else if (key == 122) { // z: toggle snooze_on
				if (snooze_on) {
					snooze_on = false;
				} else {
					snooze_on = true;
				}
			} else if (key == 32) { // space: silence or snooze a triggered alarm
				if (alarm_triggered) {
					alarm_triggered = false;
					if (snooze_on) {
						if (snoozed) {
							next_snooze_alarm_time += (60 * snooze_step);
						} else {
							next_snooze_alarm_time = alarm_time + (60 * snooze_step);
							snoozed = true;
						}
					}
				}
			} else if (key == 115) { // s: set alarm
				is_setting = 'a';
			} else if (key == 83) { // S: set time
				is_setting = 't';
			} else if (key == 100) { // d: decrease time being set by one minute
				if (is_setting == 'a') {
					alarm_set_buffer -= 60;
				} else if (is_setting == 't') {
					time_set_buffer -= 60;
				}
			} else if (key == 105) { // i: increase time being set by one minute
				if (is_setting == 'a') {
					alarm_set_buffer += 60;
				} else if (is_setting == 't') {
					time_set_buffer += 60;
				}
			} else if (key == 68) { // D: decrease time being set by one hour
				if (is_setting == 'a') {
					alarm_set_buffer -= 3600;
				} else if (is_setting == 't') {
					time_set_buffer -= 3600;
				}
			} else if (key == 73) { // I: increase time being set by one hour
				if (is_setting == 'a') {
					alarm_set_buffer += 3600;
				} else if (is_setting == 't') {
					time_set_buffer += 3600;
				}
			} else if (key == 102) { // f: finish setting time (set time will take effect)
				if (is_setting == 'a') {
					alarm_time = alarm_set_buffer;
					if (alarm_triggered) {
						alarm_triggered = false;
					}
					is_setting = 'n';
				} else if (is_setting == 't') {
					time = time_set_buffer;
					is_setting = 'n';
				}
			}

			// if user is currently setting a time, make sure their buffer is a valid time
			// (not negative and not above 86400)
			if (is_setting == 'a') {
				alarm_set_buffer = ensure_valid_time(alarm_set_buffer);
			} else if (is_setting == 't') {
				time_set_buffer = ensure_valid_time(time_set_buffer);
			}

		} else {
			key = getch();

			if (key == 113) { // q: quit
				goto EXIT;
			} else if (key == 111) { // o: toggle power
				power_on = true;
			}
		}

		usleep(1e5); // pause for 1/10 second
	}

EXIT:
	endwin();

	return 0;
}

