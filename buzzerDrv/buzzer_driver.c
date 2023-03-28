/*********************************************************************************
 *      Copyright:  (C) 2019 none
 *                  All rights reserved.
 *
 *       Filename:  buzzer_driver.c
 *    Description:  buzzer_driver
 *                 
 *        Version:  1.0.0(2019年07月21日)
 *         Author:  zhanghang <1711318490@qq.com>
 *      ChangeLog:  1, Release initial version on "2019年07月21日 16时39分07秒"
 *                 
 ********************************************************************************/

#include<linux/init.h>
#include<linux/module.h>
#include<linux/fs.h>
#include<linux/miscdevice.h>
#include<linux/gpio.h>
#include<mach/regs-gpio.h>
#define DEV_NAME "s3c2440_buzzer"
#ifndef ON
#define ON 1
#endif
#ifndef OFF
#define OFF 0
#endif

int buzzer_open(struct inode * node,struct file *file)
{
    s3c2410_gpio_cfgpin(S3C2410_GPB(0), S3C2410_GPIO_OUTPUT);//set GPB(0) to output
    s3c2410_gpio_setpin(S3C2410_GPB(0),OFF);
    return 0;
}

long buzzer_ioctl(struct file *file, unsigned int cmd, unsigned long  arg)
{
    switch(cmd)
    {  
        case ON:
            s3c2410_gpio_setpin(S3C2410_GPB(0),ON);
            break;
        default:
            s3c2410_gpio_setpin(S3C2410_GPB(0),OFF);
            break;
    }       
    return 0;
}

int buzzer_release(struct inode *inode, struct file *file)
{
    return 0;
}

static struct file_operations buzzer_fops=
{
    .owner=THIS_MODULE,
    .unlocked_ioctl=buzzer_ioctl,
    .open=buzzer_open,
    .release=buzzer_release,
};

static struct miscdevice misc={
    .minor=MISC_DYNAMIC_MINOR,
    .name=DEV_NAME,
    .fops=&buzzer_fops,
};

static int __init buzzer_init(void)
{
    int res;
    res=misc_register(&misc);
    if(res)
    {
        printk("register misc device failure\n");
        return -1;
    }
    printk("buzzer init!\n");
    return 0;
}


static void __exit buzzer_exit(void)
{
    int res;
    res=misc_deregister(&misc);
    if(res)
    {
        printk("deregister misc device failure\n");
        return ;
    }
    printk("buzzer exit!\n");
}


module_init(buzzer_init);
module_exit(buzzer_exit);
MODULE_AUTHOR("zhanghang<1711318490@qq.com>");
MODULE_DESCRIPTION("FL2440 buzzer misc driver");
MODULE_LICENSE("GPL");
