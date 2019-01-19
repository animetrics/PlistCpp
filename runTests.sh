#!/bin/bash

if [[ $1 ]]
then
	cd testing
	../build/${1}/runTests
else
	echo "runTests.sh <config Release/Debug>"
fi
