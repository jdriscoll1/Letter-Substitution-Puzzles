<?php 
session_start();
echo ((isset($_SESSION['u_id']) && $_SESSION['u_id'] != -1) ? $_SESSION['u_id'] : -1); 
?>
