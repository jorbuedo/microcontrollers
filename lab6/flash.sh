#!/bin/bash

avrdude -c USBasp -p atmega328 -B 5 -U flash:w:$1:i