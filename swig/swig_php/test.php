<?php
	$IntToWord = Allocate_IntToWordStruct();
	$WordToInt = Allocate_WordToInt(); 
	$wordSet = init_WordSet(2234);

	$w_id = 5;
	Initialize_HashMaps($WordToInt, $IntToWord, "../../docs/4.txt");
	$w_id = botPly($w_id, 5, $IntToWord, $wordSet);
	echo checkIfUsed_WordSet(4, $wordSet);
	echo Convert_IntToWord(5, $IntToWord);
	Free_HashMaps($WordToInt, $IntToWord);
	free_WordSet($wordSet);
?>
