#include <tinyara/gpio.h>
#include <tinyara/analog/adc.h>
#include <tinyara/analog/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include "artik_function.h"

int analogInit(void){
	sample = (struct adc_msg_s *)malloc(sizeof(struct adc_msg_s)*S5J_ADC_MAX_CHANNELS);
	adc_fd = open("/dev/adc0", O_RDONLY);
	if (adc_fd < 0) {
		return FALSE;
	}
	return TRUE;
}

void analogFinish(void){
	close(adc_fd);
	free(sample);
}

int analogRead(int port){
	int result = -1;
	if(port >= 0 && port <= MAX_PIN_NUM){				//Check available ADC port
		int  ret;
		size_t readsize;
		ssize_t nbytes;

		while(1){
			ret = ioctl(adc_fd, ANIOC_TRIGGER, 0);
			if (ret < 0) {
				analogFinish();
				analogInit();
				return result;
			}

			readsize = S5J_ADC_MAX_CHANNELS * sizeof(struct adc_msg_s);
			nbytes = read(adc_fd, sample, readsize);
			if (nbytes < 0) {
				if (errno != EINTR) {
					analogFinish();
					analogInit();
					return result;
				}
			} else if (nbytes == 0) {

			} else {
				int nsamples = nbytes / sizeof(struct adc_msg_s);
				if (nsamples * sizeof(struct adc_msg_s) == nbytes) {
					int i;
					for (i = 0; i < nsamples; i++) {
						if(sample[i].am_channel ==port){
							result =  sample[i].am_data;
							goto out;
						}
					}
				}
			}
		}
		out:
		up_mdelay(5);
	}
	return result;
}

void digitalWrite(int port, int value){
	char str[4];
	static char devpath[16];
	snprintf(devpath, 16, "/dev/gpio%d", port);
	int fd = open(devpath, O_RDWR);

	ioctl(fd, GPIOIOC_SET_DIRECTION, GPIO_DIRECTION_OUT);
	write(fd, str, snprintf(str, 4, "%d", value != 0) + 1);

	close(fd);
}

int digitalRead(int port){
	char buf[4];
	char devpath[16];
	snprintf(devpath, 16, "/dev/gpio%d", port);
	int fd = open(devpath, O_RDWR);

	ioctl(fd, GPIOIOC_SET_DIRECTION, GPIO_DIRECTION_IN);
	read(fd, buf, sizeof(buf));
	close(fd);
	return buf[0] == '1';
}
