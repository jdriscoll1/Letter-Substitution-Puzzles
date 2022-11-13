<?php
session_start();
$q_id = $_POST['q_id']; 

if($q_id != -1){
	// Connect to the database
	$servername = 'localhost';
	$db = 'flwg_data';
	$uname = 'debian-sys-maint';
	$password = 'yWlQkHzOIMjVxo5Y';

	$conn = new mysqli($servername, $uname, $password, $db); 
	
	// Take the q_id's row 
	$decrement_queue = sprintf("update queue set currPlayers = currPlayers - 1 where q_id = %d", $q_id);
	$conn->query($decrement_queue); 
	echo $decrement_queue; 
}
?>
