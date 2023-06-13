/***************************************************************************
**
*   \file
    led_driver.c
*   \details
    Simple GPIO driver explanation
*   \author EmbeTronicX
*   \Tested with Linux raspberrypi 5.4.51-v7l+
******************************************************************************
*/
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/uaccess.h>  //copy_to/from_user()
#include <linux/gpio.h>     //GPIO

//LED is connected to this GPIO
#define GPIO_21 (21)
#define GPIO_20 (20)
#define GPIO_16 (16)
#define GPIO_12 (12)
#define GPIO_1 (1)
#define GPIO_7 (7)
#define GPIO_8 (8)

dev_t dev = 0;
static struct class *dev_class;
static struct cdev etx_cdev;

static int __init etx_driver_init(void);
static void __exit etx_driver_exit(void);
/*************** Driver functions **********************/
static int etx_open(struct inode *inode, struct file *file);
static int etx_release(struct inode *inode, struct file *file);
static ssize_t etx_read(struct file *filp,
                char __user *buf, size_t len,loff_t * off);
static ssize_t etx_write(struct file *filp,
                const char *buf, size_t len, loff_t * off);
/******************************************************/

//File operation structure
static struct file_operations fops =
{
    .owner = THIS_MODULE,
    .read = etx_read,
    .write = etx_write,
    .open = etx_open,
    .release = etx_release,
};

//seven segment
static char SEVEN[11][7] = {
    "1111110", //0
    "0110000", //1
    "1101101", //2
    "1111001", //3
    "0110011", //4
    "1011011", //5
    "1011111", //6
    "1110010", //7
    "1111111", //8
    "1110011", //9
};

/*
** This function will be called when we open the Device file
*/
static int etx_open(struct inode *inode, struct file *file)
{
    pr_info("Device File Opened...!!!\n");
    return 0;
}

/*
** This function will be called when we close the Device file
*/
static int etx_release(struct inode *inode, struct file *file)
{
    pr_info("Device File Closed...!!!\n");
    return 0;
}

/*
** This function will be called when we read the Device file
*/
static ssize_t etx_read(struct file *filp,
                char __user *buf, size_t len, loff_t *off)
{
    uint8_t gpio_state_21 = 0, gpio_state_20 = 0, gpio_state_12 = 0, gpio_state_16 = 0;

    //reading GPIO value
    gpio_state_21 = gpio_get_value(GPIO_21);
    gpio_state_20 = gpio_get_value(GPIO_20);
    gpio_state_12 = gpio_get_value(GPIO_12);
    gpio_state_16 = gpio_get_value(GPIO_16);

    //write to user
    len = 1;
    if( copy_to_user(buf, &gpio_state_21, len) > 0) {
        pr_err("ERROR: Not all the bytes have been copied to user\n");  
    }pr_info("Read function : GPIO_21 = %d \n", gpio_state_21);

    if( copy_to_user(buf, &gpio_state_20, len) > 0) {
        pr_err("ERROR: Not all the bytes have been copied to user\n");
    }pr_info("Read function : GPIO_20 = %d \n", gpio_state_20);

    if( copy_to_user(buf, &gpio_state_12, len) > 0) {
        pr_err("ERROR: Not all the bytes have been copied to user\n");
    }pr_info("Read function : GPIO_12 = %d \n", gpio_state_12);

    if( copy_to_user(buf, &gpio_state_16, len) > 0) {
        pr_err("ERROR: Not all the bytes have been copied to user\n");
    }pr_info("Read function : GPIO_16 = %d \n", gpio_state_16);

    return 0;
}

