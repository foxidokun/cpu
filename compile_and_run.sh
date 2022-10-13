#!/bin/bash
./bin/asm $1 /tmp/a.bin && ./bin/cpu /tmp/a.bin
