# mbus-test

## Overview

Utilities to drive the Raspberry Pi [M-Bus Master Hat](https://www.packom.net/m-bus-master-hat/) and [M-Bus Slave Hat](https://www.packom.net/m-bus-slave-hat/) as a pair.

## Building

Run:

```
./build.sh
```

## Running

Start the slave first:

```
./slave /dev/serial0
```

Start the master once the slave is running:

```
./master /dev/serial0
```

Replace /dev/serial0 with the serial port the M-Bus device is connected to.  If this is a USB device it may be, for example, /dev/ttyUSB0.

## Operation

Currently works by the Master sending 256 bytes (0..255) and the slave printing them out on receipt.

