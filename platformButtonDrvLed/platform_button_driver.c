/*********************************************************************************
 *      Copyright:  (C) 2019 none
 *                  All rights reserved.
 *    
 *       Filename:  platform_button_driver.c
 *    Description:  This file 
 *                         
 *        Version:  1.0.0(2019年05月29日)
 *         Author:  zhanghang <1711318490@qq.com>
 *      ChangeLog:  1, Release initial version on "2019年05月29日 09时10分05秒"
 *                                      
 *********************************************************************************/

#ifndef __S3C_DRIVER_H
#define __S3C_DRIVER_H
 
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/kref.h>
#include <linux/spinlock.h>
#include <asm/uaccess.h>
#include <linux/mutex.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/i2c.h>
#include <linux/string.h>
#include <linux/bcd.h>
#include <linux/miscdevice.h>
#include <linux/poll.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <linux/sysfs.h>
#include <linux/proc_fs.h>
#include <linux/rtc.h>
#include <linux/spinlock.h>
#include <linux/usb.h>
#include <asm/uaccess.h>
#include <asm/delay.h>
#include <linux/syscalls.h>  /*  For sys_access*/
#include <linux/platform_device.h>
#include <linux/unistd.h>
#include <linux/tty.h>
#include <linux/tty_flip.h>
#include <linux/serial.h>
#include <linux/serial_core.h>
#include <linux/irq.h>
#include <mach/regs-gpio.h>
 
 
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,32)
#include <mach/hardware.h>
#include <mach/gpio.h>
#include <asm/irq.h>
#else
#include <asm-arm/irq.h>
#include <asm/arch/gpio.h>
#include <asm/arch/hardware.h>
#endif
 
 
 
/*===========================================================================
 ***         S3C24XX device driver common macro definition
 ***===========================================================================*/

#define ENPULLUP                    1
#define DISPULLUP                   0
 
#define HIGHLEVEL                   1
#define LOWLEVEL                    0
 
#define INPUT                       1
#define OUTPUT                      0
 
#define OFF                         0
#define ON                          1
 
#define ENABLE                      1
#define DISABLE                     0
 
#define TRUE                        1
#define FALSE                       0
 
 
/****** Bit Operate Define *****/
#define SET_BIT(data, i)   ((data) |=  (1 << (i)))    /*    Set the bit "i" in "data" to 1  */
#define CLR_BIT(data, i)   ((data) &= ~(1 << (i)))    /*    Clear the bit "i" in "data" to 0 */
#define NOT_BIT(data, i)   ((data) ^=  (1 << (i)))    /*    Inverse the bit "i" in "data"  */
#define GET_BIT(data, i)   ((data) >> (i) & 1)        /*    Get the value of bit "i"  in "data" */
#define L_SHIFT(data, i)   ((data) << (i))            /*    Shift "data" left for "i" bit  */
#define R_SHIFT(data, i)   ((data) >> (i))            /*    Shift "data" Right for "i" bit  */
 
 
/*===========================================================================
***         S3C24XX device driver common function definition
***===========================================================================*/
 
#define SLEEP(x)    {DECLARE_WAIT_QUEUE_HEAD (stSleep); if (10 > x) mdelay ((x * 1000)); \
                            else wait_event_interruptible_timeout (stSleep, 0, (x / 10));}
 
#define VERSION_CODE(a,b,c)       ( ((a)<<16) + ((b)<<8) + (c))
#define DRV_VERSION               VERSION_CODE(DRV_MAJOR_VER, DRV_MINOR_VER, DRV_REVER_VER)
#define MAJOR_VER(a)              ((a)>>16&0xFF)
#define MINOR_VER(a)              ((a)>>8&0xFF)
#define REVER_VER(a)              ((a)&0xFF)
 
