#!/bin/bash

pubtest_num=(1 2 3 4 5 6 7 8 9 10)
## Start public testing
echo "Start public testing ..."
for tno in ${pubtest_num[*]}
do
cp "test_inputs/test$tno.txt" "./events.txt"

if cmp -s <(./Dispatcher 800 < ./events.txt) <(cat "srt_test_outputs/srt_results$tno.txt"); then
   echo "Test $tno passed"
else
    echo "Test $tno failed"
fi
done
## Public testing is done!
echo "Public testing is done!"

