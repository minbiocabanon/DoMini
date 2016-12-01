<?php
	include("../infos/config.inc.php"); // on inclu le fichier de config

	$etat_chauffage_status = isset($_GET['etat_chauffage_status']) ? $_GET['etat_chauffage_status'] : '';
	$puissance = isset($_GET['puissance']) ? $_GET['puissance'] : '';

	// echo"etat_chauffage_status = $etat_chauffage_status </br>";
	// echo"puissance = $puissance </br>";

	//On regarde le chauffage
	//on vérifie qu'au moins un des cas possible existe		
	if($etat_chauffage_status == 1 || $etat_chauffage_status == 2 || $etat_chauffage_status == 3){
		// echo "etat_chauffage_status =  $etat_chauffage_status<br>";
		// echo"	ETAT_ERREUR, 	// 0<br>    ETAT_OFF, 		// 1<br>	ETAT_ON, 		// 2<br>	ETAT_AUTO		// 3";

		// Connexion à la BDD
		$link = mysqli_connect($host,$login,$passe,$bdd);
		if (!$link) {
			die('Erreur de connexion (' . mysqli_connect_errno() . ') '
					. mysqli_connect_error());
		}

		//si la puissance est fixée pour l'utilisateur
		if($etat_chauffage_status == 2 && $puissance != ""){
			echo "Chauffage ET puissance : $puissance<br>";
			//on met à jour la base avec le mode et la puissance
			$SQL="UPDATE `domotique`.`chauffage_statut` SET `etat` = '$etat_chauffage_status', `consigne_utilisateur` = '$puissance' WHERE `chauffage_statut`.`id` =1;"; 
			// on execute la requete
			$RESULT = @mysqli_query($link,$SQL);
		}

		else{
			// echo "chauffage seul<br>";
			//on met à jour la base avec le mode 
			$SQL="UPDATE `domotique`.`chauffage_statut` SET `etat` = '$etat_chauffage_status' WHERE `chauffage_statut`.`id` =1;"; 
			// on execute la requete
			$RESULT = @mysqli_query($link$SQL);
		}
		
		//Execution de la requete
		mysqli_query($link,$SQL) or die('Erreur SQL !'.$SQL.'<br>'.mysqli_error());
		mysqli_close($link);
		
		include('../php/restore_donnees_instant.php');
		
		// echo "Chauffage mis à jour<br>";
	}
?>

						<!-- Incrustation d'un boite de confirmation, possibilité de la ferme avec une croix -->
						<div class="alert alert-success alert-dismissable">
						  <button type="button" class="close" data-dismiss="alert" aria-hidden="true">&times;</button>
						  <strong>OK!</strong> Etat mis à jour.
						</div>