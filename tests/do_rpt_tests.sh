#!/bin/sh

# find all test programs
PROGS=`find . -type f -name \*.llscr`

# counters
TOTALCNT=0
FAILCNT=0
FAILED=""

# execute all test programs
for i in $PROGS
do
  TESTDIR=`dirname $i`
  TESTSCR=`basename $i`
  TESTNAME=`echo $TESTSCR | sed -e 's/.llscr//g'`
  ((TOTALCNT++))
  echo "Running test $i..."
  ./run_rpt_test.sh $TESTDIR $TESTSCR
  if [ -f $TESTDIR/$TESTNAME.diff ]
  then
    if [ -z $FAILED ]
    then
      FAILED=$i
    else
      FAILED="${FAILED},$i"   
    fi
    ((FAILCNT++))
  fi
done

# print summary
echo
echo "Executed $TOTALCNT tests, with $FAILCNT failures."
if [ $FAILCNT -gt 0 ]
then
  echo $FAILED
fi
