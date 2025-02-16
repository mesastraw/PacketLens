#!/bin/bash

# The purpose of this script is to compile both the go client and c packet catcher
# It will then store the binarys in their own bin dir
# Also allows for easy cleaning of bin dir and binarys

# Dont know why i put this here 
# Too lazy to change it if it aint broke dont fix it
root=$PWD

buildC () {
 make -C $root/code/c/
}

buildGo () {
  cd $root/code/go/src/
  go build -o client
}

clean() {
  if test -d $root/bin; 
  then
    echo "Cleaning up!"
    make -C $root/code/c clean
    rm -rf $root/bin
    exit 0
  fi

  echo "Directory does not exist"
  exit 1
}

"$@"

if test -d $root/bin; then
echo "Its exists!"
buildC
buildGo
mv $root/code/c/packetCatcher $root/bin
mv $root/code/go/src/client $root/bin
exit 0
fi

buildC
buildGo
mkdir $root/bin
mv $root/code/c/packetCatcher $root/bin
mv $root/code/go/src/client $root/bin
