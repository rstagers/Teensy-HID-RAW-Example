/*
 * Hidraw Userspace Example
 *
 * Copyright (c) 2010 Alan Ott <alan@signal11.us>
 * Copyright (c) 2010 Signal 11 Software
 *
 * The code may be used by anyone for any purpose,
 * and can serve as a starting point for developing
 * applications using hidraw.
 */

/* Linux */
#include <linux/types.h>
#include <linux/input.h>
#include <linux/hidraw.h>

/*
 * Ugly hack to work around failing compilation on systems that don't
 * yet populate new version of hidraw.h to userspace.
 */
#ifndef HIDIOCSFEATURE
#warning Please have your distro update the userspace kernel headers
#define HIDIOCSFEATURE(len)    _IOC(_IOC_WRITE|_IOC_READ, 'H', 0x06, len)
#define HIDIOCGFEATURE(len)    _IOC(_IOC_WRITE|_IOC_READ, 'H', 0x07, len)
#endif

/* Unix */
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

/* C */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>

const char *bus_str(int bus);

void dump_data( unsigned char * buffer, unsigned int length)
{
   unsigned int    i;
   unsigned int    wholelines;
   unsigned char * current = buffer;
 
   if (length == 0) {
       printf("\n");
       return;
   }
 
   wholelines = length / 16;
 
   for (i=0; i < wholelines; i++) {
      printf("%05X: %02X %02X %02X %02X %02X %02X %02X %02X-"
                   "%02X %02X %02X %02X %02X %02X %02X %02X  "
                   "%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\n",
                (unsigned int)(current-buffer),
                current[0], current[1], current[2], current[3],
                current[4], current[5], current[6], current[7],
                current[8], current[9], current[10],current[11],
                current[12],current[13],current[14],current[15],
                isprint(current[0])  ? current[0] : '.',
                isprint(current[1])  ? current[1] : '.',
                isprint(current[2])  ? current[2] : '.',
                isprint(current[3])  ? current[3] : '.',
                isprint(current[4])  ? current[4] : '.',
                isprint(current[5])  ? current[5] : '.',
                isprint(current[6])  ? current[6] : '.',
                isprint(current[7])  ? current[7] : '.',
                isprint(current[8])  ? current[8] : '.',
                isprint(current[9])  ? current[9] : '.',
                isprint(current[10]) ? current[10] : '.',
                isprint(current[11]) ? current[11] : '.',
                isprint(current[12]) ? current[12] : '.',
                isprint(current[13]) ? current[13] : '.',
                isprint(current[14]) ? current[14] : '.',
                isprint(current[15]) ? current[15] : '.' );
 
      current += 16;
      length  -= 16;
   }
 
   if (length) {
      printf("%05X: ", (unsigned int)(current-buffer));
      for (i=0; i<length; i++) {
         printf("%02X ", current[i]);
      }
 
      for (; i < 16; i++) printf("   "); printf(" ");
 
      for (i=0; i < length; i++) {
         printf("%c", isprint(current[i]) ? current[i] : '.');
      }
      printf("\n");
   }
   else
      printf("\n");

   return;
}

