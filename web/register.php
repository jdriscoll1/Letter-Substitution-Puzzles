<?php 
session_start(); 
// First, connect to the server

$servername = 'localhost';
$db = 'flwg_data';
$uname = 'debian-sys-maint';
$password = 'yWlQkHzOIMjVxo5Y';

$conn = new mysqli($servername, $uname, $password, $db);

if($conn->connect_error){
	die("Connection Failed: " . $conn->connect_error);
}


function calculateAge($b_date, $c_date){
	//split the inital date into [0] = Y [1] = m [2] = d
	$i = explode('-', $b_date);
	// split the current date into [0] = Y [1] = m [2] = d	
	$c = explode('-', $c_date);
	// If the current month is less than the the birth month, that means that the age is going to to be y2 - y1 - 1 
	return ($c[0] - $i[0]) - (($c[1] < $i[1]) ? 1 : ($c[1] == $i[1] && $c[2] < $i[2]) ? 1 : 0);  
	
}

$u_name = $_POST['u_name']; 
$p_word = $_POST["p_word"];
$email = $_POST["email"];


// The string is not too long

if(strlen($u_name) < 4 || strlen($u_name) > 20){
	echo "The username must be within 4-20 characters"; 
	exit(0); 
}	
// There are no special characters
if(preg_match('/[^a-z\d]/i', $u_name)){
	echo "The string must contain only alphabetical and numerical characters, no special characters are allowed";	
	exit(0); 
}
// There are no inappropriate usernames (regex)
// Check if the username hasn't already been taken 
$q = sprintf("select * from users where u_name = '%s'", $u_name); 
// Create the sql queyr with the username
	
// Run the query & check the number of rows 
$r = $conn->query($q); 
$n = $r->num_rows; 
if($n > 0){
	echo "This username has been used ". $n . " times";
	exit(0); 
} 



// Validate that the user is at least 13 
$creation_date = date('Y-m-d');

 
// Validate that the email is proper 
if(!filter_var($email, FILTER_VALIDATE_EMAIL)){
	echo "invalid email";
	exit(0); 
}	

// Hash the password 
$o = [ 'cost' => 9, ]; 
$p_word = password_hash($p_word, PASSWORD_DEFAULT, $o); 



//echo "Your data has been validated!"; 
$insert_format = "insert into users (u_name, p_word, email, creation_date) values ('%s', '%s', '%s', '%s');"; 
$insert_query = sprintf($insert_format, $u_name, $p_word, $email, $creation_date);
if($conn->query($insert_query) === TRUE){
	echo "New Record Succesfully Created"; 
}
else{
	echo "Error: " . $insert_query . "<br>" . $conn->error; 
}


/*We need to get the user's id*/
$get_id_query = sprintf("select u_id from users where u_name = '%s';", $u_name); 

$result = $conn->query($get_id_query); 

//echo "Result: " . $result->num_rows; 

// Check if there's at least one row in the database where there is a username 
if($result->num_rows == 1){
	// First get the correct row in the database
	$row = $result->fetch_assoc(); 
	$_SESSION['u_name'] = $u_name; 
	$_SESSION['u_id'] = $row['u_id'];
}
$conn->close(); 
echo ""; 
?>


