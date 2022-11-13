<?php
$servername = 'localhost';
$uname = 'debian-sys-maint';
$password = 'yWlQkHzOIMjVxo5Y';

$conn = new mysqli($servername, $uname, $password);

if($conn->connect_error){
	die("Connection Failed: " . $conn->connect_error);
}
echo "Successful Connection";
?>
