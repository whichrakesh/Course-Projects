BEGIN{FS = "|";}
{ 
 if(!($6 in Ips)){
	count++;
	Ips[$6] = 1;
	}
 else {
	Ips[$6] += 1;
  }
}
END{
	for(ip in Ips){
		print ip,Ips[ip];
		if(Ips[ip] > max){
			max = Ips[ip];
			max_ip = ip;
		}
	}
	print max_ip, max
	#print count;
}

