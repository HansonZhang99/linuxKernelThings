#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<fcntl.h>

#define ON 1
#define OFF 0
int main(int argc, char *argv[])
{
    int flag = 0;
    int fd = open("/dev/s3c2440_buzzer", O_RDWR);
    if(fd < 0)
        printf("open error:%d\n", errno);
    ioctl(fd,ON,0);
    sleep(2);
    ioctl(fd,OFF,0);
    return 0;
}

