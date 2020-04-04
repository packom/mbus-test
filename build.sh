#!/bin/bash
rm -f master slave
cc master.c mbus-test.c -o master
cc slave.c mbus-test.c -o slave