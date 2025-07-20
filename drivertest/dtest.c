#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <string.h>

#define DEVICE_NAME "/dev/sandpiper"
#define MY_IOCTL_GET_VIRT_ADDR _IOR('k', 0, void*)
#define MEM_SIZE 0x2000000  // 32MB - should match the driver

int main() {
	int fd;
	void *mapped_mem;
	unsigned int *ptr;

	fd = open(DEVICE_NAME, O_RDWR);  // Open for read/write
	if (fd < 0) {
		perror("Failed to open device");
		fprintf(stderr, "Error: %s\n", strerror(errno));
		return 1;
	}

	// Map the physical memory to user space
	mapped_mem = mmap(NULL, MEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (mapped_mem == MAP_FAILED) {
		perror("Failed to mmap device memory");
		close(fd);
		return 1;
	}

	printf("Memory mapped successfully at user virtual address: %p\n", mapped_mem);

	// Now you can safely access the memory as a regular pointer
	ptr = (unsigned int*)mapped_mem;
	
	// Read the first 32-bit value
	printf("Value at offset 0: 0x%x\n", ptr[0]);
	
	// Write a test value
	ptr[0] = 0x12345678;
	printf("Wrote 0x12345678 to offset 0\n");
	
	// Read it back
	printf("Read back: 0x%x\n", ptr[0]);
	
	// Access different offsets (be careful not to exceed MEM_SIZE)
	ptr[1] = 0xDEADBEEF;
	printf("Wrote 0xDEADBEEF to offset 4, read back: 0x%x\n", ptr[1]);

	// Clean up
	munmap(mapped_mem, MEM_SIZE);
	close(fd);

	return 0;
}

