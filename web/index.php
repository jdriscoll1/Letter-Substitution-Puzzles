<!DOCTYPE html>
<?php
session_start();
?>

<html>

<head>

<link rel="stylesheet" href="modal.css">
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<link href="https://cdn.jsdelivr.net/npm/bootstrap@5.2.1/dist/css/bootstrap.min.css" rel="stylesheet">
<script src="https://cdn.jsdelivr.net/npm/bootstrap@5.2.1/dist/js/bootstrap.bundle.min.js"></script>
<script src="http://ajax.googleapis.com/ajax/libs/jquery/1.9.1/jquery.min.js"></script>
<script src="template.js" type="text/javascript" def></script>

</head>
<body>
<script>
	document.write(navigationBar);
</script>

<div class="page-content">
<!-- Header-->
<div class="countainer-fluid p-4 bg-primary text-white text-center" style="font-size: 20px">
<h2>Welcome to the Four Letter Word Game</h2>
<p>Start by choosing a game mode</p>
</div>



<!-- The three cards that allow user to select game-->
<div class="container mt-2" style="width: 70%; height: 70%">
	<div class="row">
		<div class="col-lg-4 p-4">
			<div class="card" style="width:275px">
				<div class="card-body">
				<img class="card-img-top" src="./img/graph_flwg.png" alt="flwg_png" style="width:100%">
				<h4 class="card-title">Versus</h4>
				<p class="card-text" style="font-size:14px">Play against opponents taking turns changing the words</p>
				<button id="flwgBtn">Select</button>
				</div>
			</div>
		</div>

		<div class="col-lg-4 p-4">
			<div class="card" style="width:275px">
				<div class="card-body">
				<img class="card-img-top" src="./img/graph_flwp.png" alt="flwp_png" style="width:100%">
				<h4 class="card-title">Pathfinder</h4>
				<p class="card-text" style="font-size:14px">Starting at a word, see if you can reach a goal word through letter substitution</p>
				<button id="myBtn">Select</button>
				</div>
			</div>
		</div>

		<div class="col-lg-4 p-4">
			<div class="card" style="width:275px">	
				<div class="card-body">
				<img class="card-img-top" src="./img/graph_flwc.png" alt="flwc_png" style="width:100%">
				<h4 class="card-title">Challenge</h4>
				<p class="card-text" style="font-size: 14px">Play against opponents taking turns changing the word, each with their own goal word</p>
				<button id="myBtn">Select</button>
				</div>
			</div>
		</div>
	</div>
</div>



<div id="myModal" class="modal">
	<div class="modal-content" style="width:80%">
		<span class="close" id="flwgClose">&times;</span>
		<p>Select your opponent</p>
		<!-- Play Against Computer -->
		<button class="button m-3" onclick="location.href='./flwg_alg.php'");>Computer</button>	
		<!-- Play Against Friend -->
		<button class="button m-3 bg-primary" onclick="queryFLWG();" id="flwg-guest">Guest</button>
		<!-- Customize -->
	</div>
</div>
</div>

<div class="content bg-dark" style="position:fixed; left:0; bottom:0; width:100%; text-alighn: center;">
<p>Test</p>
</div>
<footer class="footer"></footer>
</body>



<script>

var flwgModal = document.getElementById("myModal");

var flwgButton = document.getElementById("flwgBtn");

var flwgClose = document.getElementById("flwgClose");

flwgButton.onclick = function() {
	flwgModal.style.display = "block";
}

flwgClose.onclick = function() {
	flwgModal.style.display = "none";
}

function queryQueue(data, button){
	// When the user queries the queue it sends out an ajax request telling it to run queryQueue.php
	$.ajax({
		type: "POST",
		data: data,
		url: 'queryQueue.php',
		success: function(g_id){
			if(g_id == 1){
				button.innerHTML = "Opponent Found, Booting Game!";	
			}	
		}

	}); 
	// If it returns a game id, it changes the innerHTML to 2 or more players are in 

}
// If a user is in a queue when they log out in decrements the # of players in the queue 
// This Adds a player to the queue when they press the queue button 
function queryFLWG(){
	// This is the modal that will be shown if the user is not logged in 
	var loginModal = document.getElementById("loginModal"); 
	// The boolean that determines if the user is signed in 
	var isSignedIn = (<?php echo (isset($_SESSION['u_id']) ? $_SESSION['u_id'] : -1); ?>); 
	// If the user is not singed in it will open the sign in sheet
	if(isSignedIn == -1){
		loginModal.style.display = "block";	
		flwgModal.style.display = "none";	
	
	}
	// Otherwise it will announce the user is in the queue 
	else{
		// Solves weird error where it removes user from queue multiple times
		var decrement = false; 
		// Now I have to change the button to say that they are "Waiting for an opponent"
		var flwgGuestButton = document.getElementById("flwg-guest");
		var data = {totPlayers: 2, gmode: 0};
		// Then I send out an ajax request asking if there is a queue, if there is not one they will be inserted into the queue 
		flwgGuestButton.innerHTML = "Waiting for Opponents";
		$.ajax({
			type: "POST",
			data: data,
			url: 'initialQueue.php',	
			success: function(jsonData){
				// The result of the game query 	
				var result = JSON.parse(jsonData); 
				// This is the data 
				var queueData = {'q_id': result['q_id']}; 

				// When the user wants to leave they also leave the queue
				// So when the window unloads, it asks the user if they're certain they want to close it 	
				window.addEventListener('beforeunload', function (e) {
					e.preventDefault();
					e.returnValue = '';
					if(!decrement){
						decrement = true; 
						$.ajax({
							type: "POST",
							data: queueData,
							url: "userQuit.php",
							success: function(r){
								decrement = true; 
							}
						});
						flwgGuestButton.innerHTML = "Guest";	

					}

				});	    



				// It is necessary to occasionally check if any users have entered the game
				setInterval(queryQueue, 1000, queueData, flwgGuestButton);
			}
		});
		
	}
}		
		// Send out an ajax request to see if there is someone on the webstie who is already interested in playing: 2 scripts - 1 that sends out initial request and one that checks
		// Script 1: sendQuery --> Sends out game information; Returns q_id
		// Script 2: checkQuery --> Sends out q_id; Returns g_id 
</script>
</html>

