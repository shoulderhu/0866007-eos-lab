#include <iostream>
#include <sstream>     // stringstream
#include <vector>      // vector
#include <cstdio>      // fprintf
#include <cstdlib>     // exit
#include <cstring>
#include <cerrno>      // errno
#include <cctype>      // isdigit
#include <cmath>       // pow

#include <sys/ioctl.h> // ioctl
#include <fcntl.h>     // open
#include "asm-arm/arch-pxa/lib/creator_pxa270_lcd.h"

using namespace std;

int lcdfd;
unsigned short key, data;
lcd_write_info_t display;
_7seg_info_t seg;

void show_lcd(const string &line) {
    // write char to lcd
    display.Count = sprintf((char *)display.Msg, line.c_str());
    ioctl(lcdfd, LCD_IOCTL_CLEAR, NULL);
    ioctl(lcdfd, LCD_IOCTL_WRITE, &display);
}

void set_led(int i) {
    switch(i) {
        case 0:
            data = LED_D9_INDEX;
            break;
        case 1:
            data = LED_D10_INDEX;
            break;
        case 2:
            data = LED_D11_INDEX;
            break;
        case 3:
            data = LED_D12_INDEX;
            break;
        case 4:
            data = LED_D13_INDEX;
            break;
        case 5:
            data = LED_D14_INDEX;
            break;
        case 6:
            data = LED_D15_INDEX;
            break;
        case 7:
            data = LED_D16_INDEX;
            break;
    }
}

void show_led(int result) {
    if (result == 0) {
        data = LED_ALL_OFF;
        ioctl(lcdfd, LED_IOCTL_SET, &data);
    }

    int array[] = {128, 64, 32, 16, 8, 4, 2, 1};

    for (int i = 0; i < 8; ++i) {
        if (result >= array[i]) {
            result -= array[i];
            set_led(i);
            ioctl(lcdfd, LED_IOCTL_BIT_SET, &data);
        }
        if (result == 0) return;
    }
}

void show_7seg(int result) {
    int value = 0;
    for (int i = 0; i < 4; ++i) {
        value += (result % 10) * (int)pow(16.0, (double)i);
        result /= 10;
    }

    seg.Mode = _7SEG_MODE_HEX_VALUE;
    seg.Which = _7SEG_ALL;
    seg.Value = value;
    ioctl(lcdfd, _7SEG_IOCTL_SET, &seg);
}

int calculator(string str) {
    vector<string> vec;
    int result;

    for (unsigned int i = 0; i < str.length(); ++i) {
        if (isdigit(str[i])) {
            int j = 1;
            while (isdigit(str[i + j])) ++j;
            string num = str.substr(i, j);
            stringstream ss;

            if (!vec.empty()) {
                if (vec.back() == "*") {
                    vec.pop_back();
                    ss << atoi(vec.back().c_str()) * atoi(num.c_str());
                    vec.pop_back(); vec.push_back(ss.str());
                }
                else if (vec.back() == "/") {
                    vec.pop_back();
                    ss << atoi(vec.back().c_str()) / atoi(num.c_str());
                    vec.pop_back(); vec.push_back(ss.str());
                }
                else { // '+', '-'
                    vec.push_back(num);
                }
            }
            else { // empty
                vec.push_back(num);
            }

            i += j - 1;
        }
        else if (str[i] == '=') {
            result = atoi(vec[0].c_str());
            for (unsigned int j = 1; j < vec.size(); j += 2) {
                if (vec[j] == "+") {
                    result += atoi(vec[j + 1].c_str());
                }
                else {
                    result -= atoi(vec[j + 1].c_str());
                }
            }
        }
        else { // '+', '-', '*', '/'
            vec.push_back(string(1, str[i]));
        }
    }

    return result;
}

void clear_lcd() {
    // KEYPAD && LCD
    ioctl(lcdfd, KEY_IOCTL_CLEAR, key);
    ioctl(lcdfd, LCD_IOCTL_CLEAR);

    // 7SEG
    ioctl(lcdfd, _7SEG_IOCTL_ON);
    show_7seg(0);

    // LED
    data = LED_ALL_OFF;
    ioctl(lcdfd, LED_IOCTL_SET, &data);
}

void open_lcd() {
    if ((lcdfd = open("/dev/lcd", O_RDWR)) == -1) {
        fprintf(stderr, "open /dev/lcd: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    clear_lcd();
}

int main() {
    open_lcd();

    string line;
    char c;
    bool flag_clear = false;

    while (true) {
        if (ioctl(lcdfd, KEY_IOCTL_CHECK_EMTPY, &key) == -1) continue;

        // get key
        ioctl(lcdfd, KEY_IOCTL_GET_CHAR, &key);
        c = (key & 0xFF);

        // preprocessing
        switch (c) {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                if (flag_clear) {
                    flag_clear = false;
                    line.clear();
                    show_7seg(0);
                    show_led(0);
                }
                line += c;
                printf("%c", c);
                fflush(stdout);
                break;
            case 'A': // '+'
                line += '+';
                printf("%c", '+');
                fflush(stdout);
                break;
            case 'B': // '-'
                line += '-';
                printf("%c", '-');
                fflush(stdout);
                break;
            case 'C': // '*'
                line += '*';
                printf("%c", '*');
                fflush(stdout);
                break;
            case 'D': // '/'
                line += '/';
                printf("%c", '/');
                fflush(stdout);
                break;
            case '*': // clear
                line.clear();
                show_7seg(0);
                show_led(0);
                printf("\n");
                break;
            case '#': // '='
                line += '=';
                int result = calculator(line);
                stringstream ss;
                ss << result;
                line += ss.str() + '\n';
                printf("=%d\n", result);
                show_7seg(result);
                show_led(result);
                flag_clear = true;
                break;
        }

        show_lcd(line);
    }





    // printf("%d\n", calculator("1+2*3="));

    return 0;
}
