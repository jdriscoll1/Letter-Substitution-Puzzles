<?php 
// 0. User opens website
session_start();

$_SESSION['wordSet'] = [];

//Create the list of words 
$filename = "./docs/4/Four_Letter_Dictionary.txt";

/*Open the dictionary storing all words*/
$file = fopen($filename, "r") or die ("file doesn't exist"); 

$index = 0; 

srand(time());

//Loop through every line of the file
while(!feof($file)){
	$wordList[rtrim(fgets($file, 10))] = $index; 
	$index += 1; 
}

$initWordID = rand(0, 2146); 

$keys = array_keys($wordList); 
$initWord = $keys[$initWordID]; 
$_SESSION['wordList'] = $wordList; 
array_push($_SESSION['wordSet'], $initWordID); 
fclose($file);


?>

<!--  2. Design the front end of the website -->
<html>
	<head>	 
		<!-- Initialize jquery libraries for ajax accessibility -->
		<script src="http://ajax.googleapis.com/ajax/libs/jquery/1.9.1/jquery.min.js"></script>
		<!-- Initialize any templates -->
		<script src="template.js" type="text/javascript" def></script>
		<!-- Initialize Bootstrap -->
		<link href="https://cdn.jsdelivr.net/npm/bootstrap@5.2.1/dist/css/bootstrap.min.css" rel="stylesheet">
		<script src="https://cdn.jsdelivr.net/npm/bootstrap@5.2.1/dist/js/bootstrap.bundle.min.js"></script>	
		<link rel="stylesheet" href="modal.css">
	</head>

	<body>
		<!-- Navbar -->
		<script>
			document.write(navigationBar);
		</script>
		<div style="text-align:center;padding-top:25px">	
			<h1 style="center">Versus: Beat The Bot!</h1>
		</div>
			
		<!-- Word Display -->
		<!-- The full box -->

                <form method="get" class="ajax" id="wordForm" action="includes/functions.php">
		<div class="d-flex justify-content-center flex-nowrap container" style="padding:20px">
			<!-- The grey box -->
			<div class="d-flex justify-content-center flex-nowrap bg-dark" id="b1" style="padding:20px; width:1000px; height: 250px;">
				<div class="row">
					<!-- The blue box -->
					<div class="col-sm-12 col-xl-12 justify-content-center d-flex">
						<div class="bg-primary text-white mx-10 p-3"; style="width:550px; text-align:center">
							<!-- The text -->
							<h1 id="word" style="center"><?php echo "$initWord"?></h1>
						</div>
					</div>
					
					<div class="row">
				
						<!-- Insert input #1  -->
						<div class="form-group col-xl-12" style="padding:20px">
							<input type="text" maxlength=5 placeholder="Enter a four letter word" class="form-control" id="w_str">
						</div>
				

					</div>

					<div class="row">
						<div class="form-group col-xl-12">
							<div class="alert alert-danger" id="err" hidden><strong>Error: </strong>Input was not long enough</div>	
						</div>
					</div>
					<div class="row justify-content-center flex-nowrap">
						<div class="col-sm-4 justify-content-center d-flex">
							<button type="submit" id="submit" class="btn btn-success" style="font-size:25px">Enter word!</button>
						</div>
						<div class="col-sm-4 justify-content-center d-flex"> 
							<button type="button" id="resign" class="btn btn-danger justify-content-center d-flex" style="font-size:25px" href="index.php">Resign</button>
						</div>
					
					</div>				

				</div>

			</div>
		</div>
		</form>	

		<div id="winModal" class="modal">
			<div class="modal-content">
				<!-- Header: Congrats! You Won-->
				<div class="container-fluid p-5 text-center bg-success text-white" style="width:100%">
					<h1>Congratulations, you've won!</h1>
				</div>	
				
				<!-- Check Mark png -->
				<img src="./img/star.svg" alt="star" style="width:35%; margin:auto; height:35%">
				

				<!-- New Game/Home Screen -->
				<div class="row justify-content-center flex-nowrap" style="padding-top:25px">
				
					<div class="col-sm-4 justify-content-center d-flex">
						<form action="flwg_alg.php">
							<button type="submit" id="submit" class="btn btn-success" style="font-size:25px">Play Again</button>
						</form>
					</div>
						<div class="col-sm-4 justify-content-center d-flex"> 
							
					<form action="index.php">
							<button type="submit" class="btn btn-secondary justify-content-center d-flex" style="font-size:25px">Home Page</button>

					</form>
						</div>
				</div>
			</div>
		</div>

		<div id="loseModal" class="modal">
			<div class="modal-content">
				<!-- Header: Congrats! You Won-->
				<div class="container-fluid p-5 text-center bg-danger text-white" style="width:100%">
					<h1>Defeat, better luck next time!</h1>
				</div>	
				
				<!-- Check Mark png -->
				<img src="./img/lose.svg" alt="star" style="width:35%; margin:auto; padding-top: 20px; height:35%">
				

				<!-- New Game/Home Screen -->
				<div class="row justify-content-center flex-nowrap" style="padding-top:25px">
				
					<div class="col-sm-4 justify-content-center d-flex">
						<form action="flwg_alg.php">
							<button type="submit" id="submit" class="btn btn-primary" style="font-size:25px">Play Again</button>
						</form>
					</div>
						<div class="col-sm-4 justify-content-center d-flex"> 
							
					<form action="index.php">
							<button type="submit" class="btn btn-secondary justify-content-center d-flex" style="font-size:25px">Home Page</button>

					</form>
				</div>

			</div>
		</div>
		<div class="content bg-dark" style="position:fixed; left:0; bottom:0; width:100%; text-align: center;">
			<p>Display</p>
		</div>

			




	        <script>
			
			var initWord = "<?php echo $initWord?>";
			let prevWord = initWord; 
			let resignBtn = document.getElementById("resign");	
			resignBtn.onclick = function() {	
				document.getElementById("loseModal").style.display="block";
			}
			
			function resetGame(){
				// Set the previous word to pies 
				prevWord = initWord; 
				$.ajax({
					type: "GET",
					url: 'includes/resetGame.php',
					success: function(result){
					}
				});		
						
				document.getElementById("word").innerHTML = initWord;
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
					url: 'includes/wordCheck.php',
					success: function(errMsg){
						var alertBox = document.getElementById("err"); 
						// This tells the computer to look for a new word
						if(errMsg === 'true'){
							$.ajax({
                               					type: "GET",
								data: input, 
                        	       				url: 'includes/botPly.php',
                      	        				success: function(newWord){
									// The bot has lost 
									if(newWord.trim() === 'l'){	
										document.getElementById("winModal").style.display="block";

									}										
									else{
										document.getElementById("word").innerHTML = newWord;
										document.getElementById("b1").style.height = 250;
										if(!alertBox.hasAttribute('hidden')){
											alertBox.setAttribute('hidden', 'hidden'); 
										}
										//update the prev word
										prevWord = newWord;

									}	
								
                               					} // Ends success function 

							}); // Ends the ajax request to find a new word

						} // Ends if the word is valid if statement

						else{
							// Change the height of the surrounding grey box
							document.getElementById('b1').style.height = 325; 
							// Show the alert block
							if(alertBox.hasAttribute('hidden')){
								alertBox.removeAttribute('hidden');
							}
							// Chagne the alert block to the proper text 
							alertBox.innerHTML = "<strong>Error: </strong>" + errMsg; 	
							//document.getElementById("err").innerHTML = errMsg; 
						}

					}  // Ends the success function of the word check 

                            	}); //Ends the ajax call for the Word Check
				document.getElementById("wordForm").reset(); 
		        }); // Ends the Ajax on submit call 
                </script>

        </body>
	

</html>
