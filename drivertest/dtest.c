#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <string.h>

#define DEVICE_NAME "/dev/sandpiper"
#define MY_IOCTL_GET_VIRT_ADDR _IOR('k', 0, void*)

int main() {
    int fd;
    void *virt_addr;
    unsigned int *ptr; // Pointer to access the memory

    fd = open(DEVICE_NAME, O_RDONLY);
    if (fd < 0) {
        perror("Failed to open device");
        fprintf(stderr, "Error: %s\n", strerror(errno));
        return 1;
    }

    if (ioctl(fd, MY_IOCTL_GET_VIRT_ADDR, &virt_addr) < 0) {
        perror("Failed to execute ioctl");
        close(fd);
        return 1;
    }

    printf("Virtual address from driver: %p\n", virt_addr);

    // Example: Access the first 4 bytes of the mapped memory
    ptr = (unsigned int*)virt_addr;
    printf("Value at virtual address: 0x%x\n", *ptr);

    close(fd);

    return 0;
}

