<?php
	include("../infos/config.inc.php"); // on inclu le fichier de config 
	
	$idjour = isset($_GET['idjour']) ? $_GET['idjour'] : '';
	$typejour = isset($_GET['typejour']) ? $_GET['typejour'] : '';
	echo "idjour = $idjour<br>";
	echo "typejour = $typejour<br>";
	
				$link = mysqli_connect($host,$login,$passe,$bdd);
			if (!$link) {
				die('Erreur de connexion (' . mysqli_connect_errno() . ') '
						. mysqli_connect_error());
			}
	

	//on met à jour le type de jour dans la table calendrier_saison`
	$SQL = "UPDATE `domotique`.`calendrier` SET `type_jour` = '$typejour' WHERE `calendrier`.`id` = $idjour;";
	mysqli_query($link,$SQL);
	//fermeture session MySQL
	mysqli_close($link);
	echo "type jour mis à jour dans la table calendrier <br>";
				
				$link = mysqli_connect($host,$login,$passe,$bdd);
			if (!$link) {
				die('Erreur de connexion (' . mysqli_connect_errno() . ') '
						. mysqli_connect_error());
			}
	
	//requete pour récupérer la température de consigne de saison
	$SQL="	SELECT `consigne_temperature`
			FROM `calendrier_saison` 
			WHERE `type` = ( 
				SELECT `saison` 
				FROM `calendrier` 
				WHERE `id` = $idjour ) 
			LIMIT 0 , 1"; 
	
	//on envoie la requete
	$RESULT = @mysqli_query($link, $SQL);
	//on récupère le résultat
	$myrow=@mysqli_fetch_array($RESULT); 
	// on extrait le résultat et on le stocke dans une variable
	$temp_consigne = $myrow["consigne_temperature"];
	//libération de la variable
	mysqli_free_result($RESULT) ;
	//fermeture session MySQL
	mysqli_close($link);
	echo "temp_consigne = $temp_consigne<br>";
	
	// Maintenant, on va traiter chaque créneau de chauffe du type de jour , puis remplir la table planning en conséquence (température de consigne à mettre à jour dans les créneau concernés)
	
	//on commence par mettre à 0 toutes les cellules
				$link = mysqli_connect($host,$login,$passe,$bdd);
			if (!$link) {
				die('Erreur de connexion (' . mysqli_connect_errno() . ') '
						. mysqli_connect_error());
			}
	
	$SQL = "UPDATE `domotique`.`calendrier_30min` 
			SET `temperature` = 0.0 
			WHERE `calendrier_30min`.`date` = (SELECT `date` FROM `calendrier` WHERE `id` = $idjour);";
	mysqli_query($link,$SQL);
	//fermeture session MySQL
	mysqli_close($link);
	echo "Consigne = 0.0 pour idjour=$idjour<br>";

	
	// Preparation de la requete MySQL
	// on ne récupère que les infos du jour en cours
	//sprintf(query, "SELECT * FROM `calendrier_type_jour` WHERE `type_jour` = '%s'", stJourTraite.inf_type_jour);
				$link = mysqli_connect($host,$login,$passe,$bdd);
			if (!$link) {
				die('Erreur de connexion (' . mysqli_connect_errno() . ') '
						. mysqli_connect_error());
			}
	
	//requete pour récupérer les heures de debut et fin des creneaux de chauffe
	$SQL="	SELECT `heure_debut`, `heure_fin` FROM `calendrier_type_jour` WHERE `type_jour` = \"$typejour\"";
	//on envoie la requete
	$RESULT = @mysqli_query($link, $SQL);
	// pour chq créneau de chauffe existant pour ce type de jour
	while($myrow = @mysqli_fetch_array($RESULT)) {
		// on recupère l'heure de début
		$hdebut = $myrow["heure_debut"];
		// on recupère l'heure de fin
		$hfin = $myrow["heure_fin"];
		echo "hdebut = $hdebut - hfin = $hfin<br>";
		// on récupère les id des creneaux de chauffe pour le type de jour
		$SQL = "SELECT `id` 	FROM `calendrier_30min` 
				WHERE `date` = (SELECT `date` FROM `calendrier` WHERE `id` = $idjour) 
				AND `heure_debut` >= '$hdebut'
				AND `heure_fin` <= '$hfin';";
		//on envoie la requete
		$RESULT2 = @mysqli_query($link, $SQL);
		
		//pour chaque id des creneaux de chauffe, on met à jour la température de consigne
		while($myrow2 = @mysqli_fetch_array($RESULT2)) {	
			$idcellule = $myrow2["id"];
			echo "idcellule = $idcellule<br>";
			$SQL = "UPDATE `domotique`.`calendrier_30min` SET `temperature` = $temp_consigne WHERE `calendrier_30min`.`id` = $idcellule;";
			mysqli_query($link,$SQL);
			//libération de la variable
		}
		mysqli_free_result($RESULT2) ;
		//libération de la variable
	}
	mysqli_free_result($RESULT) ;
	//fermeture session MySQL
	mysqli_close($link);
?>