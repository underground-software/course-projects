#include <fcntl.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define BUFFER_LENGTH	100
#define MAX_ROLL	6
#define ROLL_OFFSET	4
#define CHECK_COUNT	1000

bool check_read(int fp);
bool check_write(int fp);
bool check_ioctl(int fp, int argv, char ** argc);
bool check_ret(int ret, char* file_operation);
bool check_bounds(int roll, int start, int end);
void print_output(bool output, char* file_operation);
void correct_message(char* file_operation);
void fail_message(char* file_operation);

int main(int argv, char **argc) 
{
	int fp;

	fp = open("/dev/dice", O_RDWR);
	if (fp < 0) {
		perror("Could not open device file\n");
		exit(1);
	}

	printf("Testing file operations\n\n");
	print_output(check_read(fp), "read");
	print_output(check_write(fp), "write");
	print_output(check_ioctl(fp, argv, argc), "ioctl");

	return 0;
}

bool check_read(int fp) 
{
	char read_buffer[BUFFER_LENGTH];
	int roll;
	char write_buffer[] = { MAX_ROLL, '\0' };
	int ret = write(fp, write_buffer, sizeof(write_buffer));

	check_ret(ret, "write");
	for (int i = 0; i < CHECK_COUNT; i++) {
		ret = read(fp, read_buffer, sizeof(read_buffer));
		roll = read_buffer[0];
		if (!check_ret(ret, "read") || !check_bounds(roll, 1, MAX_ROLL)) {
			return false;
		}
	}

	return true;
}

bool check_write(int fp)
{	
	int roll;
	int new_max_roll = MAX_ROLL + ROLL_OFFSET;
	char read_buffer[BUFFER_LENGTH];
	char write_buffer[] = { new_max_roll, '\0' };
	int ret = write(fp, write_buffer, sizeof(write_buffer));

	check_ret(ret, "write");
	for (int i = 0; i < CHECK_COUNT; i++) {
		ret = read(fp, read_buffer, sizeof(read_buffer));
		roll = read_buffer[0];
		if (!check_ret(ret, "read") || !check_bounds(roll, 1, new_max_roll)) {
			return false;
		}
	}

	return true;
}

bool check_ioctl(int fp, int argv, char **argc)
{
	int ret = ioctl(fp, 100, *argc[0]);
	return check_ret(ret, "ioctl");
}

bool check_ret(int ret, char* file_operation)
{
	if (ret < 0) {
		fprintf(stderr, "was not able to perform %s operation\n", file_operation);
		return false;
	}

	return ret >= 0;
}

bool check_bounds(int roll, int start, int end)
{
	return roll >= start && roll <= end;
}

void print_output(bool output, char* file_operation) 
{
	if (output) {
		correct_message(file_operation);
	} else {
		fail_message(file_operation);
	}
}

void correct_message(char* file_operation)
{
	printf("%-7s: OK\n", file_operation);
}

void fail_message(char* file_operation)
{
	printf("%-7s: ERR\n", file_operation);
}
