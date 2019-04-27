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

#define MBUS_TEST_C
#include "mbus-test.h"

char *get_baud_rate_as_string(speed_t speed)
{
  int ii;
  char *speeds = "unknown";

  for (ii = 0; ii < NUM_BAUD_RATES; ii++)
  {
    if (rates[ii].baud == speed)
    {
      speeds = rates[ii].bauds;
      break;
    }
  }

  return speeds;
}

int get_termios(int fd, struct termios *t)
{
  int rc;
  rc = tcgetattr(fd, t);
  return rc;
}

void change_termios(struct termios *t)
{
  t->c_cflag &= ~(CSTOPB | CSIZE | PARODD | HUPCL);
  t->c_cflag |= CS8 | CREAD | CLOCAL | PARENB;
  t->c_iflag = 0;
  t->c_oflag = 0;
  t->c_lflag = 0;
  t->c_cc[VMIN] = (cc_t)0;
  t->c_cc[VTIME] = (cc_t)2;
}

int set_termios(int fd, struct termios *t)
{
  int rc;
  rc = tcsetattr(fd, TCSANOW, t);
  return rc;
}

void log_termios(struct termios *t)
{
  int ii;

  fprintf(stdout, "  c_iflag 0x%08x\n", t->c_iflag);
  fprintf(stdout, "  c_oflag 0x%08x\n", t->c_oflag);
  fprintf(stdout, "  c_cflag 0x%08x\n", t->c_cflag);
  fprintf(stdout, "  c_lflag 0x%08x\n", t->c_lflag);
  for (ii = 0; ii < NCCS; ii++)
  {
    fprintf(stdout, "c_cc[%d] 0x%08x\n", ii, t->c_cc[ii]);
  }

  return;
}

int set_up_device(char *dev, FILE **file, speed_t speed)
{
  struct termios t;
  int fd, rc; 

  fprintf(stdout, "Using device %s\n", dev);

  fd = open_device(dev, file);
  EXIT_IF_FAILED(fd, "Failed to open device %s %d\n", dev, errno);
  EXIT_IF_NULL(*file, "Failed to open device %s %d\n", dev, errno);

  rc = get_termios(fd, &t);
  EXIT_IF_FAILED(fd, "Failed to open device %s %d\n", dev, errno);
  log_termios(&t);

  rc = set_ispeed(&t, speed);
  EXIT_IF_FAILED(rc, "Failed to set ispeed %d\n", errno);

  rc = set_ospeed(&t, speed);
  EXIT_IF_FAILED(rc, "Failed to set ospeed %d\n", errno);

  change_termios(&t);
  rc = set_termios(fd, &t);
  EXIT_IF_FAILED(rc, "Failed to set termios %d\n", errno);

  rc = get_termios(fd, &t);
  EXIT_IF_FAILED(fd, "Failed to open device %s %d\n", dev, errno);
  log_termios(&t);

  speed = get_ispeed(&t);
  fprintf(stdout, "  ispeed set to %s baud\n", get_baud_rate_as_string(speed));

  speed = get_ospeed(&t);
  fprintf(stdout, "  ospeed set to %s baud\n", get_baud_rate_as_string(speed));

  return fd;
}

int open_device(char *dev, FILE **file)
{
  int fd;

  fd = open(dev, O_RDWR | O_NOCTTY);
  if (fd >= 0)
  {
    *file = fdopen(fd, "r+");
  }
  else
  {
    *file = NULL;
  }
  
  return fd;
}

int set_ospeed(struct termios *t, speed_t speed)
{
  int rc;
  rc = cfsetospeed(t, speed);
  return rc;
}

int set_ispeed(struct termios *t, speed_t speed)
{
  int rc;
  rc = cfsetispeed(t, speed);
  return rc;
}

speed_t get_ospeed(struct termios *t)
{
  speed_t rc;
  rc = cfgetospeed(t);
  return rc;
}

speed_t get_ispeed(struct termios *t)
{
  speed_t rc;
  rc = cfgetispeed(t);
  return rc;
}

// Returns:
// - 1 if all OK
// - 0 if printed version or help (and should now exit)
// - -1 on error
int handle_args(int argc, char *argv[], char *app_desc, char **device, speed_t *speed)
{
  int rc = -1, ii, got_baud;

  // Set defaults first
  *speed = BAUDRATE;
  *device = DEVICE;

  // Process args
  if (argc > 1)
  {
    // If getting version of help return -1 so app exits
    if (!strncmp(argv[1], ARG_VERSION1, strlen(ARG_VERSION1)) ||
        !strncmp(argv[1], ARG_VERSION2, strlen(ARG_VERSION2)))
    {
      fprintf(stdout, VERSION(app_desc));
      rc = 0;
      goto EXIT_LABEL;
    }
    else if (!strncmp(argv[1], ARG_HELP1, strlen(ARG_HELP1)) ||
             !strncmp(argv[1], ARG_HELP2, strlen(ARG_HELP2)) ||
             !strncmp(argv[1], ARG_HELP3, strlen(ARG_HELP3)))
    {
      fprintf(stdout, VERSION(app_desc));
      fprintf(stdout, "Usage: %s [-b BAUDRATE] [device]\n", argv[0]);
      fprintf(stdout, "       [-b BAUDRATE] - baudrate, defaults to\n", BAUDRATE);
      fprintf(stdout, "       [device]      - defaults to %s\n", DEVICE);
      rc = 0;
      goto EXIT_LABEL;
    }
    
    // Looks for baudrate
    if (argc >= 3)
    {
      if (!strncmp(argv[1], "-b", 2))
      {
        got_baud = 0;
        for (ii = 0; ii < NUM_BAUD_RATES; ii++)
        {
          if (!strncmp(argv[2], rates[ii].bauds, strlen(rates[ii].bauds)))
          {
            *speed = rates[ii].baud;
            got_baud = 1;
            break;
          }
        }
        if (!got_baud)
        {
          fprintf(stderr, "Invalid baud rate specified\n");
          rc = -1;
          goto EXIT_LABEL;
        }
      }
    }

    // Look for device
    if (argc == 2)
    {
      *device = argv[1];
    }
    else if (argc == 4)
    {
      *device = argv[3];
    }
  }

  if (*device[0] != '/')
  {
    fprintf(stderr, "Invalid device specified %s\n", *device);
    rc = -1;
    goto EXIT_LABEL;
  }

  rc = 1;

EXIT_LABEL:  

  return rc;
}

