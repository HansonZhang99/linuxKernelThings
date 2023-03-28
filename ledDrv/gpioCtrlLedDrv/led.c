#include<linux/module.h>
#include<linux/init.h>
#include<linux/kernel.h>
#include<linux/fs.h>
#include<linux/errno.h>
#include<linux/cdev.h>
#include<asm/io.h>
#include<linux/ioport.h>
#include<asm/ioctl.h>
#include<linux/printk.h>

#define DEV_NAME   "led"
#define LED_NUM    4

#ifndef DEV_MAJOR
#define DEV_MAJOR 0
#endif

#define DISABLE  0
#define ENABLE   1

#define GPIO_INPUT  0x00
#define GPIO_OUTPUT 0x01

#define MAGIC 0x60
#define LED_ON _IO(MAGIC,0x18)
#define LED_OFF _IO(MAGIC,0x19)

#define GPB_BASE 0x56000010
#define GPB_LEN 0x10
#define GPBCON_OFFSET 0
#define GPBDAT_OFFSET 4
#define GPBUP_OFFSET 8


#define read_reg32(addr)  *(volatile unsigned int *)(addr)
#define write_reg32(addr,val) *(volatile unsigned int *)(addr)=(val)

static void __iomem *gpbbase=NULL;

int led[LED_NUM]={5,6,8,10};

int dev_count=ARRAY_SIZE(led);
int dev_major=DEV_MAJOR;
int dev_minor=0;
int debug=DISABLE;
static struct cdev *led_cdev;


static int led_hardware_init(void)
{
    unsigned int regval;
    int i=0;
    if(!request_mem_region(GPB_BASE,GPB_LEN,"s3c2440 led"))
    {
        printk(KERN_ERR "request_mem_region error!\n)");
        return -EBUSY;
    }

    if(!(gpbbase=(unsigned int *)ioremap(GPB_BASE,GPB_LEN)))
    {
        release_mem_region(GPB_BASE,GPB_LEN);
        printk(KERN_ERR "ioremap error\n");
        return -ENOMEM;
    }

    for(i=0;i<dev_count;i++)
    {
        regval=read_reg32(gpbbase+GPBCON_OFFSET);
        regval=regval & (~(0x03<<(2*led[i])));
        regval = regval | (GPIO_OUTPUT<<(2*led[i]));
        write_reg32(gpbbase+GPBCON_OFFSET,regval);

        regval=read_reg32(gpbbase+GPBUP_OFFSET);
        regval|=(0x01<<led[i]);
        write_reg32(gpbbase+GPBUP_OFFSET,regval);

        regval=read_reg32(gpbbase+GPBDAT_OFFSET);
        regval|=(0x01<<led[i]);
        write_reg32(gpbbase+GPBDAT_OFFSET,regval);
    }
    return 0;
}

static void turn_led(int which,unsigned int cmd)
{
    unsigned int regval;
    regval=read_reg32(gpbbase+GPBDAT_OFFSET);
    if(LED_ON == cmd)
    {
        regval&=~(0x01<<led[which]);
    }
    else if(LED_OFF==cmd)
    {
        regval|=(0x01<<led[which]);
    }
    write_reg32(gpbbase+GPBDAT_OFFSET,regval);
}

static void led_reset(void)
{
    int i;
    unsigned int regval;
    for(i=0;i<dev_count;i++)
    {
        regval=read_reg32(gpbbase+GPBDAT_OFFSET);
        regval|=(0x01<<led[i]);
        write_reg32(gpbbase+GPBDAT_OFFSET,regval);
    }
    release_mem_region(GPB_BASE,GPB_LEN);
    iounmap(gpbbase);
}

static int led_open(struct inode *inode,struct file *file)
{
    int minor=iminor(inode);//get minor device number
    file->private_data=(void *)minor;
    printk(KERN_DEBUG "/dev/led%d opened.\n",minor);
    return 0;
}
static int led_release(struct inode *inode,struct file *file)
{
    printk(KERN_DEBUG "/dev/led%d closed.\n",iminor(inode));
    return 0;
}

static void print_help(void)
{
    printk("Usage:...\n");
}

static long led_ioctl(struct file *file,unsigned int cmd,unsigned long arg)
{
    int which_s=(int)file->private_data;
    switch(cmd)
    {
        case LED_ON:
            turn_led(which_s,LED_ON);
            break;
        case LED_OFF:
            turn_led(which_s,LED_OFF);
            break;
        default:
            print_help();
            break;
    }
    return 0;
}

static struct file_operations led_fops=
{
    .owner=THIS_MODULE,
    .open=led_open,
    .release=led_release,
    .unlocked_ioctl=led_ioctl
};

static int __init led_init(void)
{
    int             result;
    dev_t           devno;
    if(led_hardware_init()!=0)
    {
        printk(KERN_ERR "s3c2440 LED hardware initilize error\n");
        return -ENODEV;
    }
    
    if (0 != dev_major) /* Static */
    {
        devno = MKDEV(dev_major, 0);
        result = register_chrdev_region(devno, dev_count, DEV_NAME);
    }
    else
    {
        result = alloc_chrdev_region(&devno, dev_minor, dev_count, DEV_NAME);
        dev_major = MAJOR(devno);
    }
    if (result < 0)
    {
        printk(KERN_ERR "S3C %s driver can't use major %d\n", DEV_NAME, dev_major);
        return -ENODEV;
    }
    printk(KERN_DEBUG "S3C %s driver use major %d\n", DEV_NAME, dev_major);

    //动态分配cdev结构体
    if(NULL==(led_cdev=cdev_alloc()))
    {
        printk(KERN_ERR "%s driver can not alloc for the cdev.\n",DEV_NAME);
        unregister_chrdev_region(devno,dev_count);
        return -ENOMEM;
    }
    //绑定主次设备号，fops到cdev结构体，注册到Linux内核
    led_cdev->owner=THIS_MODULE;
    cdev_init(led_cdev,&led_fops);
    result=cdev_add(led_cdev,devno,dev_count);
    if(0!=result)
    {
        printk(KERN_INFO "%s dirver can not register cdev:result=%d\n",DEV_NAME,result);
        goto ERROR;
    }
    printk(KERN_ERR "%s dirver major[%d] install successfully\n",DEV_NAME,dev_major);
    return 0;


ERROR:
    printk(KERN_ERR "%s driver install failure\n",DEV_NAME);
    cdev_del(led_cdev);
    unregister_chrdev_region(devno,dev_count);
    return result;
}

static void __exit led_exit(void)
{
    dev_t devno=MKDEV(dev_major,dev_minor);
    led_reset();
    cdev_del(led_cdev);
    unregister_chrdev_region(devno,dev_count);
    printk(KERN_ERR "%s dirver removed\n",DEV_NAME);
    return ;
}


module_init(led_init);
module_exit(led_exit);

module_param(debug,int,S_IRUGO);
module_param(dev_major,int,S_IRUGO);

MODULE_AUTHOR("zhanghang<1711318490@qq.com>");
MODULE_DESCRIPTION("FL2440");
MODULE_LICENSE("GPL");

