#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#define DRIVER_NAME "/dev/chilka"
#define POP_DATA _IOR('a','c',struct data*)

struct data
{
	int size;
	char *val;
};

int main(void)
{
	int fd = open(DRIVER_NAME,O_RDWR);
	struct data *d = malloc(sizeof(struct data));
	d->size = 3;
	d->val = malloc(3);
	int ret = ioctl(fd,POP_DATA,d);
	close(fd);
	free(d->val);
	free(d);
	return ret;
}
