#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define DRIVER_NAME "/dev/chilka"
#define PUSH_DATA _IOW('a','b',struct data*)

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
	memcpy(d->val,"xyz",3);
	int ret = ioctl(fd,PUSH_DATA,d);
	close(fd);
	free(d->val);
	free(d);
	return ret;
}

