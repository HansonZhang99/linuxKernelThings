#include <stdio.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/select.h>

#define LED_CNT 4
#define DEVNAME_LEN 10

#define MAGIC 0x60
#define LED_OFF _IO (MAGIC, 0x19)
#define LED_ON _IO (MAGIC, 0x18)

static inline msleep(unsigned long ms)
{
    struct timeval tv;
    tv.tv_sec = ms/1000;
    tv.tv_usec = (ms%1000)*1000;
    select(0, NULL, NULL, NULL, &tv);
}

int main (int argc, char **argv)
{
    int i;
    int fd[LED_CNT];
    char dev_name[DEVNAME_LEN]={0,0,0,0};
    for(i=0; i<LED_CNT; i++)
    {
        snprintf(dev_name, sizeof(dev_name), "/dev/led%d", i);
        fd[i] = open(dev_name, O_RDWR, 0755);
        if(fd[i] < 0)
            goto err;
    }
    while(1)
    {
        for(i=0; i<LED_CNT; i++)
        {
            ioctl(fd[i], LED_ON);
            msleep(300);
            ioctl(fd[i], LED_OFF);
            msleep(300);
            ioctl(fd[i], LED_ON);
            msleep(300);
            ioctl(fd[i], LED_OFF);
            msleep(300);
        }
    }
    for(i=0; i<LED_CNT; i++)
    {
        close(fd[i]);
    }
    return 0;
err:
    for(i=0; i<LED_CNT; i++)
    {
        if(fd[i] >= 0)
        {
            close(fd[i]);
        }
    }
    return -1;
}
