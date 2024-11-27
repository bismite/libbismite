#!/bin/bash

echo "Download $2 from $1"
if type curl > /dev/null; then
	curl -LsS -o $2.tmp $1
else
	wget -O $2.tmp $1
fi
mv $2.tmp $2
