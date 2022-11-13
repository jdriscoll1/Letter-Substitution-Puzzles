<?php 
session_start(); 

$word = $_GET['w_str']; 

/*Check the word has no special characters or numbers*/
if(!ctype_alpha($word)){
	echo "Input must only contain letters"; 
	exit(1); 
}	
/*Verify that the word is not too short*/
if(strlen($word) < 4){
	echo "Input is shorter than 4 characters";
	exit(1); 

}

/*Verify that the word is not too long*/
if(strlen($word) > 4){
	echo "Input is longer than 4 characters"; 
	exit(1);
}	

/*Verify that it is a valid word*/
$wordList = $_SESSION["wordList"]; 
if(!array_key_exists($word, $wordList)){
	echo "Word not found in dictionary";
	exit(1);
}	
/*Validate that it connects properly*/
// Take the previous word
$prevWord = $_GET['w_prev']; 
$c = 0;

//Loop through the current character set 
for($i = 0; $i < 4; $i++){
	if($word[$i] === $prevWord[$i]){
		$c += 1; 
	}
}
if($c < 3){
	echo "Not enough letters in common";
	exit(1); 
}
else if($c > 3){
	echo "Word cannot be same as previous"; 
	exit(1); 

}


/*Verify that it is not already used*/
$wordSet = $_SESSION['wordSet']; 

// Check if the word is in the set of used words
if(in_array($wordList[$word], $wordSet)){
	echo "Word already used"; 
	exit(1); 
}

echo "true"; 	
 
?>
