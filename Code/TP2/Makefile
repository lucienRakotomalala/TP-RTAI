# Remplacer foo par le fichier objet (.o) compilé
obj-m	:= tp2_gen_sign.o

KDIR	:= /lib/modules/$(shell uname -r)/build
PWD		:= $(shell pwd)
EXTRA_CFLAGS := -I/usr/realtime/include -I/usr/include/ -I/usr/local/include  -D__IN_RTAI__ -ffast-math -mhard-float


default: clean
	$(MAKE) -lcomedi -lm -C $(KDIR) SUBDIRS=$(PWD) modules
	chmod +x $(obj-m)
	
clean :
	rm -f  $(obj-m)
