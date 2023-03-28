/*********************************************************************************
 *      Copyright:  (C) 2019 none
 *                  All rights reserved.
 *
 *       Filename:  test.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(2019年07月18日)
 *         Author:  zhanghang <1711318490@qq.com>
 *      ChangeLog:  1, Release initial version on "2019年07月18日 16时05分12秒"
 *                 
 ********************************************************************************/

#include <stdio.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <termios.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>

int main (int argc, char **argv)
{
    int fd;
    double result = 0;
    unsigned char buff[2];
    unsigned short temp = 0;
    int flag = 0;

    if ((fd=open("/dev/ds18b20",O_RDWR | O_NDELAY | O_NOCTTY)) < 0)
    {
        perror("open device ds18b20 failed.\r\n");
        exit(1);
    }
    while(1)
    {
        printf("open device ds18b20 success.\r\n");
        read(fd, buff, sizeof(buff));
        temp=((unsigned short)buff[1])<<8;
        temp|=(unsigned short)buff[0];
        result=0.0625*((double)temp);
        printf("temperature is %4f \r\n", result);
        sleep(2);
    }
    close(fd);

    return 0;
}
