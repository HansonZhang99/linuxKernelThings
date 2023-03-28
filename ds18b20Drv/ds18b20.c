#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/delay.h>
#include <asm/irq.h>
#include <mach/regs-gpio.h>
#include <mach/hardware.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/kdev_t.h>
#include <asm/uaccess.h>
#include <linux/platform_device.h>
#include <linux/gpio.h>

#define DRV_AUTHOR                          "zhanghang <1711318490@qq.com>"
#define DRV_DESC                            "S3C2440 ds18b20 driver"

#define DEV_NAME                            "ds18b20"


#define LOW                                 0
#define HIGH                                1

#ifndef DEV_MAJOR
#define DEV_MAJOR                           0
#endif

int dev_major = DEV_MAJOR;
int dev_minor = 0;

typedef unsigned char BYTE;
static BYTE data[2];

struct ds18b20_device 
{
	struct class *class;
	struct cdev  *cdev;
};

static struct ds18b20_device dev;

BYTE ds18b20_reset(void)
{
    s3c2410_gpio_cfgpin(S3C2410_GPG(0), S3C2410_GPIO_OUTPUT);/*配置GPG(0)管脚为输出模式，CPU到DS18B20*/
    s3c2410_gpio_setpin(S3C2410_GPG(0), LOW);/*配置GPG(0)管脚低电平，持续480μs*/
    udelay(480);
    s3c2410_gpio_setpin(S3C2410_GPG(0), HIGH);/*配置GPG(0)管脚高电平，持续60μs*/
    udelay(60);


    s3c2410_gpio_cfgpin(S3C2410_GPG(0), S3C2410_GPIO_INPUT);/*配置GPG(0)为输入模式，DS18B20到CPU*/
    if(s3c2410_gpio_getpin(S3C2410_GPG(0)))/*读GPG(0)管脚的状态，此时必须为低电平才算初始化成功*/
    {
        printk("ds18b20 reset failed.\r\n");
        return 1;
    }
    udelay(240);/*延时240μs*/
    return 0;
}


BYTE ds18b20_read_byte(void)
{
    BYTE i = 0;
    BYTE byte = 0;
/* 读“1”时隙：   
    若总线状态保持在低电平状态1微秒到15微秒之间   
    然后跳变到高电平状态且保持在15微秒到60微秒之间   
    就认为从DS18B20读到一个“1”信号   
    理想情况: 1微秒的低电平然后跳变再保持60微秒的高电平   
   读“0”时隙：   
    若总线状态保持在低电平状态15微秒到30微秒之间   
    然后跳变到高电平状态且保持在15微秒到60微秒之间   
    就认为从DS18B20读到一个“0”信号   
    理想情况: 15微秒的低电平然后跳变再保持46微秒的高电平  */ 

    for(i = 0; i < 8; i++)
    {
        s3c2410_gpio_cfgpin(S3C2410_GPG(0), S3C2410_GPIO_OUTPUT);/*配置GPG(0)管脚为输出模式，CPU到DS18B20*/
        s3c2410_gpio_setpin(S3C2410_GPG(0), LOW);/*设置GPG(0)管脚为低电平状态*/
        udelay(1);/*延时1μs*/
        byte >>= 1;/*byte右移一位*/
        s3c2410_gpio_setpin(S3C2410_GPG(0), HIGH);/*设置GPG(0)管脚为高电平状态*/
        s3c2410_gpio_cfgpin(S3C2410_GPG(0), S3C2410_GPIO_INPUT);/*设置GPG(0)管脚为输入模式,DS18B20到CPU*/
        if(s3c2410_gpio_getpin(S3C2410_GPG(0)))/*读GPG(0)管脚，如果为高电平*/
            byte |= 0x80;/*将最高为置为1*/
        udelay(60);/*延时60μs*/
    }
/*这个循环创造8个读时隙，每次读取1个位到byte，最后byte的最低位为第一次读取数据，最高位为最后其次读取的位*/
    return byte;
}


