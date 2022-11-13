var navigationBar = `
<!-- THE REGISTER MODAL--> 
<div id="container mt-3"> <div class="modal" id="registerModal">
		<div class="modal-content" style="width: 80%">

			<span class="close" id="registerClose">&times;</span>
			<h1>Register</h1>
			<form class="rc" id="registerForm">
				<div class="mb-3 mt-3">
					<label for="uname" class="form-label">Username: </label>
					<input type="text" class="form-control" id="u_name" placeholder="Enter username" maxlength=20 name="u_bname">
				</div>
				<div class="mb-3 mt-3">
					<label for="pword" class="form-label">Password: </label>	
					<input type="password" class="form-control" id="p_word" placeholder="Enter password" maxlength=20 name="p_word">
				</div>

				<div class="mb-3 mt-3">
					<label for="pword2" class="form-label">Re-enter Password: </label>	
					<input type="password" class="form-control" id="p_word2" placeholder="Re-enter password" maxlength=20 name="p_word2">
				</div>	
				<div class="mb-3 mt-3">
					<label for="email" class="form-label">Email:</label>
					<input type="email" class="form-control" id="email" placeholder="Enter email" name="email">
				</div>	
				<div class="mb-3 mt-3">
					<button type="submit" class="btn btn-primary">Submit</button>
				</div>
			</form>
		</div>
	</div>
</div>

<!-- The login modal -->
<div id="container mt-3">
	<div class="modal" id="loginModal">
		<div class="modal-content" style="width: 80%">

			<span class="close" id="loginClose">&times;</span>
			<h1>Login:</h1>
			<form class="rc" id="loginForm">
				<div class="mb-3 mt-3">
					<label for="uname" class="form-label">Username: </label>
					<input type="text" class="form-control" id="u_name_login" placeholder="Enter username" maxlength=20 name="u_bname">
				</div>
				<div class="mb-3 mt-3">
					<label for="pword" class="form-label">Password: </label>	
					<input type="password" class="form-control" id="p_word_login" placeholder="Enter password" maxlength=20 name="p_word">
				</div>

				<div class="mb-3 mt-3">
					<button type="submit" class="btn btn-primary">Submit</button>
				</div>
			</form>
		</div>
	</div>
</div>

<!-- Navbar -->
<nav class="navbar navbar-expand-lg p-0 navbar-dark bg-dark">
	<div class="container-fluid p-0" style="float:right; fill:white;">
		<a class="navbar-brand" style="fill:white; padding-left:10px" href="./index.php">
			<img src="./img/home.svg" alt="logo" width="33" height="33">
		</a>	
		<button class="navbar-toggler" type="button" data-bs-toggle="collapse" data-bs-target="#mynavbar">
			<span class="navbar-toggler-icon"></span>
		</button>
		<div class="collapse navbar-collapse" id="mynavbar">
			<ul class="navbar-nav me-auto mb-2 mb-sm-0">
				<li class="nav-item" style="font-size:24px">
					<a class="nav-link" href="javascript:void(0)">Help</a>
				</li>
				<li class="nav-item" style="font-size:24px;">
					<a class="nav-link" href="javascript:void(0)">About</a>
				</li>
				<li>
					<a class="nav-link" style="font-size:24px;" href="javascript:void()">More</a>
				</li>
			</ul>	
		<ul class="nav navbar-nav navbar-right" id="loginInfo">
			<script>	
				// Goal: Create two different navbar outputs depending if the user is logged in 
				// 2 different strings, a. logged in variants b. not logged in variant
				// Not Logged in variant is what we already have

				// Logged Out: If the user is not logged in it will output Register & Login	
      				var l1 = '<li><a class="nav-link" style="font-size:24px;" id="registerButton" href="#registerModal" date-toggle="modal"><span class="glyphicon glyphicon-user"></span> Sign Up</a></li><li><a class="nav-link" style="font-size:24px;" id="loginButton" href="#login"><span class="glyphicon glyphicon-log-in"></span>Login</a></li>';
				// Logged in: If the user is logged in it will output Profile & Logout variant is what we want to get 
      				var l2 = '<li><a class="nav-link" style="font-size:24px;" id="profileButton" href="#profile" date-toggle="modal"><span class="glyphicon glyphicon-user"></span>Profile</a></li><li><a class="nav-link" style="font-size:24px;" id="logoutButton" href="#logout"><span class="glyphicon glyphicon-log-in"></span>Logout</a></li>';

				var signedIn = -1; 
				$.ajax({
					type: "GET",
					url: "getSignedIn.php",
					success: function(s){
						document.getElementById("loginInfo").innerHTML = (s == -1) ? l1 : l2;
						// If the user is not signed in 
						if(s == -1){
							// Obtain the buttons
							var registerButton = document.getElementById("registerButton");
							var registerModal = document.getElementById("registerModal");
							var registerClose = document.getElementById("registerClose");

							var loginButton = document.getElementById("loginButton");
							var loginModal = document.getElementById("loginModal");
							var loginClose = document.getElementById("loginClose");

							if (registerButton != null) {
								registerButton.onclick = function(){
									registerModal.style.display = "block";
								}
							}
							if(registerClose != null){
								registerClose.onclick = function() {
									registerModal.style.display = "none";
								}
							}
							if(loginButton != null){	
								loginButton.onclick = function(){
									loginModal.style.display = "block";
								}
							}
							if(loginClose != null){
								loginClose.onclick = function(){
									loginModal.style.display = "none";
								}
							}	

							// When the button is clicked it is necessary to send an ajax request to register.php which should be renamed register.php
							if(document.getElementById("loginForm") != null){
								$("#loginForm").submit(function(e){
									e.preventDefault();
									var userData = {
										u_name: document.getElementById('u_name_login').value,
										p_word: document.getElementById('p_word_login').value,
									};
									$.ajax({
										type: "POST",
										data: userData,
										url: 'login.php',
										success: function(u_id){
											console.log("Success");
											window.location.href = "";
										}
									
									});
								
								}); 	
							}
							if(document.getElementById("registerForm") != null){
								// When the button is clicked it is necessary to send an ajax request to login.php which sends the data to teh login form 
								$("#registerForm").submit(function(e){
									e.preventDefault();
									var userData = {
										u_name: document.getElementById('u_name').value,
										p_word: document.getElementById('p_word').value,
										email: document.getElementById('email').value
									};
								 
									$.ajax({
										type: "POST",
										data: userData,
										url: 'register.php',
										success: function(o){
											console.log("Success");
											window.location.href = ""; 
										}
									
									});
								
								}); 
			
 

							}
						}
	
						// If the user is signed in 
						else{
							var logoutButton = document.getElementById("logoutButton");
							if(logoutButton != null){
								logoutButton.onclick = function(){
									
									$.ajax({
										type: "GET",
										url: "logout.php",
										success: function(e){
											var x = 0; 
											window.location.href = "";
										}
									});
								}
							}
						}
					}
				});
			</script>
   		 </ul>	
	</div>

</nav>
<!-- END COPY -->

`
