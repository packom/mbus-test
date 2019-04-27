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

#define APP_DESC "M-Bus Slave Hat tester"

int main(int argc, char *argv[])
{
  int fd, rc;
  size_t rc2;
  char array[1];
  FILE *file;
  char *device;
  speed_t speed;

  rc = handle_args(argc, argv, APP_DESC, &device, &speed);
  if (rc < 1)
  {
    return rc;
  }

  fd = set_up_device(device, &file, speed);
  if (fd < 0)
  {
    return fd;
  }

  fprintf(stdout, "Waiting for data ...\n");

  do
  {
    rc2 = fread(array, 1, 1, file);
    if (rc2 == 1)
    {
      fprintf(stdout, "%2.2x", array[0]);
    }
    else
    {
      rc = feof(file);
      if (rc)
      {
        //fprintf(stdout, "\nHit EOF\n", rc );
      }
      rc = ferror(file);
      if (rc)
      {
        fprintf(stdout, "\nHit error %d\n", rc);
      }
      clearerr(file);
    }
  } while (1);

  fclose(file);

  return 0;
}