#define dbg_print(format,args...) if(DISABLE!=debug) \
                                                      {printk("[kernel] ");printk(format, ##args);}
 
static inline void print_version(int version)
{
#ifdef __KERNEL__
    printk("%d.%d.%d\n", MAJOR_VER(version), MINOR_VER(version), REVER_VER(version));
#else
    printf("%d.%d.%d\n", MAJOR_VER(version), MINOR_VER(version), REVER_VER(version));
#endif
}
 
 
#endif /*  __S3C_DRIVER_H */
  
 
  
/* Driver version*/  
#define DRV_MAJOR_VER             1  
#define DRV_MINOR_VER             0  
#define DRV_REVER_VER             0  
  
#define DEV_BUTTON_NAME "key"
#define DEV_NAME                  DEV_BUTTON_NAME  
  
//#define DEV_MAJOR               DEV_BUTTON_MAJOR  
#ifndef DEV_MAJOR  
#define DEV_MAJOR                 0 /* dynamic major by default */  
#endif  
  
#define BUTTON_UP                 0 /* Button status is up */  
#define BUTTON_DOWN               1 /* Button status is pushed down */  
#define BUTTON_UNCERTAIN          2 /* Button status uncerntain */  
    
#define TIMER_DELAY_DOWN          (HZ/50)   /*Remove button push down dithering timer delay 20ms  */  
#define TIMER_DELAY_UP            (HZ/10)   /*Remove button up dithering timer delay 100ms  */  
      
static int debug = DISABLE;  
static int dev_major = DEV_MAJOR;  
static int dev_minor = 0;  
 
/* Button hardware informtation structure*/  
struct s3c_button_info  
{  
    unsigned char           num;       /*Button nubmer  按键号*/    
    char *                  name;      /*Button nubmer  按键名*/  
    int                     nIRQ;      /*Button IRQ number 中断号*/  
    unsigned int            setting;   /*Button IRQ Pin Setting 中断引脚配置*/  
    unsigned int            gpio;      /*Button GPIO port 对应的IO引脚*/  
};  
  
/* The button plaotform device private data structure */  
struct s3c_button_platform_data //按键数据结构体  
{  
    struct s3c_button_info *buttons; //用来访问按键硬件信息的指针  
    int                     nbuttons;//按键数量  
};  
  
/* Button hardware informtation data*/ //具体的相应按键信息  
static struct s3c_button_info  s3c_buttons[] = {  
    [0] = {  
        .num = 1,  
        .name = "KEY1",  
        .nIRQ = IRQ_EINT0,//中断号  
        .gpio = S3C2410_GPF(0),  
        .setting = S3C2410_GPF0_EINT0,//datasheet手册上对应的IO中断口  
    },  
    [1] = {  
        .num = 2,  
        .name = "KEY2",  
        .nIRQ = IRQ_EINT2,  
        .gpio = S3C2410_GPF(2),  
        .setting = S3C2410_GPF2_EINT2,  
    },  
    [2] = {  
        .num = 3,  
        .name = "KEY3",  
        .nIRQ = IRQ_EINT3,  
        .gpio = S3C2410_GPF(3),  
        .setting = S3C2410_GPF3_EINT3,  
    },  
    [3] = {  
        .num = 4,  
        .name = "KEY4",  
        .nIRQ = IRQ_EINT4,  
        .gpio = S3C2410_GPF(4),  
        .setting = S3C2410_GPF4_EINT4,  
    },  
};  
  
/* The button platform device private data */  
static struct s3c_button_platform_data s3c_button_data = {  
    .buttons = s3c_buttons,  
    .nbuttons = ARRAY_SIZE(s3c_buttons),  
};  
struct button_device  
{  
    unsigned char                      *status;      /* The buttons Push down or up status */  
    struct s3c_button_platform_data    *data;        /* The buttons hardware information data */  
  
    struct timer_list                  *timers;      /* The buttons remove dithering timers */  
    wait_queue_head_t                  waitq;           /* Wait queue for poll()  */  
    volatile int                       ev_press;     /* Button pressed event */  
  
    struct cdev                        cdev;             
    struct class                       *dev_class;   
} button_device;  
  
static void platform_button_release(struct device * dev)  
{  
    return;   
}  
  
static struct platform_device s3c_button_device = {  
    .name    = "s3c_button",  
    .id      = 1,  
    .dev     =   
    {  
        .platform_data = &s3c_button_data,   
        .release = platform_button_release,  
    },  
};  
  
static irqreturn_t s3c_button_intterupt(int irq,void *de_id) //按键中断服务程序  
{  
    int i;  
    int found = 0;  
    struct s3c_button_platform_data *pdata = button_device.data;  
  for(i=0; i<pdata->nbuttons; i++)  
    {  
        if(irq == pdata->buttons[i].nIRQ)//找到具体的中断号  
        {  
            found = 1;   
            break;  
        }  
    }  
  
    if(!found) /* An ERROR interrupt  */  
        return IRQ_NONE;  
  
    /* Only when button is up then we will handle this event */  
    if(BUTTON_UP  == button_device.status[i])  
    {  
       button_device.status[i] = BUTTON_UNCERTAIN;//延时消抖，且不确定是否为有效按键,所以先设置为不确定状态  
       mod_timer(&(button_device.timers[i]), jiffies+TIMER_DELAY_DOWN);  
    }  
  
    return IRQ_HANDLED;  
}  
  
static void button_timer_handler(unsigned long data) //定时器中断服务程序  
{  
    struct s3c_button_platform_data *pdata = button_device.data;  
    int num =(int)data;  
    int status = s3c2410_gpio_getpin( pdata->buttons[num].gpio );  
  
    if(LOWLEVEL == status)  
    {  
        if(BUTTON_UNCERTAIN == button_device.status[num]) /* Come from interrupt */  
        {  
          button_device.status[num] = BUTTON_DOWN;  
  
            printk("%s pressed.\n", pdata->buttons[num].name);  
  
            /* Wake up the wait queue for read()/poll() */  
            button_device.ev_press = 1;  
            wake_up_interruptible(&(button_device.waitq)); //ev_press唤醒等待队列  
        }  
  
        /* Cancel the dithering  */  
        mod_timer(&(button_device.timers[num]), jiffies+TIMER_DELAY_UP);//重新激活并设置定时器  
    }  
    else  
    {  
            button_device.status[num] = BUTTON_UP;  
    }
    return ;
}
 
/*===================== Button device driver part ===========================*/  
  
static int button_open(struct inode *inode, struct file *file)  
{   
    struct button_device *pdev ;  
    struct s3c_button_platform_data *pdata;  
    int i, result;  
  
    pdev = container_of(inode->i_cdev,struct button_device, cdev);  
    pdata = pdev->data;  
    file->private_data = pdev;  
  
    /* Malloc for all the buttons remove dithering timer */  
    pdev->timers = (struct timer_list *) kmalloc(pdata->nbuttons*sizeof(struct timer_list), GFP_KERNEL);  
    if(NULL == pdev->timers)//内核里的内存申请失败  
    {  
        printk("Alloc %s driver for timers failure.\n", DEV_NAME);  
        return -ENOMEM;  
    }  
    memset(pdev->timers, 0, pdata->nbuttons*sizeof(struct timer_list));  
  
    /* Malloc for all the buttons status buffer */  
    pdev->status = (unsigned char *)kmalloc(pdata->nbuttons*sizeof(unsigned char), GFP_KERNEL);  
    if(NULL == pdev->status)  
    {  
        printk("Alloc %s driver for status failure.\n", DEV_NAME);  
        result = -ENOMEM;   
        goto  ERROR;  
    }  
    memset(pdev->status, 0, pdata->nbuttons*sizeof(unsigned char));  
  
    init_waitqueue_head(&(pdev->waitq));//初始化等待队列头  
    
    for(i=0; i<pdata->nbuttons; i++)   
    {  
        /* Initialize all the buttons status to UP  */  
        pdev->status[i] = BUTTON_UP;   
  
        /* Initialize all the buttons' remove dithering timer */  
        setup_timer(&(pdev->timers[i]), button_timer_handler, i); //初始化每个定时器  
  
        /* Set all the buttons GPIO to EDGE_FALLING interrupt mode */  
        s3c2410_gpio_cfgpin(pdata->buttons[i].gpio, pdata->buttons[i].setting);//按键相应的管脚配置成IRQ中断模式  
        irq_set_irq_type(pdata->buttons[i].nIRQ, IRQ_TYPE_EDGE_FALLING);//把相应的中断号设置为下降沿触发方式  
  
        /* Request for button GPIO pin interrupt  */  
        result = request_irq(pdata->buttons[i].nIRQ, s3c_button_intterupt, IRQF_DISABLED, DEV_NAME, (void *)i);//注册给内核，一旦发生中断号的中断就调用s3c_button_intterupt这个中断处理程序  
        if( result )  
        {  
            result = -EBUSY;  
            goto ERROR1;  
        }  
    }  
  
    return 0;  
  
ERROR1:  
     kfree((unsigned char *)pdev->status);  
     while(--i)   
     {   
         disable_irq(pdata->buttons[i].nIRQ);   
         free_irq(pdata->buttons[i].nIRQ, (void *)i);   
     }  
ERROR:  
     kfree(pdev->timers);  
  
     return result;  
}  
  
static int button_read(struct file *file, char __user *buf, size_t count, loff_t *ppos)  
{   
    struct button_device *pdev = file->private_data;  
    struct s3c_button_platform_data *pdata;  
    int   i, ret;  
    unsigned int status = 0;  
  
    pdata = pdev->data;  
  
    dbg_print("ev_press: %d\n", pdev->ev_press);  
    if(!pdev->ev_press) //ev_press为按键标识符，即按下时才为1.结合下面等待队列程序  
    {  
         if(file->f_flags & O_NONBLOCK)  
         {  
             dbg_print("read() without block mode.\n");// O_NONBLOCK是设置为非阻塞模式  
             return -EAGAIN;//若没有按键按下，直接返回  
         }  
         else  
         {  
             /* Read() will be blocked here */  
             dbg_print("read() blocked here now.\n");  
             wait_event_interruptible(pdev->waitq, pdev->ev_press); //在阻塞模式读取且没按键按下则让等待队列进入睡眠，直到ev_press为1  
         }  
    }  
  
    pdev->ev_press = 0;//清除标识，准备下一次  
    
    for(i=0; i<pdata->nbuttons; i++)  
    {  
        dbg_print("button[%d] status=%d\n", i, pdev->status[i]);  
        status |= (pdev->status[i]<<i);   
    }  
  
    ret = copy_to_user(buf, (void *)&status, min(sizeof(status), count));  
  
    return ret ? -EFAULT : min(sizeof(status), count);  
}  
  
static unsigned int button_poll(struct file *file, poll_table * wait)//类似于应用层的select轮询监听  
{   
    struct button_device *pdev = file->private_data;  
    unsigned int mask = 0;  
  
    poll_wait(file, &(pdev->waitq), wait);//添加等待队列到等待队列中  
    if(pdev->ev_press)  
    {  
        mask |= POLLIN | POLLRDNORM; /* The data aviable */   
    }  
  
    return mask;  
}  
  
static int button_release(struct inode *inode, struct file *file)  
{   
    int i;  
    struct button_device *pdev = file->private_data;  
    struct s3c_button_platform_data *pdata;  
    pdata = pdev->data;  
 
     for(i=0; i<pdata->nbuttons; i++)   
    {  
        disable_irq(pdata->buttons[i].nIRQ);  
        free_irq(pdata->buttons[i].nIRQ, (void *)i);  
        del_timer(&(pdev->timers[i]));  
    }  
  
    kfree(pdev->timers);  
    kfree((unsigned char *)pdev->status);  
  
    return 0;  
}  
  
  
static struct file_operations button_fops = {   
    .owner = THIS_MODULE,  
    .open = button_open,   
    .read = button_read,  
    .poll = button_poll,   
    .release = button_release,   
};  
  
  
static int s3c_button_probe(struct platform_device *dev)  
{  
    int result = 0;  
    dev_t devno;  
  
  
    /* Alloc the device for driver  */   
    if (0 != dev_major)   
    {   
        devno = MKDEV(dev_major, dev_minor);   
        result = register_chrdev_region(devno, 1, DEV_NAME);   
    }   
     else   
    {   
        result = alloc_chrdev_region(&devno, dev_minor, 1, DEV_NAME);   
        dev_major = MAJOR(devno);   
    }  
  
    /* Alloc for device major failure */  
    if (result < 0)   
    {   
        printk("%s driver can't get major %d\n", DEV_NAME, dev_major);   
        return result;   
    }  
  
    /*  Initialize button_device structure and register cdev*/  
     memset(&button_device, 0, sizeof(button_device));  
     button_device.data = dev->dev.platform_data;  
     cdev_init (&(button_device.cdev), &button_fops);  
     button_device.cdev.owner  = THIS_MODULE;  
  
     result = cdev_add (&(button_device.cdev), devno , 1);   
     if (result)   
     {   
         printk (KERN_NOTICE "error %d add %s device", result, DEV_NAME);   
         goto ERROR;   
     }  
  
     button_device.dev_class = class_create(THIS_MODULE, DEV_NAME);   
     if(IS_ERR(button_device.dev_class))   
     {   
         printk("%s driver create class failture\n",DEV_NAME);   
         result =  -ENOMEM;   
         goto ERROR;   
     }  
 
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,24)       
     device_create(button_device.dev_class, NULL, devno, NULL, DEV_NAME);  
#else  
     device_create (button_device.dev_class, NULL, devno, DEV_NAME);  
#endif  
  
     printk("S3C %s driver version %d.%d.%d initiliazed.\n", DEV_NAME, DRV_MAJOR_VER, DRV_MINOR_VER, DRV_REVER_VER);  
  
     return 0;  
  
ERROR:   
     printk("S3C %s driver version %d.%d.%d install failure.\n", DEV_NAME, DRV_MAJOR_VER, DRV_MINOR_VER, DRV_REVER_VER);  
     cdev_del(&(button_device.cdev));   
     unregister_chrdev_region(devno, 1);  
     return result;  
}  
  
  
static int s3c_button_remove(struct platform_device *dev)  
{  
    dev_t devno = MKDEV(dev_major, dev_minor);  
  
    cdev_del(&(button_device.cdev));  
    device_destroy(button_device.dev_class, devno);  
    class_destroy(button_device.dev_class);  
  
    unregister_chrdev_region(devno, 1);   
    printk("S3C %s driver removed\n", DEV_NAME);  
  
    return 0;  
}  
 
/*===================== Platform Device and driver regist part ===========================*/  
  
static struct platform_driver s3c_button_driver = {   
    .probe      = s3c_button_probe,   
    .remove     = s3c_button_remove,   
    .driver     = {   
        .name       = "s3c_button",   
        .owner      = THIS_MODULE,   
    },  
};  
  
  
static int __init s3c_button_init(void)   
{  
   int       ret = 0;  
  
   ret = platform_device_register(&s3c_button_device);  
   if(ret)  
   {  
        printk(KERN_ERR "%s: Can't register platform device %d\n", __FUNCTION__, ret);   
        goto fail_reg_plat_dev;  
   }  
   dbg_print("Regist S3C %s Device successfully.\n", DEV_NAME);  
  
   ret = platform_driver_register(&s3c_button_driver);  
   if(ret)  
   {  
        printk(KERN_ERR "%s: Can't register platform driver %d\n", __FUNCTION__, ret);   
        goto fail_reg_plat_drv;  
   }  
   dbg_print("Regist S3C %s Driver successfully.\n", DEV_NAME);  
  
   return 0;  
  
fail_reg_plat_drv:  
   platform_driver_unregister(&s3c_button_driver);  
fail_reg_plat_dev:  
   return ret;  
}  
  
  
static void s3c_button_exit(void)  
{  
    platform_driver_unregister(&s3c_button_driver);  
    dbg_print("S3C %s platform device removed.\n", DEV_NAME);  
  
    platform_device_unregister(&s3c_button_device);  
    dbg_print("S3C %s platform driver removed.\n", DEV_NAME);  
}  
  
module_init(s3c_button_init);  
module_exit(s3c_button_exit);  
  
module_param(debug, int, S_IRUGO);  
module_param(dev_major, int, S_IRUGO);  
module_param(dev_minor, int, S_IRUGO);  
  
MODULE_AUTHOR("zhanghang <1711318490@qq.com>");  
MODULE_DESCRIPTION("FL2440 button platform driver");  
MODULE_LICENSE("GPL");  
