/*********************************************************************************
 *      Copyright:  (C) 2019 none
 *                  All rights reserved.
 *
 *       Filename:  test.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(2019年05月24日)
 *         Author:  zhanghang <1711318490@qq.com>
 *      ChangeLog:  1, Release initial version on "2019年05月24日 17时34分34秒"
 *                 
 ********************************************************************************/

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#define PLATDRV_MAGIC             0x60
#define LED_OFF                   _IO (PLATDRV_MAGIC, 0x18)
#define LED_ON                    _IO (PLATDRV_MAGIC, 0x19)
#define LED_BLINK                 _IO (PLATDRV_MAGIC, 0x20)

int main()
{
    int fd=open("/dev/led",O_RDWR,0644);
    if(fd<0)
    {
        printf("open error\n");
        return -1;
    }
    while(1)
    {
        ioctl(fd,LED_ON,0);
        ioctl(fd,LED_ON,1);
        ioctl(fd,LED_ON,2);
        ioctl(fd,LED_ON,3);
        sleep(3);
        ioctl(fd,LED_BLINK,0);
        ioctl(fd,LED_BLINK,1);
        ioctl(fd,LED_BLINK,2);
        ioctl(fd,LED_BLINK,3);
        sleep(3);
        ioctl(fd,LED_OFF,0);
        ioctl(fd,LED_OFF,1);
        ioctl(fd,LED_OFF,2);
        ioctl(fd,LED_OFF,3);
        sleep(3);
    }
    return 0;
}
