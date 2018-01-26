PROJECT_ROOT=.

mkdir -p $PROJECT_ROOT/data
BINDIR=$PROJECT_ROOT/mybins
DATA=$PROJECT_ROOT/data

if test -f $DATA/g26_lab05data_02.csv;
then
	rm $DATA/g26_lab05data_02.csv
fi	
#
#gXXout-<iteration_value>-<rerun_number>.txt

# Beginning of outer loop.

for ((itr_val=1; itr_val <= 1200 ; itr_val++))  # Double parentheses, and naked "LIMIT"
do
  echo -n "$itr_val "
  # ===============================================
  # Beginning of inner loop.
  for ((rerun=1; rerun <= 150 ; rerun++))  # Double parentheses, and naked "LIMIT
  do
  echo -n "$rerun "
  TEMP=$($BINDIR/cs296_26_exe $itr_val) 
  TEMP1=$(sed "s/[^0-9.]//g" <<< "${TEMP}")
  VAR=$(tr -s '\n' ' ' <<< "${TEMP1}") 
  a=($VAR)
  TEMP=${a[0]}","$rerun","${a[1]}","${a[2]}","${a[3]}","${a[4]}","${a[5]}
  echo $TEMP >> $DATA/g26_lab05data_02.csv	 	  
  done
  # End of inner loop.
  # ===============================================
  echo
done
# End of outer loop.

cat ./data/g26_lab05data_02.csv | ./scripts/process.sh >./scripts/temp.dat     
cat ./data/g26_lab05data_02.csv | ./scripts/process2.sh >./scripts/temp2.dat	
