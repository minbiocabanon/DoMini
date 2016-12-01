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
	
	//on va r�cup�rer la consigne de la cellule cliqu�e
	$SQL="SELECT `temperature` FROM `calendrier_30min` WHERE `calendrier_30min`.`id` = $IdCell ;";
	//on envoie la requete
	$RESULT = @mysqli_query($link, $SQL);
	//on r�cup�re le r�sultat
	$myrow=@mysqli_fetch_array($RESULT); 
	// on extrait le r�sultat et on le stocke dans une variable
	$temp_consigne_calendrier = $myrow["temperature"];
	//echo "$temp_consigne_calendrier";
	//lib�ration de la variable
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
		// //requete pour r�cup�rer la temp�rature de consigne de saison
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
		// //on r�cup�re le r�sultat
		// $myrow=@mysqli_fetch_array($RESULT); 
		// // on extrait le r�sultat et on le stocke dans une variable
		// $temp_consigne = $myrow["consigne_temperature"];
		// //lib�ration de la variable
		// mysqli_free_result($RESULT) ;
		// //fermeture session MySQL
		// mysqli_close($link);
		
		// on transmet la valeur �crite dans le champ de saisie, par d�faut la valeur est de la saison en cours . sinon il s'agit de la consigne de l'utilisateur.
		$temp_consigne = number_format($temp_man, 1, '.', '');	// on format en float
		echo "$temp_consigne";
	}
	// sinon, la consigne de chauffe est � enlever, on �crit 0.0 dans la table MySQL et on envoie du vide dans la page html
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