#!/bin/bash
pwd
rm *.gch
# gcc main.c list.c list.h hb_pub.c hb_pub.h -g
gcc main.c list.c list.h hb_pub.c hb_pub.h -g -lpthread