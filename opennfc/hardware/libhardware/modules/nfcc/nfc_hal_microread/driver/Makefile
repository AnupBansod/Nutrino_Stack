TARGET = open_nfc_driver

obj-m += $(TARGET).o

$(TARGET)-objs += open_nfc_main.o
$(TARGET)-objs += open_nfc_ccclient.o
$(TARGET)-objs += open_nfc_custom.o

all:
	make -C  $(ANDROID_BUILD_TOP)/kernel ARCH=arm CROSS_COMPILE=arm-eabi- M=$(PWD) modules

clean:
	make -C  $(ANDROID_BUILD_TOP)/kernel ARCH=arm CROSS_COMPILE=arm-eabi- M=$(PWD) clean


