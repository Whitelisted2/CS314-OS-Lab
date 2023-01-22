#!/bin/bash
cp schedule.c /usr/src/minix/servers/sched/schedule.c;
cd ..;
cd usr/src/;
make build MKUPDATE=yes > log.txt;