/*
** This function will be called when we write the Device file
*/
static ssize_t etx_write(struct file *filp,
                    const char __user *buf, size_t len, loff_t *off)
{
    uint8_t rec_buf[10] = {0};
    size_t n1 = 0, n2 = 0, n3 = 0, n4 = 0, n5 = 0, n6 = 0, n7 = 0, i;
    int  index = 0;
    
    if( copy_from_user( rec_buf, buf, len ) > 0) {
        pr_err("ERROR: Not all the bytes have been copied from user\n");
    }

    //print to dmesg
    pr_info("Write Function : GPIO Set = ");
    for(i = 0; i < len; i++)
        pr_info("%c", rec_buf[i]);
    pr_info("\n");

    
    for(i = 0 ;i < len; i++){

        index = rec_buf[i] - '0';
        pr_info("number is : %d\n", index);
        n1 = SEVEN[index][0];
        n2 = SEVEN[index][1];
        n3 = SEVEN[index][2];
        n4 = SEVEN[index][3];
        n5 = SEVEN[index][4];
        n6 = SEVEN[index][5];
        n7 = SEVEN[index][6];


        if (n1 == '1') {
        //set the GPIO value to HIGH
            gpio_set_value(GPIO_21, 1);
        } else if (n1 == '0') {
        //set the GPIO value to LOW
            gpio_set_value(GPIO_21, 0);
        } else {
            pr_err("Unknown command : Please provide either 1 or 0 \n");
        }

        if (n2 == '1') {
        //set the GPIO value to HIGH
            gpio_set_value(GPIO_20, 1);
        } else if (n2 == '0') {
        //set the GPIO value to LOW
            gpio_set_value(GPIO_20, 0);
        }

        if (n3 == '1') {
        //set the GPIO value to HIGH
            gpio_set_value(GPIO_16, 1);
        } else if (n3 == '0') {
        //set the GPIO value to LOW
            gpio_set_value(GPIO_16, 0);
        } 

        if (n4 == '1') {
        //set the GPIO value to HIGH
            gpio_set_value(GPIO_12, 1);
        } else if (n4 == '0') {
        //set the GPIO value to LOW
            gpio_set_value(GPIO_12, 0);
        }

        if (n5 == '1') {
        //set the GPIO value to HIGH
            gpio_set_value(GPIO_1, 1);
        } else if (n5 == '0') {
        //set the GPIO value to LOW
            gpio_set_value(GPIO_1, 0);
        }

        if (n6 == '1') {
        //set the GPIO value to HIGH
            gpio_set_value(GPIO_7, 1);
        } else if (n6 == '0') {
        //set the GPIO value to LOW
            gpio_set_value(GPIO_7, 0);
        }

        if (n7 == '1') {
        //set the GPIO value to HIGH
            gpio_set_value(GPIO_8, 1);
        } else if (n7 == '0') {
        //set the GPIO value to LOW
            gpio_set_value(GPIO_8, 0);
        }
        
        msleep(1000);
    }
    gpio_set_value(GPIO_21, 0);
    gpio_set_value(GPIO_20, 0);
    gpio_set_value(GPIO_16, 0);
    gpio_set_value(GPIO_12, 0);
    gpio_set_value(GPIO_1, 0);
    gpio_set_value(GPIO_7, 0);
    gpio_set_value(GPIO_8, 0);

    return len;
}

