#!/bin/sh

# test 3
../zcore -c $PWD/testconfig.json test3.script > test3.result
if diff test3.result test3.reference; then
  echo "test 3 success"
else
  echo "test 3 fail"
fi
# test 3 cleanup
rm test3.result

