#!/bin/bash

if [ $1 == 1 ]
then
  echo -n "# Jobs (1 - 1024)" >> store.txt
elif [ $1 == 2 ]
then
  echo -n "# Jobs (1 - 100)" >> store.txt
elif [ $1 == 3 ]
then
  echo -n "# Jobs (500 - 1000)" >> store.txt
fi

if [ $2 == 1 ]
then
  echo -e " with Best Fit\n" >> store.txt
elif [ $2 == 2 ]
then
  echo -e " with Worst Fit\n" >> store.txt
elif [ $2 == 3 ]
then
  echo -e " with First Fit\n" >> store.txt
fi
g++ SE20UCSE105.cpp -o SE20UCSE105
./SE20UCSE105 $1 $2 >> store.txt

rm -rf SE20UCSE105