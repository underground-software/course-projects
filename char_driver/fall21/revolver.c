#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <linux/fs.h>
#include <linux/cdev.h>

#define MAJOR 42
#define MINOR_COUNT 1

struct dev_data {
  struct cdev cdev;
  int chamber; // 0 - 6
};

struct dev_data devs[MINOR_COUNT]

const struct file_operations fops {
  .owner = THIS_MODULE,
  .open = ropen,
  .read = rread,
  .write = rwrite,
  .release = rrelease,
  .unlocked_ioctl = rioctl;
};

int init_module(void){
  int i, err;
  register_chrdev_region(MKDEV(MAJOR, 0), MINOR_COUNT, "revolver_driver");
  for(i = 0; i < MINOR_COUNT; i++) {
    cdev_init(&devs[i].cdev, &fops);
    cdev_add(&devs[i].cdev, MKDEV(MAJOR, i), 1);
  }
  return 0;
}

void cleanup_module(void){
  int i;
  for(i = 0; i < MINOR_COUNT; i++) {
    cdev_del(&devs[i].cdev);
  }
  unregister_chrdev_region(MKDEV(MAJOR, 0), MINOR_COUNT);
}

static int ropen(struct inode *inode, struct file *file){
  return 0;
}

static int rclose(struct inode *inode, struct file *file){
  return 0;
}

static long rioctl(struct file *file, unsigned int cmd, unsigned long arg){
  return 0;
}

ssize_t (*read) (struct file *, char __user *, size_t, loff_t *){
  _user[0] = '0' + devs[0].chamber;
  return 1;
}

ssize_t (*write) (struct file *, const char __user *, size_t, loff_t *){
  if(_user[0] == 's'){
    if(devs[0].chamber < 1){
      return -1;
    }
    devs[0].chamber = devs[0].chamber - 1;
    return 1;
  } else if(_user[0] == 'l'){
    if(devs[0].chamber > 5){
      return -1;
    }
    devs[0].chamber = devs[0].chamber + 1;
  }
  return -2;
}

int main(int argc, char * argv[]){

  return 0;
}
