<?php
	include("../infos/config.inc.php"); // on inclu le fichier de config 
	
	$IdCell = isset($_GET['IdCell']) ? $_GET['IdCell'] : '';
	$temp_man = isset($_GET['temp_man']) ? $_GET['temp_man'] : '';
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
	
	//si la consigne est vide (0.0) alors il faut appliquer la consigne de la saison`
	if($temp_consigne_calendrier == "0.0"){
		// 			$link = mysqli_connect($host,$login,$passe,$bdd);
			if (!$link) {
				die('Erreur de connexion (' . mysqli_connect_errno() . ') '
						. mysqli_connect_error());
			}
		// 
		// //requete pour récupérer la température de consigne de saison
		// $SQL="	SELECT `consigne_temperature`
				// FROM `calendrier_saison` 
				// WHERE `type` = ( 
					// SELECT `saison` 
					// FROM `calendrier` 
					// WHERE `id` = (
						// SELECT DAYOFYEAR(`date`) 
						// FROM `calendrier_30min` 
						// WHERE `id`= $IdCell) ) 
				// LIMIT 0 , 1"; 
		
		// //on envoie la requete
		// $RESULT = @mysqli_query($link, $SQL);
		// //on récupère le résultat
		// $myrow=@mysqli_fetch_array($RESULT); 
		// // on extrait le résultat et on le stocke dans une variable
		// $temp_consigne = $myrow["consigne_temperature"];
		// //libération de la variable
		// mysqli_free_result($RESULT) ;
		// //fermeture session MySQL
		// mysqli_close($link);
		
		// on transmet la valeur écrite dans le champ de saisie, par défaut la valeur est de la saison en cours . sinon il s'agit de la consigne de l'utilisateur.
		$temp_consigne = number_format($temp_man, 1, '.', '');	// on format en float
		echo "$temp_consigne";
	}
	// sinon, la consigne de chauffe est à enlever, on écrit 0.0 dans la table MySQL et on envoie du vide dans la page html
	else{
		$temp_consigne = "0.0";
		echo " ";
	}
	
		$link = mysqli_connect($host,$login,$passe,$bdd);
	if (!$link) {
		die('Erreur de connexion (' . mysqli_connect_errno() . ') '
				. mysqli_connect_error());
	}
	
	$SQL="UPDATE `domotique`.`calendrier_30min` SET `temperature` = '$temp_consigne' WHERE `calendrier_30min`.`id` = $IdCell;";
	//on envoie la requete
	mysqli_query($link,$SQL);
	//fermeture session MySQL
	mysqli_close($link);
?>