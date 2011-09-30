if [[ $1 ]]
then
	cd testing
	../${1}/Release/runTests
else
	echo "runTests.sh <Platform Linux, Windows, OSX>"
fi
