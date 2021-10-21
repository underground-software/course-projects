#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

#define BUFFER_SIZE 100

void test_ioctl(int my_dev);

int main(int argc, char **argv)
{
  int choice;
  char input_buffer[BUFFER_SIZE];
  char output_buffer[BUFFER_SIZE];
  int my_dev = open("/dev/mydev", 0); // path, minor number
  bool play = true;

  if (my_dev < 0) {
    perror("Failed to open device file: /dev/mydev.");
    exit(1);
  } 

  test_ioctl(my_dev);

  while (play) {
    printf("Enter 1 to read sacred text. Enter 2 to write to sacred text. Enter 3 to stop\n");
    scanf("%d", &choice);
    switch (choice) {
      case 1:
        printf("Enter text to write into sacred text: ");
        scanf("%s", input_buffer);
        write(my_dev, input_buffer, BUFFER_SIZE * sizeof(char));
        break;
      case 2:
        printf("Reading sacred text\n");
        read(my_dev, output_buffer, BUFFER_SIZE * sizeof(char));
        printf("Sacred text is as follows: %s\n", output_buffer);
        break;
      case 3:
        play = false;
        break;
      default:
        printf("Enter valid choice\n");
    }
  }

  close(my_dev);
  return 0;
}

void test_ioctl(int my_dev) {
  ioctl(my_dev, 1, 2); // cmd = 1, arg = 2 
}