BYTE ds18b20_write_byte(BYTE byte)
{
/*   写“1”时隙：   
         保持总线在低电平1微秒到15微秒之间   
         然后再保持总线在高电平15微秒到60微秒之间   
         理想状态: 1微秒的低电平然后跳变再保持60微秒的高电平     
     写“0”时隙：   
         保持总线在低电平15微秒到60微秒之间   
         然后再保持总线在高电平1微秒到15微秒之间   
         理想状态: 60微秒的低电平然后跳变再保持1微秒的高电平   */
    BYTE i;
    s3c2410_gpio_cfgpin(S3C2410_GPG(0), S3C2410_GPIO_OUTPUT);/*配置GPG(0)管脚为输出模式，CPU到DS18B20*/
    for(i = 0; i < 8; i++)
    {
        s3c2410_gpio_setpin(S3C2410_GPG(0), LOW);/*设置GPG(0)管脚为低电平*/
        udelay(15);/*延时15μs*/
        if(byte & HIGH)/*判断byte的最低位是否为高电平*/
        {
            /*如果byte的最低位为1，根据写1时隙规则，应将GPG(0)管脚设置为高电平*/
            s3c2410_gpio_setpin(S3C2410_GPG(0), HIGH);/*设置GPG(0)为高电平*/
        }
        else
        {
            /*如果byte的最低位为0，根据写0时隙规则，应将GPG(0)管脚设置为低电平*/
            s3c2410_gpio_setpin(S3C2410_GPG(0), LOW);/*设置GPG(0)为低电平*/
        }
        udelay(45);
        /*延时45μs后，将管脚设置为高电平，释放总线*/

        s3c2410_gpio_setpin(S3C2410_GPG(0), HIGH);
        udelay(1);
        byte >>= 1;
    }
    s3c2410_gpio_setpin(S3C2410_GPG(0), HIGH);/*最后释放总线*/
    return 0;
}


void ds18b20_proc(void)
{
        while(ds18b20_reset());// 每次读写均要先复位
        udelay(120);
        ds18b20_write_byte(0xcc);//发跳过ROM命令
        ds18b20_write_byte(0x44);//发读开始转换命令
        udelay(5);


        while(ds18b20_reset());
        udelay(200);
        ds18b20_write_byte(0xcc);//发跳过ROM命令
        ds18b20_write_byte(0xbe);//读寄存器，共九字节，前两字节为转换值
          
        data[0] = ds18b20_read_byte();//存低字节
        data[1] = ds18b20_read_byte();//存高字节
}


static ssize_t s3c2440_ds18b20_read(struct file *filp, char *buff, size_t len, loff_t *off)
{
        int err;
        ds18b20_proc();
        err = copy_to_user(buff, data, sizeof(data));/*发送数据到用户空间*/

        return err? -EFAULT:len;

        return 1;
}


static struct file_operations ds18b20_fops =
{
    .owner = THIS_MODULE,
    .read = s3c2440_ds18b20_read,
};


static int __init s3c_ds18b20_init(void)
{
    int                     result;
    dev_t                   devno;


    if(0 != ds18b20_reset())
    {
        printk(KERN_ERR "s3c2440 ds18b20 hardware initialize failure.\n");
        return -ENODEV;
    }

    if(0 != dev_major)
    {
        devno = MKDEV(dev_major, 0);
        result = register_chrdev_region(devno, 1, DEV_NAME);
    }
    else
    {
        result = alloc_chrdev_region(&devno, dev_minor, 1, DEV_NAME);
        dev_major = MAJOR(devno);
    }

    if(result < 0)
    {
        printk(KERN_ERR "s3c %s driver cannot use major %d.\n", DEV_NAME, dev_major);
        return -ENODEV;
    }
    printk(KERN_DEBUG "s3c %s driver use major %d.\n", DEV_NAME, dev_major);

    if(NULL == (dev.cdev = cdev_alloc()))
    {
        printk(KERN_ERR "s3c %s driver cannot register cdev: result = %d.\n", DEV_NAME, result);
        unregister_chrdev_region(devno, 1);
        return -ENOMEM;
    }

    dev.cdev -> owner = THIS_MODULE;
    cdev_init(dev.cdev, &ds18b20_fops);

    result = cdev_add(dev.cdev, devno, 1);
    if(0 != result)
    {
        printk(KERN_ERR "s3c %s driver cannot register cdev: result = %d.\n", DEV_NAME, result);
        goto ERROR;
    }
	dev.class  = class_create(THIS_MODULE, DEV_NAME);
	device_create(dev.class, NULL, MKDEV(dev_major, dev_minor), NULL, DEV_NAME);
    printk(KERN_ERR "s3c %s driver[major=%d] installed successfully.\n",DEV_NAME, dev_major);
	
	
    return 0;

ERROR:
    printk(KERN_ERR "s3c %s driver installed failure./", DEV_NAME);
    cdev_del(dev.cdev);
    unregister_chrdev_region(devno, 1);
    return result;
}

static void __exit s3c_ds18b20_exit(void)
{
    dev_t devno = MKDEV(dev_major, dev_minor);

    cdev_del(dev.cdev);
	device_destroy(dev.class,devno);
    class_destroy(dev.class);
    unregister_chrdev_region(devno, 1);

    printk(KERN_ERR "s3c %s driver removed!\n", DEV_NAME);
    return;
}

module_init(s3c_ds18b20_init);
module_exit(s3c_ds18b20_exit);

MODULE_AUTHOR(DRV_AUTHOR);
MODULE_DESCRIPTION(DRV_DESC);
MODULE_LICENSE("GPL");
