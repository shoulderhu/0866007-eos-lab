#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h> // copy_to_user(), copy_from_user();

#define MAJOR_NUM 244
#define DEVICE_NAME "mydev"

MODULE_LICENSE("GPL");

char data;

static int map[27] = {
        0b1111001100010001, // A
        0b0000011100000101, // b
        0b1100111100000000, // C
        0b0000011001000101, // d
        0b1000011100000001, // E
        0b1000001100000001, // F
        0b1001111100010000, // G
        0b0011001100010001, // H
        0b1100110001000100, // I
        0b1100010001000100, // J
        0b0000000001101100, // K
        0b0000111100000000, // L
        0b0011001110100000, // M
        0b0011001110001000, // N
        0b1111111100000000, // O
        0b1000001101000001, // P
        0b0111000001010000, //q
        0b1110001100011001, //R
        0b1101110100010001, //S
        0b1100000001000100, //T
        0b0011111100000000, //U
        0b0000001100100010, //V
        0b0011001100001010, //W
        0b0000000010101010, //X
        0b0000000010100100, //Y
        0b1100110000100010, //Z
        0b0000000000000000
};

static ssize_t my_read(struct file *fp, char *buf, size_t count, loff_t *fpos) {
    // printk("call read\n");
    int chr;
    char from[32] = {};
        switch (data) {
        case 'A':
        case 'a':
            chr = map[0];
            break;
        case 'B':
        case 'b':
            chr = map[1];
            break;
        case 'C':
        case 'c':
            chr = map[2];
            break;
        case 'D':
        case 'd':
            chr = map[3];
            break;
        case 'E':
        case 'e':
            chr = map[4];
            break;
        case 'F':
        case 'f':
            chr = map[5];
            break;
        case 'G':
        case 'g':
            chr = map[6];
            break;
        case 'H':
        case 'h':
            chr = map[7];
            break;
        case 'I':
        case 'i':
            chr = map[8];
            break;
        case 'J':
        case 'j':
            chr = map[9];
            break;
        case 'K':
        case 'k':
            chr = map[10];
            break;
        case 'L':
        case 'l':
            chr = map[11];
            break;
        case 'M':
        case 'm':
            chr = map[12];
            break;
        case 'N':
        case 'n':
            chr = map[13];
            break;
        case 'O':
        case 'o':
            chr = map[14];
            break;
        case 'P':
        case 'p':
            chr = map[15];
            break;
        case 'Q':
        case 'q':
            chr = map[16];
            break;
        case 'R':
        case 'r':
            chr = map[17];
            break;
        case 'S':
        case 's':
            chr = map[18];
            break;
        case 'T':
        case 't':
            chr = map[19];
            break;
        case 'U':
        case 'u':
            chr = map[20];
            break;
        case 'V':
        case 'v':
            chr = map[21];
            break;
        case 'W':
        case 'w':
            chr = map[22];
            break;
        case 'X':
        case 'x':
            chr = map[23];
            break;
        case 'Y':
        case 'y':
            chr = map[24];
            break;
        case 'Z':
        case 'z':
            chr = map[25];
            break;
        default:
            chr = map[26];
            break;
    }

    int i;
    for (i = 0; i < 16; ++i) {
        from[i] = (chr >> (15 - i)) & 1;
    }
    copy_to_user(buf, from, 16);
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
