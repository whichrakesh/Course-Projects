
    {
            if ($3 == "lw"){cycle_count += 5;}
            else if($3 == "bgez" || $3 == "beq" || $3 == "bne" ){cycle_count += 3;}
            else{cycle_count += 4;}
            count++;
    }
    END{
            print "cycle-count for multi-cycle", cycle_count;
            print "cycle-count length for single-cycle", count * 5;
            print "cycle-count for single-cycle", count;
            print "multi cycle implementation is ", (count * 5)/cycle_count ," times faster than single cycle implementation.";
    }

	
	
	   
