   
BEGIN{FS = "|";}
{ 
 if(!($5 in ASes)){
	count++;
	ASes[$5] = 1;
	print $5;
	}
}
END{print count;}

