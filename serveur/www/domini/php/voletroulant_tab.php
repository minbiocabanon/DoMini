<?php	
	// Connexion à la BDD pour récupérer l'état des volets
	@mysql_connect($host,$login,$passe) or die("Impossible de se connecter à la base de données");
	@mysql_select_db("$bdd") or die("Impossible de se connecter à la base de données");

	// requete MySQL pour stocker les nouvelles valeurs
	$SQL="SELECT * 
		FROM `voletroulant_statut`"; 

	$RESULT = @mysql_query($SQL);

	// on récupère l'état et le mode des VR
	if ($myrow=@mysql_fetch_array($RESULT)) {
	  do {
			// on converti le mode (numéro) en chaine interpétable pour la liste de choix HTML
			$mode_vr = reverse_convert_mode($myrow["mode"]);
			$etat_vr_bureau = reverse_convert_etat($myrow["bureau"]);
			$etat_vr_salon = reverse_convert_etat($myrow["salon"]);
			$etat_vr_chm = reverse_convert_etat($myrow["chambreM"]);
			$etat_vr_chjf = reverse_convert_etat($myrow["chambreJF"]);
	  }while ($myrow=@mysql_fetch_array($RESULT));
	}
		
	//Execution de la requete
	mysql_query($SQL) or die('Erreur SQL !'.$SQL.'<br>'.mysql_error());
	mysql_close();
	
	// echo "mode_vr = $mode_vr <br>";
	// echo "etat_vr_bureau = $etat_vr_bureau <br>";
	// echo "etat_vr_salon = $etat_vr_salon <br>";
	// echo "etat_vr_chm = $etat_vr_chm <br>";
	// echo "etat_vr_chjf = $etat_vr_chjf <br>";
	
	if($mode_vr == "automatique"){
		$bouton_actif = "disabled";
	}
	else{
			$bouton_actif = "";
	}
?>
