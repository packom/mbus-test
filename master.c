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

#include "mbus-test.h"

#define APP_DESC "M-Bus Master Hat tester"

int main(int argc, char *argv[])
{
  int fd, rc, c;
  FILE *file;
  char *device;
  speed_t speed;

  device = handle_args(argc, argv, APP_DESC);
  if (device == NULL)
  {
    return 0;
  }

  fd = set_up_device(device, &file, B2400);
  if (fd < 0)
  {
    return fd;
  }

  fprintf(stdout, "Sending data ...\n");

  for (c = 0; c < 256; c++)
  {
    rc = fputc(c, file);
    if (rc != c)
    {
      fprintf(stderr, "Failed to send char 0x%2.2x %d %d\n", c, rc, errno);
      return 1;
    }
  }

  fprintf(stdout, "Sent\n");

  fclose(file);

  return 0;
}