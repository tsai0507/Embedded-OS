#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>



MODULE_LICENSE("GPL");


static char name_to_[27][16] = {
    "1111001100010001", // A
    "0000011100000101", // b
    "1100111100000000", // C
    "0000011001000101", // d
    "1000011100000001", // E
    "1000001100000001", // F
    "1001111100010000", // G
    "0011001100010001", // H
    "1100110001000100", // I
    "1100010001000100", // J
    "0000000001101100", // K
    "0000111100000000", // L
    "0011001110100000", // M
    "0011001110001000", // N
    "1111111100000000", // O
    "1000001101000001", // P
    "0111000001010000", // q
    "1110001100011001", // R
    "1101110100010001", // S
    "1100000001000100", // T
    "0011111100000000", // U
    "0000001100100010", // V
    "0011001100001010", // W
    "0000000010101010", // X
    "0000000010100100", // Y
    "1100110000100010", // Z
    "0000000000000000"
};

// static int name_to_[27][16] = {
//     {1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1}, // A
//     {0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 1}, // b
//     {1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0}, // C
//     {0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 1, 0, 1}, // d
//     {1, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1}, // E
//     {1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1}, // F
//     {1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0}, // G
//     {0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1}, // H
//     {1, 1, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0}, // I
//     {1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0}, // J
//     {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0}, // K
//     {0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0}, // L
//     {0, 0, 1, 1, 0, 0, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0}, // M
//     {0, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0}, // N
//     {1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0}, // O
//     {1, 0, 0, 0, 0, 0, 1, 1, 0, 1, 0, 0, 0, 0, 0, 1}, // P
//     {0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0}, // q
//     {1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 1}, // R
//     {1, 1, 0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1}, // S
//     {1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0}, // T
//     {0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0}, // U
//     {0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 1, 0}, // V
//     {0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 1, 0, 1, 0}, // W
//     {0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0}, // X
//     {0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0}, // Y
//     {1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0}, // Z
//     {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
// };

// static uint16_t name_Number = 0;
// static uint16_t reader_Count = 0;
static char NAME[5];
static int index;
static char letter;

// File Operations
static ssize_t my_read(struct file *fp, char *buf, size_t count, loff_t *fpos) {

    printk("call read\n");

    index = 0;
    letter = NAME[0];
    printk("latter is %c\n", letter);

    if(letter == 'a')
        index = 0;
    else if(letter == 'b')
        index = 1;
    else if(letter == 'c')
        index = 2;
    else if(letter == 'd')
        index = 3;
    else if(letter == 'e')
        index = 4;
    else if(letter == 'f')
        index = 5;
    else if(letter == 'g')
        index = 6;
    else if(letter == 'h')
        index = 7;
    else if(letter == 'i')
        index = 8;
    else if(letter == 'j')
        index = 9;
    else if(letter == 'k')
        index = 10;
    else if(letter == 'l')
        index = 11;
    else if(letter == 'm')
        index = 12;
    else if(letter == 'n')
        index = 13;
    else if(letter == 'o')
        index = 14;
    else if(letter == 'p')
        index = 15;
    else if(letter == 'q')
        index = 16;
    else if(letter == 'r')
        index = 17;
    else if(letter == 's')
        index = 18;
    else if(letter == 't')
        index = 19;
    else if(letter == 'u')
        index = 20;
    else if(letter == 'v')
        index = 21;
    else if(letter == 'w')
        index = 22;
    else if(letter == 'x')
        index = 23;
    else if(letter == 'y')
        index = 24;
    else if(letter == 'z')
        index = 25;
    else
        index = 26;

    count = 16;
    if( copy_to_user(buf, name_to_[index], count) > 0) {
        pr_err("ERROR: Not all the bytes have been copied to user\n");  
    }

    // reader_Count += 1;
    // if(reader_Count == name_Number)
    //     reader_Count = 0;
    // else if(reader_Count == 10)
    //     reader_Count = 0;

    return count;
}

static ssize_t my_write(struct file *fp,const char *buf, size_t count, loff_t *fpos){

    printk("call write\n");

    if( copy_from_user( NAME, buf, count ) > 0) {
        pr_err("ERROR: Not all the bytes have been copied from user\n");
    }
    // name_Number = count;
    // reader_Count = 0;

    return count;
}

static int my_open(struct inode *inode, struct file *fp) {

    printk("call open\n");

    return 0;
}

struct file_operations my_fops = {
    read: my_read,
    write: my_write,
    open: my_open
};

#define MAJOR_NUM 200
#define DEVICE_NAME "my_dev"

static int my_init(void) {
    printk("call init\n");
    if(register_chrdev(MAJOR_NUM, DEVICE_NAME, &my_fops) < 0) {
    printk("Can not get major %d\n", MAJOR_NUM);
    return (-EBUSY);
    }

    printk("My device is started and the major is %d\n", MAJOR_NUM);
    return 0;
}

static void my_exit(void) {
    unregister_chrdev(MAJOR_NUM, DEVICE_NAME);
    printk("call exit\n");
}

module_init(my_init);
module_exit(my_exit);