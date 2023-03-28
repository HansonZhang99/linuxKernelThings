LINUX_SRC=${shell pwd}/../../../../gitee/fl2440/linux/linux-3.0/
CROSS_COMPILE=/opt/xtools/arm920t/bin/arm-linux-gcc
INST_PATH=${shell pwd}
PWD:=${shell pwd}
EXTRA_CFLAGS +=-DMODULE

all: target1 modules 

target1:
	${CROSS_COMPILE} test.c -o test

obj-m +=platform_led_driver.o

modules:
	@make -C ${LINUX_SRC} M=${PWD} modules
	@make clear

uninstall:
	rm -f ${INST_PATH}/*.ko

install:uninstall
	cp -af *.ko ${INST_PATH}
clear:
	@rm -f *.o *.cmd *.mod.c
	@rm -rf *~ core .depend .tmp_versions Module.symvers modules.order -f
	@rm -f .*ko.cmd .*.o.cmd .*.o.d
#	@rm -f *.unsigned

clean:
	@rm -f led.ko
	@rm -f button.ko
	@rm -f test
