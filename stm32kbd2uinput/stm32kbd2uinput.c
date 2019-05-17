#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <inttypes.h>
#include <linux/uinput.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>

uint8_t debug = 0; // TODO make configuable

const char* kbd_device = "/dev/irmp_stm32_kbd"; // TODO make configuable
//const char* kbd_device = "/dev/input/event4"; // TODO make configuable

int write_event(int fd, __u16 type, __u16 code, __s32 value)
{
	struct input_event event;

	memset(&event, 0, sizeof(event));
	event.type = type;
	event.code = code;
	event.value = value;

	if (write(fd, &event, sizeof(event)) != sizeof(event)) {
		fprintf(stderr, "Error on write_event\n");
		return -1;
	}

	return 0;
}

int main(void)
{
	struct uinput_setup usetup;
	int fd, ufd;
	struct input_id id;
	struct input_event event;
	uint8_t release_needed = 0, value = 0, pair = 0, skip = 0;
	long int this_time, last_sent = 0, last_received = 0;
	uint16_t delay = 250, period = 150, timeout = 130, code = 0, code2 = 0; // TODO make configurable
	struct timespec now;

	fd = open(kbd_device, O_RDONLY | O_NONBLOCK);
	if(fd == -1){
		printf("Cannot open %s. %s.\n", kbd_device, strerror(errno));
		return -1;
	}

	if(ioctl(fd, EVIOCGRAB, 1)){
		printf("Cannot grab %s. %s.\n", kbd_device, strerror(errno));
	} else {
		if(debug) printf("Grabbed %s!\n", kbd_device);
	}

	ufd = open("/dev/uinput",O_WRONLY);
	if(ufd == -1){
		printf("Cannot open uinput. %s.\n", strerror(errno));
		return -1;
	}  

	memset(&usetup, 0, sizeof(usetup));
        if(ioctl(ufd, UI_SET_EVBIT, EV_KEY) < 0)
 		fprintf(stderr, "error setting event type\n");
	for(int i=1; i<195; i++){
		if (ioctl(ufd, UI_SET_KEYBIT,i ) < 0) // TODO only those from usb_hid_keys.h translated by hid_keyboard[]
		fprintf(stderr, "error setting keys\n");
	}
	memset(&id, 0, sizeof(id));
	usetup.id = id;
	usetup.id.bustype = BUS_USB;
	usetup.id.vendor = 0x1209;
	usetup.id.product = 0x4445;
	usetup.id.version = 1;
	strcpy(usetup.name, "IRMP_STM32_KBD");
	if(ioctl(ufd, UI_DEV_SETUP, &usetup) < 0)
		fprintf(stderr, "error setup\n");
	if(ioctl(ufd, UI_DEV_CREATE) < 0)
		fprintf(stderr, "error create\n");
        if(write_event(ufd, EV_REP, REP_DELAY, 0) < 0)
 		fprintf(stderr, "error turning autorepeat off\n");

	sleep(1);

	while(1){
		usleep(1000); // don't eat too much cpu
		if (read(fd, &event, sizeof(event)) != -1) {
		if (event.type == EV_KEY && event.value == 1) {
			clock_gettime(CLOCK_MONOTONIC, &now);
			this_time = now.tv_sec * 1000 + now.tv_nsec / 1000 / 1000;
			pair = (this_time - last_received < 10)? 1 : 0; // modifier+key
			if(debug) printf("r %ld %d %d --- ", this_time, event.code, pair);
			if(!pair){
				if(this_time - last_received > timeout) { // new key
					last_received = this_time;
					value = 1;
					skip = 0;
				} else { // repeat
					last_received = this_time;
					if( ((value == 1) && (this_time - last_sent < delay)) || ((value == 2) && (this_time - last_sent) < period)) {
						skip = 1;
						continue; // don't send key
					} else {
						value = 2;
						skip = 0;
					}
				}
			}
      
			/* send key */
			if (!skip){
				if(!pair) {
					code = event.code;
				} else {
					code2 = event.code;
				}
				write_event(ufd, EV_KEY, event.code, value);
				write_event(ufd, EV_SYN, SYN_REPORT, 0);
				last_sent = this_time;
				if(debug) printf("w %d %3d\n", event.code, value);
				release_needed = 1;
				}
			}
		}
 
		/* send release */
		clock_gettime(CLOCK_MONOTONIC, &now);
		this_time = now.tv_sec * 1000 + now.tv_nsec / 1000 / 1000;
		if((this_time - last_received > timeout) && release_needed) {
			release_needed = 0;
			write_event(ufd, EV_KEY, code, 0);
			write_event(ufd, EV_SYN, SYN_REPORT, 0);
			if(debug) printf("                    w %ld %d %3d\n", this_time, code, 0);
			if (pair) {
				write_event(ufd, EV_KEY, code2, 0);
				write_event(ufd, EV_SYN, SYN_REPORT, 0);
				if(debug) printf("                    w %ld %d %3d\n", this_time, code2, 0);
			}
		}
	}
	//TODO on exit
	//ioctl(fd, EVIOCGRAB, 0);
	//ioctl(ufd, UI_DEV_DESTROY);
	//close(fd);
	//close(ufd);
	//return 0;
}
