<?php
// The goal of this part of the program is to occasionally check if there is another user who has decided to join the game

// Connect to the database 
$servername = 'localhost';
$db = 'flwg_data';
$uname = 'debian-sys-maint';
$password = 'yWlQkHzOIMjVxo5Y';

$conn = new mysqli($servername, $uname, $password, $db);

// Grab the q_id from the server
$q_id = $_POST['q_id']; 
//
// Check the total number of players and current number of players
$playerCheck = sprintf("select totPlayers, currPlayers from queue where q_id = %d", $q_id);
$result = $conn->query($playerCheck);
$row = $result->fetch_assoc();
// Save those numbers in a variable 
$c = $row['totPlayers'];
$t = $row['currPlayers'];

$output = ""; 
if($c == $t){
	$output = 1; 
}

else{
	$output = 0;
}
echo $output; 
// Check if the number of users is equal to the # players
//
// Success Case: There are an adequete # of users to justify starting a new game
// If there are are send back success
//
// Fail Case: No user has decided to enter the game, or more accurately there are not enough users to be able to justify a new game 


?>
