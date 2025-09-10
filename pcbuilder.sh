#!/bin/bash

if [ -z $MSYSTEM ]; then exit 0; fi

ps -W 2> /dev/null | grep -i pcbuilder 2>&1 1> /dev/null
if [ $? == 0 ]; then
    echo "WARNING: Compiling Saturn with sm64pcBuilder2 is deprecated and may no longer be functional." 1>&2
    echo "" 1>&2
    echo "You can download the executables from the official GitHub page (SM64 ROM still required):" 1>&2
    echo "https://github.com/Llennpie/Saturn/releases/latest" 1>&2
    echo "" 1>&2
    echo -n "Would you like to compile anyway? [y/N] " 1>&2
    read -r choice
    if [ "${choice,,}" == "y" ]; then exit 0; fi
    exit 1
fi

