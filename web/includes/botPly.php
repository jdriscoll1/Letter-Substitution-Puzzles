<?php 
session_start();

class flwg_structs {
	
	private $IntToWord; 
	private $WordToInt; 
	private $WordSet; 	
	private $w_id; 
	private $path; 
	
	/************** INITIALIZE DATA STRUCTURES ****************/ 
	public function __construct() {	
		/*Create the Int To Word Structure: Array*/ 
		$this->IntToWord = Allocate_IntToWordStruct();  
		/*Create the Word To Int Structure: 2D Array of Trees*/ 
		$this->WordToInt = Allocate_WordToInt(); 
		/*Stores all of the words*/	
		$this->WordSet = init_WordSet(2234); 
		/*Arbitarily chosen first word*/ 
		$this->w_id = $_GET['w_str']; 
		/*Path to the file*/ 
		$this->path = "/var/www/html/flwg/docs/4/Four_Letters_Connections.txt"; 
		/*Fills the hash maps*/	
		Initialize_HashMaps($this->WordToInt, $this->IntToWord, $this->path); 
		/*Calculate the word's id*/	
		$this->w_id = Convert_WordToInt($_GET['w_str'], $this->WordToInt);
		/*Push the new word to the word set*/
		array_push($_SESSION['wordSet'], $this->w_id); 
	}

	public function fillWordSet(){
		
		foreach($_SESSION['wordSet'] as $w_id){
			markUsed_WordSet($w_id, $this->WordSet);		
		
		}
	}
	public function _botPly(){
		return botPly($this->w_id, 12, $this->IntToWord, $this->WordSet);
	}

	public function _Convert_IntToWord($id){
		return Convert_IntToWord($id, $this->IntToWord); 
	}

	public function _Convert_WordToInt($str){
		return Convert_WordToInt($str, $this->WordToInt); 
	}	
}
$flwg = new flwg_structs();
$flwg->fillWordSet(); 
$new_id = $flwg->_botPly(); 
$output = ''; 
if($new_id == -1){
	$output = 'l';
}
else{
	//Add the word to the word set
	array_push($_SESSION['wordSet'], $new_id); 
	// Convert the integer into a word
	$output = $flwg->_Convert_IntToWord($new_id);
}
echo $output; 
?>



