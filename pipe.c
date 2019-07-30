/*
 * Author: Evan Shipman
 * Title : Xorshift PRNG pipe
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define uint8_t  unsigned char
#define uint64_t unsigned long long

// Fast PRNG
uint64_t xorshift(uint64_t *seed) {
    uint64_t x = *seed;
    x ^= x << 13;
    x ^= x >> 7;
    x ^= x << 17;
    *seed= x;
}

int main(int argc, uint8_t **argv) {
    // Hardcoded pipe location for now
    uint8_t *pipe = "./frandom";
    int fd,
        pipe_size = 524288;
    uint64_t seed;

    // Read a random seed from /dev/urandom, then close the file
    fd = open("/dev/urandom", O_RDONLY);
    read(fd, &seed, sizeof(uint64_t));
    close(fd);

    // Make a named pipe and open it
    mkfifo(pipe, 0644);
    fd = open(pipe, O_RDWR);

    // Set the size of the pipe, then verify that 
    fcntl(fd, F_SETPIPE_SZ, pipe_size);

    // Constantly write random numbers to the pipe
    while (1) {
        xorshift(&seed);
        write(fd, &seed, sizeof(uint64_t));
    }

    // Unreachable
    close(fd);

    return 0;
}
