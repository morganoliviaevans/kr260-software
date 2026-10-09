#include <gpiod.h>   // libgpiod
#include <stdio.h>
#include <unistd.h> // might not need this lib
#include <time.h>   // might not need this lib, nanosleep()

/* 
Remember to reload FPGA image before running emio-gpio.c (can potentially automate this later):
    cd fpga/out/bitstream
    fpga-load
    cd ../../..
    cd src
    gcc emio-gpio.c -o ../build/emio-gpio.out -lgpiod
    ../build/emio-gpio.out
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

    // Line access, switches 78-85 (EMIO bit 0 = PMOD2 IO1-I08)
    struct gpiod_line *lines[8];
    for (int i = 0; i < 8; i++) {
        lines[i] = gpiod_chip_get_line(chip, 78 + i);
        if (!lines[i]) {
            perror("access lines"); 
            gpiod_chip_close(chip); // Close chip on error
            break;                  // Stop trying to fetch lines if the first one fails
        }
    }

    printf("Successfully accessed lines: [");
    for (int i = 0; i < 8; i++) {
        printf("%d", gpiod_line_offset(lines[i])); // gpiod_line_offset() will pull our lines for us
        if (i < 7) { printf(", "); }               // Print a comma and space for all items except the last one
    }
    printf("].\n");

    for (int i = 0; i < 8; i++) {
        if (gpiod_line_request_output(lines[i], "emio-leds", 0) < 0) {
            perror("request line");
            gpiod_chip_close(chip); // Close chip on error
            return 1;
        }
    }
    printf("Successfully reserved lines [78, 79, 80, 81, 82, 83, 84, 85] as output.\n");

    // Dance #1
    for (int i = 0; i < 4; i++) {
        gpiod_line_set_value(lines[i], 1);
        sleep(1);
        gpiod_line_set_value(lines[i], 0);
        sleep(0);
    }
    for (int i = 7; i >= 4; i--) {
        gpiod_line_set_value(lines[i], 1);
        sleep(1);
        gpiod_line_set_value(lines[i], 0);
        sleep(0);
    }

    // Dance #2
    for (int i = 0; i < 4; i++) {
        gpiod_line_set_value(lines[i], 1);
        gpiod_line_set_value(lines[i + 4], 1);
        sleep(1);
        gpiod_line_set_value(lines[i], 0);
        gpiod_line_set_value(lines[i + 4], 0);
        sleep(0);
    }

    // Dance #3
    for (int i = 0; i < 4; i++) {
        gpiod_line_set_value(lines[i], 1);
        gpiod_line_set_value(lines[7 - i], 1);
        sleep(1);
        gpiod_line_set_value(lines[i], 0);
        gpiod_line_set_value(lines[7 - i], 0);
        sleep(0);
    }

    // Return and close
    for (int i = 0; i < 8; i++) {
        if (lines[i]) { gpiod_line_release(lines[i]); }
    }
    gpiod_chip_close(chip);

    return 0;
}