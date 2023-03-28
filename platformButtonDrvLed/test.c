#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <poll.h>
#include <string.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/time.h>
#define KEY1 0x1
#define KEY2 0x2
#define KEY3 0x4
#define KEY4 0x8

#define PLATDRV_MAGIC   0x60
#define LED_OFF             _IO(PLATDRV_MAGIC,0x18)
#define LED_ON              _IO(PLATDRV_MAGIC,0x19)
#define LED_BLINK           _IO(PLATDRV_MAGIC,0x20)
#define LED_NAME    "/dev/led"
#define BUTTON_NAME "/dev/key"

int main(void)
{
    int led_fd,button_fd,rv;
    fd_set rd;

    int flag1=0;
    int flag2=0;
    int flag3=0;
    int flag4=0;

    unsigned int status=0;
    led_fd=open(LED_NAME,O_RDWR,0644);
    if(led_fd<0)
    {
        printf("open %s failure:%s\n",LED_NAME,strerror(errno));
        return -1;
    }
    button_fd=open(BUTTON_NAME,O_RDWR,0644);
    if(button_fd<0)
    {
        printf("open %s failure:%s\n",BUTTON_NAME,strerror(errno));
        return -2;
    }
    while(1)
    {
        FD_ZERO(&rd);
        FD_SET(button_fd,&rd);
        rv=select(button_fd+1,&rd,NULL,NULL,NULL);
        if(rv<=0)
        {
            printf("select failure:%s\n",strerror(errno));
            return -3;
        }
        else if(rv>0)
        {
            if(FD_ISSET(button_fd,&rd))
            {
                read(button_fd,&status,sizeof(unsigned int));
                if(status & KEY1)
                {
                    if(flag1==0)
                    {
                        printf("led1 blink\n");
                        ioctl(led_fd,LED_BLINK,0);
                        flag1=1;
                    }
                    else
                    {
                        printf("led1 off\n");
                        ioctl(led_fd,LED_OFF,0);
                        flag1=0;
                    }
                }
                else if(status & KEY2)
                {
                    if(flag2==0)
                    {
                        printf("led2 on\n");
                        ioctl(led_fd,LED_ON,1);
                        flag2=1;
                    }
                    else
                    {
                        printf("led2 off\n");
                        ioctl(led_fd,LED_OFF,1);
                        flag2=0;
                    }
                }
                else if(status & KEY3)
                {
                    if(flag3==0)
                    {
                        printf("led3 on\n");
                        ioctl(led_fd,LED_ON,2);
                        flag3=1;
                    }
                    else
                    {
                        printf("led3 off\n");
                        ioctl(led_fd,LED_OFF,2);
                        flag3=0;
                    }
                }
                else if(status & KEY4)
                {
                    if(flag4==0)
                    {
                        printf("led4 on\n");
                        ioctl(led_fd,LED_ON,3);
                        flag4=1;
                    }
                    else
                    {
                        printf("led4 off\n");
                        ioctl(led_fd,LED_OFF,3);
                        flag4=0;
                    }
                }
            }
        }
    }
    close(led_fd);
    close(button_fd);
    return 0;
}


