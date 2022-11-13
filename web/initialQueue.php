<?php 
session_start(); 
// Connect to the database
$servername = 'localhost';
$db = 'flwg_data';
$uname = 'debian-sys-maint';
$password = 'yWlQkHzOIMjVxo5Y';

$conn = new mysqli($servername, $uname, $password, $db); 

// The queue id
$q_id = -1; 

if($conn->connect_error){
	die("Connection Failed: " . $conn->connect_error);
}
// Check the total # of players in the game
$totPlayers = $_POST["totPlayers"];
// Check the gamemode
$gmode = $_POST["gmode"]; 
// Query the database to see if the data is already there
$queue_query = sprintf("select q_id from queue where totPlayers=%d and gmode=%d", $totPlayers, $gmode);
$result = $conn->query($queue_query);
// If it is, create an associative array 
$q_id = -1; 
$output = ""; 
if($result->num_rows > 0){
	$row = $result->fetch_assoc();
	$q_id = $row['q_id']; 
	// Increment the number of players in that row 	
	$increment_queue = sprintf("update queue set currPlayers = currPlayers + 1 where q_id = %d", $q_id); 
	$conn->query($increment_queue); 
	// Create a game 
	$output = json_encode(["startGame" => true, "q_id" => $q_id]); 
}
else{
	// If it does not, create a queue row
	$create_queue_row = sprintf("insert into queue (g_id, gmode, totPlayers, currPlayers, options_id) values (%d, %d, %d, %d, %d)", -1, $gmode, $totPlayers, 1, -1);

	// Create an associative
	if($conn->query($create_queue_row) !== TRUE){
		echo "Error: " . $create_queue_row . "\n" . $conn->error; 

	}

	else{
		$q_id = $conn->insert_id; 
	}
	$aa = ["startGame" => false, "q_id" => $q_id];
	// Add a row to the database
	$output =  json_encode($aa);
}
echo $output; 
// A user wants to play a game
//
// First, it checks if there is already a player interested in a game
//
// Output: JSON encoded associative array: 
// Result: ['inGame': true/false , 'q_id': n]
//

?>
