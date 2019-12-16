#!/bin/sh

# test 1
../zcore -c $PWD/testconfig.json test2.script > test2.result
if diff test2.result test2.reference; then
  echo "test 2 success"
else
  echo "test 2 fail"
fi
# test 2 cleanup
rm test2.result

