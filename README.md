# Specs
Little Linux command line utility for displaying system and hardware information.

## Features
* Full OS and kernel info
* Uptime (days + hours, minutes, seconds) info
* CPU info
* RAM and file system info
* PCI devices list

## Installation
First compile:
```
gcc -Wall -Wextra -Wpedantic -fdiagnostics-color=always -O3 -std=gnu11 specs.c -o specs
```
Then just run it:
```
./specs
```
