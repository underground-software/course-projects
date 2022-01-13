#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/module.h>
#include <linux/random.h>
#include <linux/types.h>

#define MY_MAJOR_NUM	230
#define READ_LENGTH	2
#define WRITE_LENGTH 	10

static struct cdev dice;
bool toggle_cheat = false;
int max_roll = 6;
int losing_numbers[] = { 1, 6 };

static int open(struct inode *inode, struct file *file)
{
	printk(KERN_INFO "File opened\n");
	return 0;
}

static int close(struct inode *inode, struct file *file)
{
	printk(KERN_INFO "File closed\n");
	return 0;
}

static long ioctl(struct file *file, unsigned int cmd, unsigned long arg) 
{	
	printk(KERN_INFO "ioctl() is called. cmd = %d, arg = %ld\n", cmd, arg);
	if (cmd == 100 && arg == 7) {
		toggle_cheat = true;
		printk(KERN_INFO "Cheat has been toggled\n");
	}
	return 0;
}       

unsigned char get_random_byte(struct file *file, int max)
{
	unsigned char c;
	get_random_bytes(&c, 1);
	return (c % max)+1;
}

static ssize_t read(struct file *file, char __user *buf, size_t count, loff_t *f_pos) 
{
	unsigned char c = get_random_byte(file, max_roll);
	char buffer[READ_LENGTH];

	if (toggle_cheat) {
		while (c == losing_numbers[0] || c == losing_numbers[1]) {
			c = get_random_byte(file, max_roll);
		}
	}
	
	buffer[0] = c;
	buffer[1] = '\0';
	if (copy_to_user(buf, buffer, sizeof(buffer))) {
		printk(KERN_INFO "Unable to write buffer to user\n");
		return -EFAULT;
	}
	printk(KERN_INFO "Data read\n");
	return count;
}

static ssize_t write(struct file *file, const char __user *buf, size_t count, loff_t *f_pos) 
{
	char buffer[WRITE_LENGTH];

	if (copy_from_user(buffer, buf, sizeof(*buf))) {
		printk(KERN_INFO "Unable to read buffer from user\n");
		return -EFAULT;
	}
	max_roll = buffer[0];
	printk(KERN_INFO "Data written\n");
	return count;
}

static const struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = open,
	.release = close,
	.unlocked_ioctl = ioctl,
	.read = read,
	.write = write,
};

static int  __init dice_init(void)
{
	int ret;
	
	dev_t dev = MKDEV(MY_MAJOR_NUM, 0);
	pr_info("Init\n");
	ret = register_chrdev_region(dev, 1, "dice");
	if (ret < 0) {
		printk(KERN_INFO "Unable to allocate major number %d\n", MY_MAJOR_NUM);
	}
	cdev_init(&dice, &fops);
	ret = cdev_add(&dice, dev, 1);
	if (ret < 0) {
		unregister_chrdev_region(dev, 1);
		printk(KERN_INFO "Unable to add cdev\n");
		return ret;
	}
	return 0;
}

static void __exit dice_exit(void)
{
	printk(KERN_INFO "Exit\n");
	cdev_del(&dice);
	unregister_chrdev_region(MKDEV(MY_MAJOR_NUM, 0), 1);
}

module_init(dice_init);
module_exit(dice_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Gabriel Shahrouzi");
MODULE_DESCRIPTION("Character driver");
