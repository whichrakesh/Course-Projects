<?php
session_start();
if(!isset($_SESSION['id']))
	$_SESSION['id'] = rand();
?>
<html>
<head>
</head>
<body>
	<script src="http://crypto-js.googlecode.com/svn/tags/3.1.2/build/rollups/md5.js"></script>
	<script src="http://crypto-js.googlecode.com/svn/tags/3.1.2/build/rollups/hmac-sha256.js"></script>
	<div style="margin:auto; width:50px">
	<form method="POST" action="login.php" id="myForm">
		<input type="hidden" name="session_id" id="session_id" value="<?php echo $_SESSION['id']?>">
		Name:
		<input type="text" name="name">
		</input>
		Password:
		<input id="pwd" type="password" name="pwd">
		</input><br><br>
		<button onclick="send()" type="button" name="login">Login</button>
	</form>
	</div>
	<script>	
	function send(){
		var hash = String(CryptoJS.MD5(document.getElementById("pwd").value));
		var session_id = document.getElementById("session_id").value;
		document.getElementById("pwd").value = CryptoJS.HmacSHA256(session_id, hash);	
		document.getElementById("myForm").submit();
	}
	</script>
</body>
</html>