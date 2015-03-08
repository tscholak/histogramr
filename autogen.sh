#!/bin/sh
#
# Run this to generate the configuration scripts after a fresh
# repository clone/checkout.
#
# USAGE: ./autogen.sh [--verbose]
#
# where --verbose shows stdout and stderr of every command (by
# default they are redirect to /dev/null).

arg1=$1

step() {
    local message="$1"
    local command="$2"

    printf "$message... "

    if test "$arg1" = --verbose; then
  eval $command
    else
  eval $command >/dev/null 2>&1
    fi
    local result=$?

    if test "$result" = "0"; then
  printf "\033[32mok\033[0m\n"
    else
  printf "\033[31mfailed\033[0m\n  ** \"$command\" returned $result\n"
  exit $result
    fi
}


srcdir=`dirname $0`
test -z "$srcdir" && srcdir=.

step  "Checking top-level directory" \
  "test -f '$srcdir/configure.ac'"

cd "$srcdir"

step  "Creating dummy ChangeLog, if needed" \
  "test -f './ChangeLog' || touch './ChangeLog'"
step  "Creating NEWS ChangeLog, if needed" \
  "test -f './NEWS' || touch './NEWS'"
step  "Creating README ChangeLog, if needed" \
  "test -f './README' || touch './README'"

step  "Regenerating autotools files" \
  "autoreconf -isf -Wall"


printf "Now run \033[1m./configure\033[0m to customize your building\n"