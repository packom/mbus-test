//
// (C) Piers Finlayson 2019
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <https://www.gnu.org/licenses/>.
//

#include <fcntl.h>
#include <stdio.h>
#include <termios.h>
#include <errno.h>
#include <string.h>

#define DEVICE "/dev/serial0"

#define ARG_VERSION1  "--version"
#define ARG_VERSION2  "-v"
#define ARG_HELP1     "--help"
#define ARG_HELP2     "-h"
#define ARG_HELP3     "-?"

#define APP_VER       "0.1.0"

#define VERSION(DESC) "%s version %s\n", DESC, APP_VER

#define EXIT_IF_FAILED(X, ...)     \
if (X < 0)                         \
{                                  \
  fprintf(stderr, ##__VA_ARGS__);  \
  return -1;                       \
}

#define EXIT_IF_NULL(X, ...)       \
if (X == NULL)                     \
{                                  \
  fprintf(stderr, ##__VA_ARGS__);  \
  return -1;                       \
}

struct baud_rate
{
  speed_t baud;
  char *bauds;
};

#define NUM_BAUD_RATES 16
#ifdef MBUS_TEST_C
struct baud_rate rates[NUM_BAUD_RATES] =
{
  {B0, "0"},
  {B50, "50"},
  {B75, "75"},
  {B110, "110"},
  {B134, "134"},
  {B150, "150"},
  {B200, "200"},
  {B300, "300"},
  {B600, "600"},
  {B1200, "1200"},
  {B1800, "1800"},
  {B2400, "2400"},
  {B4800, "4800"},
  {B9600, "9600"},
  {B19200, "19200"},
  {B38400, "38400"},
};
#else // MBUS_TEST_C
extern struct baud_rate rates[NUM_BAUD_RATES];
#endif // MBUS_TEST_C

char *get_baud_rate_as_string(speed_t speed);
int get_termios(int fd, struct termios *t);
void change_termios(struct termios *t);
int set_termios(int fd, struct termios *t);
void log_termios(struct termios *t);
int set_up_device(char *dev, FILE **file, speed_t speed);
int open_device(char *dev, FILE **file);
int set_ospeed(struct termios *t, speed_t speed);
int set_ispeed(struct termios *t, speed_t speed);
speed_t get_ospeed(struct termios *t);
speed_t get_ispeed(struct termios *t);
char *handle_args(int argc, char *argv[], char *app_desc);
