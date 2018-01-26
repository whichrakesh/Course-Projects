for i in `seq 1 512`
do
        ./dineroIV -l1-usize 8k -l1-ubsize 16 -l1-uassoc $i -l1-uwback a  -l1-uwalloc a -l1-uccc  -informat d < ../d4-traces/tex.din 
done

