#define _GNU_SOURCE
 
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h> 

#define GPIO_PINS_PATH "/sys/class/gpio/"
#define GPIO_BASE_OFFSET 458

/* cm/us */
#define ULTRASONIC_SPEED_US 0.0343

#define GPIO_WHITE_TRIG 22
#define GPIO_WHITE_ECHO 13

#define WHITE_TRIG_PIN (GPIO_BASE_OFFSET + GPIO_WHITE_TRIG)
#define WHITE_ECHO_PIN (GPIO_BASE_OFFSET + GPIO_WHITE_ECHO)

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

const char* trig_paths[1] = { "/sys/class/gpio/gpio480/value" };
const char* echo_paths[1] = { "/sys/class/gpio/gpio471/value" };

#if 0
const char* trig_paths[1] = { "/sys/class/gpio/gpio" STR(WHITE_TRIG_PIN) "/value" };
const char* echo_paths[1] = { "/sys/class/gpio/gpio" STR(WHITE_ECHO_PIN) "/value" };
#endif

#define WHITE_SENSOR_INDEX 0

static inline long cycles_us(void)
{
    struct timeval t;

    gettimeofday(&t, NULL);

    return t.tv_sec * 1000000 + t.tv_usec;
}

int trig(char *onoff, int uduration, int sensor_index)
{
    int b;
    int fd;
    int err = 0;

    fd = open(trig_paths[sensor_index], O_WRONLY);
    if (fd < 0){
        perror("Failed to open file");

        err = fd;
        goto out;
    }

    b = write(fd, onoff, 3);
    if (b < 0){
        perror("write:");

        err = b;
        goto close_fd;
    }

    usleep(uduration);

    b = write(fd, "0\n", 3);
    if (b < 0){
        perror("write:");

        err = b;
        goto close_fd;
    }

close_fd:
    close(fd);
out:
    return err;
}

long wait_echo(char c, int sensor_index)
{
    long err = -1;
    int fd = open(echo_paths[sensor_index], O_RDONLY);
    if (fd < 0){
        perror("Failed to open file");
        goto out_err;
    }

    char buf[32] = {0};
    buf[0] = c;
    while (buf[0] == c){
        int b = read(fd, buf, sizeof(buf));
        if (!b){
            close(fd);
            fd = open(echo_paths[sensor_index], O_RDONLY);
            if (fd < 0){
                perror("Failed to open file inside loop");
                goto out_err;
            }

            continue;
        }

        if (b < 0){
            perror("read:");
            goto close_fd;
        }
    }

    return cycles_us();

close_fd:
    close(fd);
out_err:
    return err;
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("%s <wait time us>\n",argv[0]);
        return -1;
    }

    /* trigger duration in micro seconds */
    int sleep_us = atoi(argv[1]);

    /* Trigger a pulse sleep_us long */
    trig("1\n", sleep_us, WHITE_SENSOR_INDEX);

    /* wait untill there is no zeroes and get the time */
    long start = wait_echo('0', WHITE_SENSOR_INDEX);
    if (start < 0){
        printf("Error in waiting for echo 0\n");
        return -1;
    }

    /* wait untill there is no ones and get the time */
    long end  = wait_echo('1', WHITE_SENSOR_INDEX);
    if (end < 0){
        printf("Error in waiting for echo 1\n");
        return -1;
    }

    long dt_us = (end - start);

    float distance = ((float)dt_us * ULTRASONIC_SPEED_US) / 2.0;

    printf("Distance %fcm dt=%ld\n", distance, dt_us);

    return 0;
}
