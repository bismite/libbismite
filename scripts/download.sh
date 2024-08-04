#!/bin/bash

if type curl > /dev/null; then
	curl -LsS -o $2 $1
else
	wget -O $2 $1
fi
