#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#define DEVICE_NAME "iut_device"

#define INIT_BALANCE 2000000
#define ACCOUNT_COUNT 100

MODULE_LICENSE("GPL");
static ssize_t iut_write(struct file *, const char __user *, size_t, loff_t *);
static int iut_open(struct inode *, struct file *);
static int iut_release(struct inode *, struct file *);
static ssize_t iut_read(struct file *, char *, size_t, loff_t *);
static struct file_operations fops = {
	.open = iut_open,
	.read = iut_read,
	.write = iut_write,
	.release = iut_release,
};
static int major; // device major number. driver reacts to this major number.

struct bank
{
	int acc[ACCOUNT_COUNT];
};
static struct bank bal;

int atoi(char *data)
{
	int sum = 0, i = 0;
	for (i = 0; data[i] >= '0' && data[i] <= '9'; i++)
	{
		sum *= 10;
		sum += (data[i] - '0');
	}
	return sum;
}
int int_len(int data)
{
	int len = 0;
	if (!data)
		len++; // In case of data=0
	while (data)
	{
		len++;
		data /= 10;
	}
	return len;
}
static int decode_input(char *data, int *out)
{ // out: from, to, amount (-1 if '-')
	int temp = 0;
	int typeT = -1;
	if (data[0] == 't')
		typeT = 0;
	else if (data[0] == 'd')
		typeT = 1;
	else if (data[0] == 'w')
		typeT = 2;
	else
	{
		printk(KERN_ALERT "ERROR : type of transaction should be specify (t or d or w) : %s\n", data);
		return -1;
	}
	// from
	if (data[1] != ',')
	{
		printk(KERN_ALERT "ERROR : second character should be a comma(,) : %s\n", data);
		return -1;
	}
	if (data[2] != '-' && !(data[2] >= '0' && data[2] <= '9'))
	{
		printk(KERN_ALERT "ERROR : From should be a dash(-) or an integer : %s\n", data);
		return -1;
	}
	if (data[2] == '-')
		temp = -1;
	else
	{
		temp = atoi(&(data[2]));
		if (temp >= ACCOUNT_COUNT || temp < 0)
		{
			printk(KERN_ALERT "ERROR : sender account ID does not exist : %s\n", data);
			return -1;
		}
	}
	out[0] = temp;
	// to
	if (data[2 + int_len(out[0])] != ',')
	{
		printk(KERN_ALERT "ERROR : after From you should use a comma(,) : %s\n", data);
		return -1;
	}
	if (data[2 + int_len(out[0]) + 1] != '-' && !(data[2 + int_len(out[0]) + 1] >= '0' && data[2 + int_len(out[0]) + 1] <= '9'))
	{
		printk(KERN_ALERT "ERROR : To should be a dash(-) or an integer : %s\n", data);
		return -1;
	}
	if (data[2 + int_len(out[0]) + 1] == '-')
		temp = -1;
	else
	{
		temp = atoi(&(data[2 + int_len(out[0]) + 1]));
		if (temp >= ACCOUNT_COUNT || temp < 0)
		{
			printk(KERN_ALERT "ERROR : reciever account ID does not exist : %s\n", data);
			return -1;
		}
	}
	out[1] = temp;
	// amount
	if (data[2 + int_len(out[0]) + 1 + int_len(out[1])] != ',')
	{
		printk(KERN_ALERT "ERROR : after To you should use a comma(,) : %s\n", data);
		return -1;
	}
	if (!(data[2 + int_len(out[0]) + 1 + int_len(out[1]) + 1] >= '0' && data[2 + int_len(out[0]) + 1 + int_len(out[1]) + 1] <= '9'))
	{
		printk(KERN_ALERT "ERROR : Amount should be an integer : %s\n", data);
		return -1;
	}
	temp = atoi(&(data[2 + int_len(out[0]) + 1 + int_len(out[1]) + 1]));
	out[2] = temp;
	return typeT;
}

static int __init iut_init(void)
{
	major = register_chrdev(0, DEVICE_NAME, &fops); // 0: auto major ||| name is
	if (major < 0)
	{
		printk(KERN_ALERT "ERROR : Device001 load failed!\n");
		return major;
	}
	int index;
	for (index = 0; index < ACCOUNT_COUNT; index++)
	{
		bal.acc[index] = 2000000;
	}
	printk(KERN_INFO "iut device module has been loaded: %d\n", major);
	return 0;
}
static void __exit iut_exit(void)
{
	unregister_chrdev(major, DEVICE_NAME);
	printk(KERN_INFO " iut device module has been unloaded.\n");
}
static int iut_open(struct inode *inodep, struct file *filep)
{
	printk(KERN_INFO " iut device opened.\n");
	return 0;
}
static int iut_release(struct inode *inodep, struct file *filep)
{
	printk(KERN_INFO " iut device closed.\n");
	return 0;
}
static ssize_t iut_read(struct file *filep, char *buffer, size_t len, loff_t *offset)
{
	int errors = 0;
	char message[1024] = {0};
	int target = 0;
	int index = 0;
	int i = 0, j = 0, lengh = 0;
	for (i = 0; i < ACCOUNT_COUNT; i++)
	{
		target = bal.acc[i];
		lengh = int_len(target);
		for (j = lengh - 1; j >= 0; j--)
		{
			message[index + j] = target % 10 + '0';
			target = (int)(target / 10);
		}
		index += lengh;
		message[index++] = ',';
		message[index++] = ' ';
	}
	message[index - 1] = 0;

	errors = copy_to_user(buffer, message, index - 1);
	return errors == 0 ? strlen(message) : -EFAULT;
}

static ssize_t iut_write(struct file *filep, const char __user *buffer, size_t len, loff_t *offset)
{
	int errors = 0;

	char message[1024] = {0};
	errors = copy_from_user(message, buffer, len);
	int out[3] = {0};
	int typeT = 0;
	typeT = decode_input(message, out);
	if (typeT < 0)
	{
		return -1;
	}
	if (typeT == 0)
	{
		if (out[0] == -1 || out[1] == -1)
		{
			printk(KERN_ALERT "ERROR : you should specify both sender ID and reciever ID : %s\n", message);
			return -1;
		}
		if (bal.acc[out[0]] < out[2])
		{
			printk(KERN_ALERT "ERROR : there is not enough supply in sender account : %s\n", message);
			return -1;
		}
		bal.acc[out[0]] -= out[2];
		bal.acc[out[1]] += out[2];
		printk(KERN_INFO "%d unit transfered from %d to %d\n", out[2], out[0], out[1]);
	}
	if (typeT == 1)
	{
		if (out[0] != -1 || out[1] == -1)
		{
			printk(KERN_ALERT "ERROR : you should specify reciever ID(section To) and leave sender ID empty(by place dash(-)) : %s\n", message);
			return -1;
		}
		bal.acc[out[1]] += out[2];
		printk(KERN_INFO "%d unit deposited to %d\n", out[2], out[1]);
	}
	if (typeT == 2)
	{
		if (out[0] == -1 || out[1] != -1)
		{
			printk(KERN_ALERT "ERROR : you should specify sender ID(section From) and leave reciever ID empty(by place dash(-)) : %s\n", message);
			return -1;
		}
		bal.acc[out[0]] -= out[2];
		printk(KERN_INFO "%d unit withdrawed from %d\n", out[2], out[0]);
	}
	return errors == 0 ? strlen(message) : -EFAULT;
}

module_init(iut_init);
module_exit(iut_exit);
