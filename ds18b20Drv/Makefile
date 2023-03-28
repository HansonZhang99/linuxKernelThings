LINUX_SRC=${shell pwd}/../../linux/linux-3.0/
CROSS_COMPILE=armgcc
INST_PATH=/tftp

PWD:=${shell pwd}
EXTRA_CFLAGS +=-DMODULE

#obj-m +=test.o
obj-m +=ds18b20.o
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

