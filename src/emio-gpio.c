#include <gpiod.h>   // libgpiod
#include <stdio.h>
#include <unistd.h> // might not need this lib
#include <time.h>   // might not need this lib, nanosleep()

/* 
Remember to reload FPGA image before running emio-gpio.c:
    cd fpga/out/bitstream
    fpga-load
    cd ../../..
    cd src
    gcc emio-gpio.c -lgpiod
    ./a.out
*/

int main (void) {
    // Chip access
    const char *chip_name = "gpiochip1";
    struct gpiod_chip *chip = gpiod_chip_open_by_name(chip_name); // Note: If permission is denied, bypass (for now): 'sudo chmod 666 /dev/gpiochip*'
    if (!chip) { 
        perror("open chip"); 
        return 1; 
    }
    printf("Successfully opened chip %s.\n", chip_name);

    // Line access, switch 78 (EMIO bit 0 = PMOD2 IO1)
    unsigned int line_num = 78;
    struct gpiod_line *line = gpiod_chip_get_line(chip, line_num);
    if (!line) { 
        perror("access line"); 
        gpiod_chip_close(chip); // Close chip on error
        return 1; 
    }
    printf("Successfully accessed line %d.\n", line_num);

    // Reserve Line 78 as output, start at 0 (off)
    if (gpiod_line_request_output(line, "emio-leds", 0) < 0) { // Usage: (struct gpiod_line *line, const char *consumer, int default_val)
        perror("request line");
        gpiod_chip_close(chip); // Close chip on error
        return 1;
    }
    printf("Successfully reserved line %d as output.\n", line_num);

    // Flip Line 78 for 10 seconds
    for (int i = 0; i < 10; i++) {
        gpiod_line_set_value(line, 1); 
        sleep(1);
        gpiod_line_set_value(line, 0);
        sleep(1);
    }

    // Return and close
    gpiod_line_release(line);
    gpiod_chip_close(chip);

    return 0;
}