#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "asm-arm/arch-pxa/lib/creator_pxa270_lcd.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int keyboard(char c) {
    // char to int mapping
    switch (c) {
        case '0': return 0; break;
        case '1': return 1; break;
        case '2': return 2; break;
        case '3': return 3; break;
        case '4': return 4; break;
        case '5': return 5; break;
        case '6': return 6; break;
        case '7': return 7; break;
        case '8': return 8; break;
        case '9': return 9; break;
        case 'A': return 10; break;
        case 'B': return 11; break;
        case 'C': return 12; break;
        case 'D': return 13; break;
        case '*': return 14; break;
    }
}

int get_7seg_pattern(char c) {
    // char to value mapping
    switch (c) {
        case '0': return 0x3f; break;
        case '1': return 0x06; break;
        case '2': return 0x5b; break;
        case '3': return 0x4f; break;
        case '4': return 0x66; break;
        case '5': return 0x6d; break;
        case '6': return 0x7d; break;
        case '7': return 0x07; break;
        case '8': return 0x7f; break;
        case '9': return 0x6f; break;
        case 'A': return 0x77; break; // A
        case 'B': return 0x7c; break; // b
        case 'C': return 0x58; break; // c
        case 'D': return 0x5e; break; // d
        case '*': return 0x80; break; // .
    }
}

