<?php
session_start(); 
/*What does the login file do?*/
// 1) Connects to the database 
// First, connect to the server
$servername = 'localhost';
$db = 'flwg_data';
$uname = 'debian-sys-maint';
$password = 'yWlQkHzOIMjVxo5Y';

$conn = new mysqli($servername, $uname, $password, $db);

if($conn->connect_error){
	die("Connection Failed: " . $conn->connect_error);
}


// 2) Checks if the username & Password Match

// Get the username & password from the server
$u_name = $_POST['u_name'];
$p_word = $_POST['p_word'];
$u_id = -1; 


// Access the database to see if the username & password are there

// Get query
$query_str = "select u_id, u_name, p_word from users where u_name = '%s';"; 
$query = sprintf($query_str, $u_name);

$result = $conn->query($query); 

// Check if there's at least one row in the database where there is a username 
if($result->num_rows == 1){
	// First get the correct row in the database
	$row = $result->fetch_assoc(); 
	$_SESSION['u_name'] = $u_name; 
	$p_word_check = $row['p_word'];
	// Check if the verified password is the same is what's being passed in
	if(password_verify($p_word, $p_word_check)){
		$u_id = $row['u_id']; 		
		$_SESSION['u_id'] = $u_id;
	}
}
// 3) Echo's the id #
echo $u_id; 






?>
