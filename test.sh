#!/bin/bash

SPINC="./spindragon"

NC='\033[0m'
RED='\033[1;31m'
GREEN='\033[1;32m'

CNT_FAIL=0
CNT_PASS=0

print_result() {
    printf "[${1}${2}${NC}] $3\n"
}

print_fail() {
    CNT_FAIL=$(expr $CNT_FAIL + 1)
    print_result $RED "FAIL" $line
#    $SPINC $line
}

print_pass() {
    CNT_PASS=$(expr $CNT_PASS + 1)
    print_result $GREEN "PASS" $line
}

DIRECTORY="test/"

if [[ -n "$1" ]]
then
    DIRECTORY="$1"

fi


echo "Expect  Result"
echo "----------------------"


while read line
do
    filename=$(basename "$line")
    extension="${filename##*.}"
    filename="${filename%.*}"
    expected="`echo $filename | cut -d'_' -f 2`"

    $SPINC $line 2>/dev/null >/dev/null
    rc=$?

    if [[ "$expected" == "fail" ]]
    then

        if (($rc != 0))
        then
            print_pass
        else
            print_fail
        fi

    else

        if (($rc != 0))
        then
            print_fail
        else
            print_pass
        fi
    fi

done < <(find $1 -name \*.spin -type f)

echo "----------------------"
echo "Total Passes: $CNT_PASS"
echo "Total Fails:  $CNT_FAIL"
echo "----------------------"