/*
** Module Init function
*/
static int __init etx_driver_init(void)
{
    /*Allocating Major number*/
    if((alloc_chrdev_region(&dev, 0, 1, "etx_Dev")) <0){
        pr_err("Cannot allocate major number\n");
        goto r_unreg;
    }
    pr_info("Major = %d Minor = %d \n",MAJOR(dev), MINOR(dev));

    /*Creating cdev structure*/
    cdev_init(&etx_cdev,&fops);

    /*Adding character device to the system*/
    if((cdev_add(&etx_cdev,dev,1)) < 0){
        pr_err("Cannot add the device to the system\n");
        goto r_del;
    }

    /*Creating struct class*/
    if((dev_class = class_create(THIS_MODULE,"etx_class")) == NULL){
        pr_err("Cannot create the struct class\n");
        goto r_class;
    }

    /*Creating device*/
    if((device_create(dev_class,NULL,dev,NULL,"etx_device")) == NULL){
        pr_err( "Cannot create the Device \n");
        goto r_device;
    }

    //Checking the GPIO is valid or not
    if(gpio_is_valid(GPIO_21) == false){
        pr_err("GPIO %d is not valid\n", GPIO_21);
        goto r_device;
    }

    if(gpio_is_valid(GPIO_20) == false){
        pr_err("GPIO %d is not valid\n", GPIO_20);
        goto r_device;
    }

    if(gpio_is_valid(GPIO_16) == false){
        pr_err("GPIO %d is not valid\n", GPIO_16);
        goto r_device;
    }

    if(gpio_is_valid(GPIO_12) == false){
        pr_err("GPIO %d is not valid\n", GPIO_12);
        goto r_device;
    }
    if(gpio_is_valid(GPIO_1) == false){
        pr_err("GPIO %d is not valid\n", GPIO_1);
        goto r_device;
    }
    if(gpio_is_valid(GPIO_7) == false){
        pr_err("GPIO %d is not valid\n", GPIO_7);
        goto r_device;
    }
    if(gpio_is_valid(GPIO_8) == false){
        pr_err("GPIO %d is not valid\n", GPIO_8);
        goto r_device;
    }

    //Requesting the GPIO
    if(gpio_request(GPIO_21,"GPIO_21") < 0){
        pr_err("ERROR: GPIO %d request\n", GPIO_21);
        goto r_gpio;
    }

    if(gpio_request(GPIO_20,"GPIO_20") < 0){
        pr_err("ERROR: GPIO %d request\n", GPIO_20);
        goto r_gpio;
    }

    if(gpio_request(GPIO_16,"GPIO_16") < 0){
        pr_err("ERROR: GPIO %d request\n", GPIO_16);
        goto r_gpio;
    }

    if(gpio_request(GPIO_12,"GPIO_12") < 0){
        pr_err("ERROR: GPIO %d request\n", GPIO_12);
        goto r_gpio;
    }
    if(gpio_request(GPIO_1,"GPIO_1") < 0){
        pr_err("ERROR: GPIO %d request\n", GPIO_1);
        goto r_gpio;
    }
    if(gpio_request(GPIO_7,"GPIO_7") < 0){
        pr_err("ERROR: GPIO %d request\n", GPIO_7);
        goto r_gpio;
    }
    if(gpio_request(GPIO_8,"GPIO_8") < 0){
        pr_err("ERROR: GPIO %d request\n", GPIO_8);
        goto r_gpio;
    }
    //configure the GPIO as output
    gpio_direction_output(GPIO_21, 0);
    gpio_direction_output(GPIO_20, 0);
    gpio_direction_output(GPIO_16, 0);
    gpio_direction_output(GPIO_12, 0);
    gpio_direction_output(GPIO_1, 0);
    gpio_direction_output(GPIO_7, 0);
    gpio_direction_output(GPIO_8, 0);

    /* Using this call the GPIO 21 will be visible in /sys/class/gpio/
    ** Now you can change the gpio values by using below commands also.
    ** echo 1 > /sys/class/gpio/gpio21/value (turn ON the LED)
    ** echo 0 > /sys/class/gpio/gpio21/value (turn OFF the LED)
    ** cat /sys/class/gpio/gpio21/value (read the value LED)
    **
    ** the second argument prevents the direction from being changed.
    */
    gpio_export(GPIO_21, false);
    gpio_export(GPIO_20, false);
    gpio_export(GPIO_16, false);
    gpio_export(GPIO_12, false);
    gpio_export(GPIO_1, false);
    gpio_export(GPIO_7, false);
    gpio_export(GPIO_8, false);
    pr_info("Device Driver Insert...Done!!!\n");
    return 0;

r_gpio:
    gpio_free(GPIO_21);
    gpio_free(GPIO_20);
    gpio_free(GPIO_12);
    gpio_free(GPIO_16);
    gpio_free(GPIO_1);
    gpio_free(GPIO_7);
    gpio_free(GPIO_8);
r_device:
    device_destroy(dev_class,dev);
r_class:
    class_destroy(dev_class);
r_del:
    cdev_del(&etx_cdev);
r_unreg:
    unregister_chrdev_region(dev,1);

    return -1;
}

/*
** Module exit function
*/
static void __exit etx_driver_exit(void)
{
    gpio_unexport(GPIO_21);
    gpio_unexport(GPIO_20);
    gpio_unexport(GPIO_16);
    gpio_unexport(GPIO_12);
    gpio_unexport(GPIO_1);
    gpio_unexport(GPIO_7);
    gpio_unexport(GPIO_8);

    gpio_free(GPIO_21);
    gpio_free(GPIO_20);
    gpio_free(GPIO_16);
    gpio_free(GPIO_12);
    gpio_free(GPIO_1);
    gpio_free(GPIO_7);
    gpio_free(GPIO_8);
    
    device_destroy(dev_class,dev);
    class_destroy(dev_class);
    cdev_del(&etx_cdev);
    unregister_chrdev_region(dev, 1);
    pr_info("Device Driver Remove...Done!!\n");
}
module_init(etx_driver_init);
module_exit(etx_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("frank <frank4144@gmail.com>");
MODULE_DESCRIPTION("A simple device driver - GPIO Driver");
MODULE_VERSION("1.32");