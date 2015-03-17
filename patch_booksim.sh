#!/bin/bash

BOOKSIM_PATH=$1

if [ -z $BOOKSIM_PATH ]; then
  echo "No path to booksim specified";
  echo "FORMAT: ./path_booksim.sh [PATH_TO_BOOKSIM]";
  exit 0;
fi

echo "Getting booksim source code in $BOOKSIM_PATH/src ..."
cp -rf $BOOKSIM_PATH/src/* src/intersim/.

echo "Applying patch ... "
cd src/intersim/
patch -s -p1 < ../../booksim.patch

echo "Copying over integration code ... "
cd ../../
cp integration_code/* src/intersim/.

echo "Patch complete. Ready to build."
