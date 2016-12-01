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
	

	//on met � jour le type de jour dans la table calendrier_saison`
	$SQL = "UPDATE `domotique`.`calendrier` SET `type_jour` = '$typejour' WHERE `calendrier`.`id` = $idjour;";
	mysqli_query($link,$SQL);
	//fermeture session MySQL
	mysqli_close($link);
	echo "type jour mis � jour dans la table calendrier <br>";
				
				$link = mysqli_connect($host,$login,$passe,$bdd);
			if (!$link) {
				die('Erreur de connexion (' . mysqli_connect_errno() . ') '
						. mysqli_connect_error());
			}
	
	//requete pour r�cup�rer la temp�rature de consigne de saison
	$SQL="	SELECT `consigne_temperature`
			FROM `calendrier_saison` 
			WHERE `type` = ( 
				SELECT `saison` 
				FROM `calendrier` 
				WHERE `id` = $idjour ) 
			LIMIT 0 , 1"; 
	
	//on envoie la requete
	$RESULT = @mysqli_query($link, $SQL);
	//on r�cup�re le r�sultat
	$myrow=@mysqli_fetch_array($RESULT); 
	// on extrait le r�sultat et on le stocke dans une variable
	$temp_consigne = $myrow["consigne_temperature"];
	//lib�ration de la variable
	mysqli_free_result($RESULT) ;
	//fermeture session MySQL
	mysqli_close($link);
	echo "temp_consigne = $temp_consigne<br>";
	
	// Maintenant, on va traiter chaque cr�neau de chauffe du type de jour , puis remplir la table planning en cons�quence (temp�rature de consigne � mettre � jour dans les cr�neau concern�s)
	
	//on commence par mettre � 0 toutes les cellules
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
	// on ne r�cup�re que les infos du jour en cours
	//sprintf(query, "SELECT * FROM `calendrier_type_jour` WHERE `type_jour` = '%s'", stJourTraite.inf_type_jour);
				$link = mysqli_connect($host,$login,$passe,$bdd);
			if (!$link) {
				die('Erreur de connexion (' . mysqli_connect_errno() . ') '
						. mysqli_connect_error());
			}
	
	//requete pour r�cup�rer les heures de debut et fin des creneaux de chauffe
	$SQL="	SELECT `heure_debut`, `heure_fin` FROM `calendrier_type_jour` WHERE `type_jour` = \"$typejour\"";
	//on envoie la requete
	$RESULT = @mysqli_query($link, $SQL);
	// pour chq cr�neau de chauffe existant pour ce type de jour
	while($myrow = @mysqli_fetch_array($RESULT)) {
		// on recup�re l'heure de d�but
		$hdebut = $myrow["heure_debut"];
		// on recup�re l'heure de fin
		$hfin = $myrow["heure_fin"];
		echo "hdebut = $hdebut - hfin = $hfin<br>";
		// on r�cup�re les id des creneaux de chauffe pour le type de jour
		$SQL = "SELECT `id` 	FROM `calendrier_30min` 
				WHERE `date` = (SELECT `date` FROM `calendrier` WHERE `id` = $idjour) 
				AND `heure_debut` >= '$hdebut'
				AND `heure_fin` <= '$hfin';";
		//on envoie la requete
		$RESULT2 = @mysqli_query($link, $SQL);
		
		//pour chaque id des creneaux de chauffe, on met � jour la temp�rature de consigne
		while($myrow2 = @mysqli_fetch_array($RESULT2)) {	
			$idcellule = $myrow2["id"];
			echo "idcellule = $idcellule<br>";
			$SQL = "UPDATE `domotique`.`calendrier_30min` SET `temperature` = $temp_consigne WHERE `calendrier_30min`.`id` = $idcellule;";
			mysqli_query($link,$SQL);
			//lib�ration de la variable
		}
		mysqli_free_result($RESULT2) ;
		//lib�ration de la variable
	}
	mysqli_free_result($RESULT) ;
	//fermeture session MySQL
	mysqli_close($link);
?>