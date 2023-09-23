#!/bin/bash

clear
echo -n > store.txt

for i in {1..3};
do
  for j in {1..3};
  do
    echo Running ... ./a.out $i $j
    bash test.sh $i $j
  done
done

echo -e "\nDone! Check store.txt for outputs.\n"