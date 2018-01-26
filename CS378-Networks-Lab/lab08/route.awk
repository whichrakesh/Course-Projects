   
BEGIN{FS = "|";}
{ 
 if($6 == "18.0.0.0/8"){
	print $7;
	}
}
END{}
