#include <stdio.h>
#include <linux/kernel.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(){
	char name[32];
	char buf[100]="Hello World!!!\n";
	strcpy(name,"a.txt");
	int fd=open(name, O_WRONLY | O_CREAT);
	int count=write(fd, buf, strlen(buf));

	printf("Wrote to file\n");
	close(fd);

	return 0;
}
