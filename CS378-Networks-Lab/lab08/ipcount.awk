   
BEGIN{FS = "|"; last_ip = "0"}
{ 
 if($6 != last_ip){
	count++;
	last_ip = $6;
	#print $6;
	}
}
END{print count;}

