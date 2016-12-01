<?php
	@include("../infos/config.inc.php"); // on inclu le fichier de config 
	
	$IdCell = isset($_GET['IdCell']) ? $_GET['IdCell'] : '';
	//$IdCell=$_GET["IdCell"];
	//echo "$IdCell";
				
		$link = mysqli_connect($host,$login,$passe,$bdd);
	if (!$link) {
		die('Erreur de connexion (' . mysqli_connect_errno() . ') '
				. mysqli_connect_error());
	}
	
	//on va récupérer la consigne de la cellule cliquée
	$SQL="SELECT `temperature` FROM `calendrier_30min` WHERE `calendrier_30min`.`id` = $IdCell ;";
	//on envoie la requete
	$RESULT = @mysqli_query($link, $SQL);
	//on récupère le résultat
	$myrow=@mysqli_fetch_array($RESULT); 
	// on extrait le résultat et on le stocke dans une variable
	$temp_consigne_calendrier = $myrow["temperature"];
	//echo "$temp_consigne_calendrier";
	//libération de la variable
	mysqli_free_result($RESULT) ;
	//fermeture session MySQL
	mysqli_close($link);
	
	echo "$temp_consigne_calendrier";
?>