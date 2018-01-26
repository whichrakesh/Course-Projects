<?php
	session_start();
	if(!isset($_SESSION['id']))
		$_SESSION['id'] = rand();
	$name = $_REQUEST['name'];
	$password = $_REQUEST['pwd'];
	if($name != "" && $password != ""){
		$hash = md5('password');
		if( hash_hmac("sha256", $_SESSION['id'] , $hash) == $password){
			echo "authenticated";
		} else {
			echo "invalid password";
		}
	}	


?>