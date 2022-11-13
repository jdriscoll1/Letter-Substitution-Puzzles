<?php 
// 0. User opens website
session_start();

$_SESSION['wordSet'] = [];

//Create the list of words 
$filename = "../docs/test/test.txt";

/*Open the dictionary storing all words*/
$file = fopen($filename, "r") or die ("file doesn't exist"); 

$index = 0; 

//Loop through every line of the ifle
while(!feof($file)){
	$wordList[rtrim(fgets($file, 10))] = $index; 
	$index += 1; 
}

$_SESSION['wordList'] = $wordList; 
array_push($_SESSION['wordSet'], $wordList['aaaa']); 

fclose($file);

?>





















<!--  2. Design the front end of the website -->
<html>
	<head>	 
		<!-- Initialize jquery libraries for ajax accessibility -->
		<script src="http://ajax.googleapis.com/ajax/libs/jquery/1.9.1/jquery.min.js"></script>
		<!-- Initialize any templates -->
		<script src="../templates.js" type="text/javascript" def></script>
		<!-- Initialize Bootstrap -->
		<link href="https://cdn.jsdelivr.net/npm/bootstrap@5.2.1/dist/css/bootstrap.min.css" rel="stylesheet">
		<script src="https://cdn.jsdelivr.net/npm/bootstrap@5.2.1/dist/js/bootstrap.bundle.min.js"></script>	
	</head>

	<body>
		<script>
			document.body.appendChild(template.content);
		</script>
		<h1>Welcome to the Four Letter Word Game</h1>
		<p id="word">aaaa</p>
		

		<!-- The text box which accepts and submits text -->
                <form method="get" class="ajax" action="../includes/functions.php">
                        <input type="text" id="w_str" maxlength="5" onkeyup="unlockSubmit(this.value);">
			<input type="submit" id="submit" disabled>
			<input type="button" value="Reset Game" onclick="resetGame();">
		</form>
		<p id="err">Please enter a word</p>
		<!-- when the button is pressed call ajax -->
                <script>
			let prevWord = 'aaaa'; 

			//Enable the submit button only if the word is 4 character's long 			
			function unlockSubmit(str){
				// The submit button 
				var submitButton = document.getElementById("submit"); 
				var errMsg = document.getElementById("err");
				if(/^[a-zA-Z]+$/.test(str)){
					if(str.length ===  4){
						submitButton.disabled = false;
					}		
					else{
						submitButton.disabled = true; 
					}
				}
				else{
					submitButton.disabled = true; 
				}

			}


			function resetGame(){
				// Set the previous word to pies 
				prevWord = 'pies'; 
				$.ajax({
					type: "GET",
					url: '../includes/resetGame.php',
					success: function(result){
						alert(result);
					}
				});		
						
				document.getElementById("word").innerHTML = 'pies';
			}


			// 3. User enters word causing ajax to open buffer 
			$('form.ajax').on('submit', function(e){
				e.preventDefault();

			  	// This is the textbox that will be updated whenever a new word is entered 	
				var textbox = document.getElementById("word"); 
				// This is the input that is given by the user which is actually a dictionary
				var input = {w_str: $("#w_str").val(), w_prev: prevWord}; 
			
				// This sends a message to verify that the word is valid 
				$.ajax({
					type: "GET",
					data: input,
					url: '../includes/wordCheck.php',
					success: function(errMsg){
						
						// This tells the computer to look for a new word
						if(errMsg === 'true'){
							$.ajax({
                               					type: "GET",
								data: input, 
                        	       				url: '../includes/functions.php',
                      	        				success: function(newWord){
									document.getElementById("word").innerHTML = newWord;
									document.getElementById("err").innerHTML = 'Success, please enter another four letter word'; 
									//update the prev word
									prevWord = newWord;   
								
                               					} // Ends success function 

							}); // Ends the ajax request to find a new word

						} // Ends if the word is valid if statement

						else{
							document.getElementById("err").innerHTML = errMsg; 
						}

					}  // Ends the success function of the word check 

                            	}); //Ends the ajax call for the Word Check
			
		        }); // Ends the Ajax on submit call 
                </script>

        </body>
	

</html>



