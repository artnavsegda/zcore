#!/bin/sh

# test 1
../zcore -c $PWD/testconfig.json test1.script > test1.result
if diff test1.result test1.reference; then
  echo "test 1 success"
else
  echo "test 1 fail"
fi
# test 1 cleanup
rm test1.result

