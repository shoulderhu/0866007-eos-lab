#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h> // copy_to_user(), copy_from_user();

#define MAJOR_NUM 244
#define DEVICE_NAME "mydev"

MODULE_LICENSE("GPL");

char data;

static char map[17] = {
        0b1111110, // 0
        0b0110000, // 1
        0b1101101, // 2
        0b1111001, // 3
        0b0110011, // 4
        0b1011011, // 5
        0b1011111, // 6
        0b1110000, // 7
        0b1111111, // 8
        0b1111011, // 9
        0b1110111, // A,a
        0b0011111, // B,b
        0b1001110, // C,c
        0b0111101, // D,d
        0b1001111, // E,e
        0b1000111, // F,f
        0b0000000 // Others
};

static ssize_t my_read(struct file *fp, char *buf, size_t count, loff_t *fpos) {
    // printk("call read\n");
    char chr, from[7] = {};
    switch (data) {
        case '0':
            chr = map[0];
            break;
        case '1':
            chr = map[1];
            break;
        case '2':
            chr = map[2];
            break;
        case '3':
            chr = map[3];
            break;
        case '4':
            chr = map[4];
            break;
        case '5':
            chr = map[5];
            break;
        case '6':
            chr = map[6];
            break;
        case '7':
            chr = map[7];
            break;
        case '8':
            chr = map[8];
            break;
        case '9':
            chr = map[9];
            break;
        case 'A':
        case 'a':
            chr = map[10];
            break;
        case 'B':
        case 'b':
            chr = map[11];
            break;
        case 'C':
        case 'c':
            chr = map[12];
            break;
        case 'D':
        case 'd':
            chr = map[13];
            break;
        case 'E':
        case 'e':
            chr = map[14];
            break;
        case 'F':
        case 'f':
            chr = map[15];
            break;
        default:
            chr = map[16];
            break;
    }

    int i;
    for (i = 0; i < 7; ++i) {
        from[i] = (chr >> (6 - i)) & 1;
    }

    copy_to_user(buf, from, 7);
    return count;
}

static ssize_t my_write(struct file *fp, const char *buf, size_t count, loff_t *fpos) {
    // printk("call write\n");
    copy_from_user(&data, buf, sizeof(data));
    // printk("%c\n", data);
    return count;
}

static ssize_t my_open(struct inode *inode, struct file *fp) {
    // printk("call open\n");
    return 0;
}

struct file_operations my_fops = {
    read:  my_read,
    write: my_write,
    open:  my_open
};

static int my_init(void) {
    // printk("call init\n");
    if (register_chrdev(MAJOR_NUM, DEVICE_NAME, &my_fops) < 0) {
        printk("Can not get major %d\n", MAJOR_NUM);
        return (-EBUSY);
    }

    printk("Run: mknod /dev/mydev c %d 0", MAJOR_NUM);
    return 0;
}

static void my_exit(void) {
    // printk("call exit\n");
    unregister_chrdev(MAJOR_NUM, DEVICE_NAME);
}

module_init(my_init);
module_exit(my_exit);