int main(int argc, char **argv)
{
	int fd;
	int i, res, desc_size = 0;
	unsigned char buf[256];
	unsigned char inc = 0;
	struct hidraw_report_descriptor rpt_desc;
	struct hidraw_devinfo info;
	char *device = "/dev/hidraw0";

	if (argc > 1)
		device = argv[1];

	/* Open the Device with non-blocking reads. In real life,
	   don't use a hard coded path; use libudev instead. */
	fd = open(device, O_RDWR);
//	fd = open(device, O_RDWR|O_NONBLOCK);

	if (fd < 0) {
		perror("Unable to open device");
		return 1;
	}

	memset(&rpt_desc, 0x0, sizeof(rpt_desc));
	memset(&info, 0x0, sizeof(info));
	memset(buf, 0x0, sizeof(buf));

	/* Get Report Descriptor Size */
	res = ioctl(fd, HIDIOCGRDESCSIZE, &desc_size);
	if (res < 0)
		perror("HIDIOCGRDESCSIZE");
	else
		printf("Report Descriptor Size: %d\n", desc_size);

	/* Get Report Descriptor */
	rpt_desc.size = desc_size;
	res = ioctl(fd, HIDIOCGRDESC, &rpt_desc);
	if (res < 0) {
		perror("HIDIOCGRDESC");
	} else {
		printf("Report Descriptor:\n");
		for (i = 0; i < rpt_desc.size; i++)
			printf("%hhx ", rpt_desc.value[i]);
		puts("\n");
	}

	/* Get Raw Name */
	res = ioctl(fd, HIDIOCGRAWNAME(256), buf);
	if (res < 0)
		perror("HIDIOCGRAWNAME");
	else
		printf("Raw Name: %s\n", buf);

	/* Get Physical Location */
	res = ioctl(fd, HIDIOCGRAWPHYS(256), buf);
	if (res < 0)
		perror("HIDIOCGRAWPHYS");
	else
		printf("Raw Phys: %s\n", buf);

	/* Get Raw Info */
	res = ioctl(fd, HIDIOCGRAWINFO, &info);
	if (res < 0) {
		perror("HIDIOCGRAWINFO");
	} else {
		printf("Raw Info:\n");
		printf("\tbustype: %d (%s)\n",
			info.bustype, bus_str(info.bustype));
		printf("\tvendor: 0x%04hx\n", info.vendor);
		printf("\tproduct: 0x%04hx\n", info.product);
	}

	/* Set Feature */
//	buf[0] = 0x9; /* Report Number */
//	buf[1] = 0xff;
//	buf[2] = 0xff;
//	buf[3] = 0xff;
//	res = ioctl(fd, HIDIOCSFEATURE(4), buf);
//	if (res < 0)
//		perror("HIDIOCSFEATURE");
//	else
//		printf("ioctl HIDIOCGFEATURE returned: %d\n", res);

	/* Get Feature */
//	buf[0] = 0x9; /* Report Number */
//	res = ioctl(fd, HIDIOCGFEATURE(256), buf);
//	if (res < 0) {
//		perror("HIDIOCGFEATURE");
//	} else {
//		printf("ioctl HIDIOCGFEATURE returned: %d\n", res);
//		printf("Report data (not containing the report number):\n\t");
//		for (i = 0; i < res; i++)
//			printf("%hhx ", buf[i]);
//		puts("\n");
//	}

// The Teensy code expects a 64 byte packet, no report number!
// The bits of the first byte sent are used to set ouputs (0-7) on the Teensy.

	/* Send a Report to the Device */
	//buf[0] = inc++; /* Report Number */
	//buf[1] = 0x77;
	//res = write(fd, buf, 64);
	//if (res < 0) {
	//	printf("Error: %d\n", errno);
	//	perror("write");
	//} else {
	//	printf("write() wrote %d bytes\n", res);
	//}

	
	for(;;)
	{
		// The Teensy code expects a 64 byte packet, no report number!
		// The bits of the first byte sent are used to set ouputs (0-7) on the Teensy.

		/* Send a Report to the Device */

		buf[0] = inc;
		inc++;
		if(inc == 10)
			inc = 0;

 		buf[1] = 0x00;
		res = write(fd, buf, 64);
		if (res < 0) {
			printf("Error: %d\n", errno);
			perror("write");
		} else {
			printf("write() wrote %d bytes\n", res);
		}

		/* Teensy sends a report every two seconds.  Get a report from the device */
		res = read(fd, buf, 64);
		if (res < 0) {
			perror("read");
		} else {
			printf("read() read %d bytes:\n", res);
			dump_data(buf, res);
			puts("\n");

			// buf[2] buf[3] is the temp from the sensor...  decode and display the temp!
			float code = (buf[2] << 8) | buf[3];
			float celsius;
			float fahrenheit;
			if (code <= 289) {
				celsius = 5 + (code - 289) / 9.82;
			  }
			  if (code > 289 && code <= 342) {
				celsius = 10 + (code - 342) / 10.60;
			  }
			  if (code > 342 && code <= 398) {
				celsius = 15 + (code - 398) / 11.12;
			  }
			  if (code > 398 && code <= 455) {
				celsius = 20 + (code - 455) / 11.36;
			  }
			  if (code > 455 && code <= 512) {
				celsius = 25 + (code - 512) / 11.32;
			  }
			  if (code > 512 && code <= 566) {
				celsius = 30 + (code - 566) / 11.00;
			  }
			  if (code > 566 && code <= 619) {
				celsius = 35 + (code - 619) / 10.44;
			  }
			  if (code > 619 && code <= 667) {
				celsius = 40 + (code - 667) / 9.73;
			  }
			  if (code > 667) {
				celsius = 45 + (code - 712) / 8.90;
			  }
			  fahrenheit = celsius * 1.8 + 32;

			printf("Tempreture: %.1fF (%.1fC)\n", fahrenheit, celsius);			
		}
	}
	close(fd);
	return 0;
}

const char *
bus_str(int bus)
{
	switch (bus) {
	case BUS_USB:
		return "USB";
		break;
	case BUS_HIL:
		return "HIL";
		break;
	case BUS_BLUETOOTH:
		return "Bluetooth";
		break;
	case BUS_VIRTUAL:
		return "Virtual";
		break;
	default:
		return "Other";
		break;
	}
}
