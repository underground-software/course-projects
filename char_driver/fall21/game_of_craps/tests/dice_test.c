#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdbool.h>

#define MAX_LENGTH	100
#define MICRO_DELAY	50000
#define MAX_ROLL        6

void delay_print(char* s);
void welcome_message(char* name);
void continue_message(char* name, int* choice);
int get_roll(int fp, char* read_buffer);
void roll_message(int roll1, int roll2);
void roll_result(int roll1, int roll2, int fp, char* read_buffer);
void roll_point_result(int result, int fp, char* read_buffer);
void win_message(void);
void lose_message(void);
void goodbye_message(char* name);
void check_ret(int ret, char* file_operation);

int main(int argc, char **argv) 
{
	int fp;
	int choice;
	int roll1;
	int roll2;
	int ret;
	char name[MAX_LENGTH];
	char write_buffer[MAX_LENGTH];
	char read_buffer[MAX_LENGTH];
	bool play = true;
	bool point = false;

	fp = open("/dev/dice", O_RDWR);
	if (fp < 0) {
		perror("Could not open device file\n");
		exit(1);
	}
	
	if (argc > 1) {
		ret = ioctl(fp, 100, atoi(argv[1]));
		check_ret(ret, "ioctl");
	}
	
	write_buffer[0] = MAX_ROLL;
	ret = write(fp, write_buffer, sizeof(write_buffer));
	check_ret(ret, "write");

	welcome_message(name);
	while (play) {
		continue_message(name, &choice);
		if (choice) {
			roll1 = get_roll(fp, read_buffer);
			roll2 = get_roll(fp, read_buffer);
			roll_message(roll1, roll2);
			roll_result(roll1, roll2, fp, read_buffer);
		} else {
			play = false;
			goodbye_message(name);
		}
	}

	close(fp);
	return 0;
}

void delay_print(char* s) 
{
	int n = strlen(s);
	for (int i = 0; i < n; i++) {
		printf("%c", s[i]);
		fflush(stdout);
		usleep(MICRO_DELAY);
	}
}

void welcome_message(char* name) 
{
	delay_print("Welcome to Charlie's Casino!\n");
	delay_print("Please enter your name: ");
	scanf("%s", name);	
}

void continue_message(char* name, int* choice)
{
	delay_print(name);
	delay_print(", press 1 to play or 0 to quit: ");
	scanf("%d", choice);
	printf("\n");
}

int get_roll(int fp, char* read_buffer) 
{
	int ret = read(fp, read_buffer, sizeof(read_buffer));
	check_ret(ret, "read");
	return read_buffer[0];
}

void roll_message(int roll1, int roll2)
{
	delay_print("You have rolled ");
	printf("%d", roll1);
	delay_print(" + ");
	printf("%d", roll2);
	delay_print(" = ");
	printf("%d\n", roll1 + roll2);
}

void roll_result(int roll1, int roll2, int fp, char* read_buffer) 
{
	int result = roll1 + roll2;
	switch (result) {
		case 2:
			lose_message();
			break;
		case 3:
			lose_message();
			break;
		case 12:
			lose_message();
			break;
		case 7:
			win_message();
			break;
		case 11:
			win_message();
			break;
		default:
			roll_point_result(result, fp, read_buffer);
	}
}

void roll_point_result(int result, int fp, char* read_buffer) 
{
	int roll1;
	int roll2;
	int new_result;
	int lose_num = 7;

       	do {
		roll1 = get_roll(fp, read_buffer);
		roll2 = get_roll(fp, read_buffer);
		new_result = roll1 + roll2;
		roll_message(roll1, roll2);
	} while (new_result != result && new_result != lose_num);

	switch (new_result) {
		case 7:
			lose_message();
			break;
		default:
			win_message();
	}	
}	

void win_message(void)
{
	delay_print("You Win!\n\n");
}

void lose_message(void)
{
	delay_print("You Lose!\n\n");
}

void goodbye_message(char* name) 
{
	delay_print("Goodbye, ");
	delay_print(name);
	delay_print("!\n");
}

void check_ret(int ret, char* file_operation) 
{
	if (ret < 0) {
		fprintf(stderr, "Was not able to perform %s operation\n", file_operation); 
		exit(1);
	}
}
