#!/bin/bash

if [[ $1 && $2 ]]
then
	cd testing
	../${2}/${1}/runTests
else
	echo "runTests.sh <config Release/Debug> <Platform Linux, OSX>"
fi
