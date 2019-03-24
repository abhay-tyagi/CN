#include <linux/module.h>
#include <linux/string.h>
#include <linux/kernel.h>
#include <linux/time.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");

static int tick_init(void);
static int tick_exit(void);

static int opendev(struct inode *, struct file *);
static int closedev(struct inode *, struct file *);

static ssize_t read_me(struct file *, char *, size_t, loff_t *);
static ssize_t write_me(struct file *, const char *, size_t, loff_t *);

static int Major;

#define SUCCESS 0
#define DEVICE_NAME "TickTock"

static struct file_operations fops = {
	.read = read_me,
	.write = write_me,
	.open = opendev,
	.release = closedev
};

static int tick_init(void)
{
	Major = register_chrdev(0, DEVICE_NAME, &fops);

	if(Major < 0)
	{
		printk(KERN_ALERT "Failed to register\n");
		return -1;
	}

	printk(KERN_INFO "TickTock is ready.\n");	

	module_init(tick_init);
}

static void tick_exit(void)
{
	unregister_chrdev(Major, DEVICE_NAME);
	printk(KERN_INFO "TickTock has exited.\n");

	module_exit(tick_exit);
}

static int opendev(struct inode *a, struct file *b)
{
	printk(KERN_INFO "TickTock was opened\n");
	return SUCCESS;
}

static int closedev(struct inode *a, struct file *b)
{
	printk(KERN_INFO "TickTock was closed\n");
	return SUCCESS;
}

int pow(int x, int y) 
{ 
    if (y == 0) 
        return 1; 
    else if (y % 2 == 0) 
        return pow(x, y/2)*pow(x, y/2); 
    else
        return x*pow(x, y/2)*pow(x, y/2); 
} 

static int itoa(int val, char** ca);
int atoi(const char *str) 
{ 
    int res = 0;
  
    for (int i = 0; str[i] != '\0'; ++i) 
        res = res*10 + str[i] - '0'; 

    return res; 
} 

static ssize_t read_me(struct file *filp, char *buffer, size_t length, loff_t *offset)
{
	struct timeval time;
	do_gettimeofday(&time);

	char *b = kmalloc(sizeof(char)*40, GFP_KERNEL);
	int len = itoa(time.tv_sec, &b);	

	copy_to_user(buffer, b, len);
	printk("Return value: %s\n", b);

	return length;
}

static ssize_t write_me(struct file *filp, const char *buff, size_t len, loff_t *off)
{
	int i = atoi(buff);
	struct timespec ab;
	ab.tv_sec = i;
	ab.tv_nsec = 0;
	do_settimeofday(&ab);

	return len;
}