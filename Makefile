CC = /opt/Espressif/esp-open-sdk/xtensa-lx106-elf/bin/xtensa-lx106-elf-gcc
AR = /opt/Espressif/esp-open-sdk/xtensa-lx106-elf/bin/xtensa-lx106-elf-ar
CFLAGS = -I. -mlongcalls -DICACHE_FLASH
LDLIBS = -nostdlib -Wl,--start-group -lmain -lnet80211 -lssl -lwpa -llwip -lpp -lphy -lc -Wl,--end-group -lgcc
LDFLAGS = -Teagle.app.v6.ld
ARFLAGS = -cru

ippiot-0x00000.bin: ippiot
	esptool.py elf2image $^

ippiot: ippiot.o -lIPP

ippiot.o: ippiot.c

libIPP.a: max7221.o httpclient.o jsonparse.o
	/opt/Espressif/esp-open-sdk/xtensa-lx106-elf/bin/xtensa-lx106-elf-ar $(ARFLAGS) $@ $^

max7221.o: max7221.c

httpclient.o: httpclient.c

jsonparse.o: jsonparse.c

flash: ippiot-0x00000.bin
	esptool.py --port /dev/ttyUSB0 write_flash 0 ippiot-0x00000.bin 0x10000 ippiot-0x10000.bin

clean:
	rm -f ippiot ippiot.o ippiot-0x00000.bin ippiot-0x10000.bin