int main() {
    int fd, ret, i, j, num;
    char c, line[1024] = {0}, diamond[1024] = {0}, triangle[1024] = {0};
    unsigned short key, data;
    lcd_write_info_t display;
    bool isFirst = true;
    _7seg_info_t seg;

    if ((fd = open("/dev/lcd", O_RDWR)) == -1) {
        perror("Open /dev/lcd failed.");
        exit(EXIT_FAILURE);
    }

    // initialize
    ioctl(fd, KEY_IOCTL_CLEAR, key);
    ioctl(fd, LCD_IOCTL_CLEAR);
    data = LED_ALL_OFF;
    ioctl(fd, LED_IOCTL_SET, &data);
    ioctl(fd, _7SEG_IOCTL_ON);

    // draw triangle
    for (i = 1; i < 17; i += 2) {
        for (j = 0; j < 7 - (i - 1) / 2; ++j) {
            strcat(triangle, " ");
            strcat(diamond, " ");
        }
        for (j = 0; j < i; ++j) {
            strcat(triangle, "*");
            strcat(diamond, "*");
        }
        strcat(triangle, "\n");
        strcat(diamond, "\n");
    }

    // draw diamond
    for (i = 13; i >= 1; i -= 2) {
        for (j = 0; j < 7 - (i - 1) / 2; ++j) {
            strcat(diamond, " ");
        }
        for (j = 0; j < i; ++j) {
            strcat(diamond, "*");
        }
        strcat(diamond, "\n");
    }

    // 0 second
    data = LED_ALL_OFF;
    ioctl(fd, LED_IOCTL_SET, &data);

    seg.Mode = _7SEG_MODE_PATTERN;
    seg.Which = _7SEG_D5_INDEX | _7SEG_D6_INDEX | _7SEG_D7_INDEX | _7SEG_D8_INDEX;
    seg.Value = 0x00000000;
    ioctl(fd, _7SEG_IOCTL_SET, &seg);

    ioctl(fd, LCD_IOCTL_CLEAR);

    while (true) {
        if ((ret = ioctl(fd, KEY_IOCTL_CHECK_EMTPY, &key)) == -1)  continue;

        // get key
        ioctl(fd, KEY_IOCTL_GET_CHAR, &key);
        c = (key & 0xFF);

        // write char to lcd
        display.Count = sprintf((char *)display.Msg, &c);
        ioctl(fd, LCD_IOCTL_WRITE, &display);

        // write char to putty
        printf("%c", c);
        fflush(stdout);

        if (c == '#') {
            printf("\n");
            break;
        }
        else {
            // store char
            strncat(line, &c, 1);
        }
    }

    // printf("%s\n", line);
    sleep(3);

    for (i = 0, j = 0; ; ++i, ++j) {
        // clear led
        data = LED_ALL_OFF;
        ioctl(fd, LED_IOCTL_SET, &data);

        // clear 7seg
        seg.Mode = _7SEG_MODE_PATTERN;
        seg.Which = _7SEG_D5_INDEX | _7SEG_D6_INDEX | _7SEG_D7_INDEX | _7SEG_D8_INDEX;
        seg.Value = 0x00000000;
        ioctl(fd, _7SEG_IOCTL_SET, &seg);

        // clear lcd
        ioctl(fd, LCD_IOCTL_CLEAR);

        i %= strlen(line);
        // printf("%d seconds\n", j);

        // D9 - D12
        num = keyboard(line[i]);
        // printf("%d\n", num);

        if (num >= 8) {
            num -= 8;
            data = LED_D12_INDEX;
            ioctl(fd, LED_IOCTL_BIT_SET, &data);
        }
        if (num >= 4) {
            num -= 4;
            data = LED_D11_INDEX;
            ioctl(fd, LED_IOCTL_BIT_SET, &data);
        }
        if (num >= 2) {
            num -= 2;
            data = LED_D10_INDEX;
            ioctl(fd, LED_IOCTL_BIT_SET, &data);
        }
        if (num >= 1) {
            num -= 1;
            data = LED_D9_INDEX;
            ioctl(fd, LED_IOCTL_BIT_SET, &data);
        }

        // D13 - D16
        if (i - 1 == -1) {
            if (isFirst) {
                num = 15;
                isFirst = false;
            }
            else {
                num = keyboard(line[strlen(line) - 1]);
            }
        }
        else { num = keyboard(line[i - 1]); }
        // printf("%d\n", num);

        if (num >= 8) {
            num -= 8;
            data = LED_D16_INDEX;
            ioctl(fd, LED_IOCTL_BIT_SET, &data);
        }
        if (num >= 4) {
            num -= 4;
            data = LED_D15_INDEX;
            ioctl(fd, LED_IOCTL_BIT_SET, &data);
        }
        if (num >= 2) {
            num -= 2;
            data = LED_D14_INDEX;
            ioctl(fd, LED_IOCTL_BIT_SET, &data);
        }
        if (num >= 1) {
            num -= 1;
            data = LED_D13_INDEX;
            ioctl(fd, LED_IOCTL_BIT_SET, &data);
        }

        // 7seg
        if (j >= 0) {
            seg.Mode = _7SEG_MODE_PATTERN;
            seg.Which = _7SEG_D5_INDEX;
            seg.Value = get_7seg_pattern(line[j % strlen(line)]);
            ioctl(fd, _7SEG_IOCTL_SET, &seg);
        }
        if (j - 1 >= 0) {
            seg.Mode = _7SEG_MODE_PATTERN;
            seg.Which = _7SEG_D6_INDEX;
            seg.Value = get_7seg_pattern(line[(j - 1) % strlen(line)]);
            ioctl(fd, _7SEG_IOCTL_SET, &seg);
        }
        if (j - 2 >= 0) {
            seg.Mode = _7SEG_MODE_PATTERN;
            seg.Which = _7SEG_D7_INDEX;
            seg.Value = get_7seg_pattern(line[(j - 2) % strlen(line)]);
            ioctl(fd, _7SEG_IOCTL_SET, &seg);
        }
        if (j - 3 >= 0) {
            seg.Mode = _7SEG_MODE_PATTERN;
            seg.Which = _7SEG_D8_INDEX;
            seg.Value = get_7seg_pattern(line[(j - 3) % strlen(line)]);
            ioctl(fd, _7SEG_IOCTL_SET, &seg);
        }

        // write shape to lcd
        if (j % 2 == 0) {
            display.Count = sprintf((char *)display.Msg, diamond);
        } else {
            display.Count = sprintf((char *)display.Msg, triangle);
        }
        ioctl(fd, LCD_IOCTL_WRITE, &display);

        sleep(3);
    }

    close(fd);
    return 0;
}