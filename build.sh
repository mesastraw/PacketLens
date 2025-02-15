#!/bin/bash

# The purpose of this script is to compile both the go client and c packet catcher
# It will then store the binarys in their own bin dir
# Also allows for easy cleaning of bin dir and binarys

buildC () {
 make -C $PWD/c/
}

clean() {
  if test -d $PWD/bin; 
  then
    echo "Cleaning up!"
    rm -rf $PWD/bin
    exit 0
  fi

  echo "Directory does not exist"
  exit 1
}

"$@"

if test -d $PWD/bin; then
echo "Its exists!"
buildC
mv $PWD/c/packetCatcher $PWD/bin
exit 0
fi

buildC
mkdir $PWD/bin
mv $PWD/c/packetCatcher $PWD/bin
