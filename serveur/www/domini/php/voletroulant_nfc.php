<?php	
	include("../infos/config.inc.php"); // on inclu le fichier de config 
	
	$etat = isset($_GET['etat']) ? $_GET['etat'] : '';
	echo"etat = $etat </br>";
	$update_bdd = false;
	
	//si le tag NFC envoie le passage en mode Manuel , on vérifie si il fait jour ou nuit pour positionner les volets dans la position adequat
	if( $etat == "manuel"){
		// Connexion à la BDD pour récupérer l'état des volets
		@mysql_connect($host,$login,$passe) or die("Impossible de se connecter à la base de données");
		@mysql_select_db("$bdd") or die("Impossible de se connecter à la base de données");

		//heure courante
		$unixtime = time();
		$unixtime_h = date("d-m-Y H:i");
		echo"<br/>Heure courante = 	$unixtime - $unixtime_h<br>";
		
		// heures de levés et couchés de soleil. requete depuis la bdd
		
		$SQL="
		SELECT 	UNIX_TIMESTAMP(addtime(DATE_FORMAT(MAKEDATE(YEAR(NOW()),DAYOFYEAR(NOW())),'%Y-%m-%d %H:%i:%s') , `lever`)) AS hlever_unix, 
					addtime(DATE_FORMAT(MAKEDATE(YEAR(NOW()),DAYOFYEAR(NOW())),'%Y-%m-%d %H:%i:%s') , `lever`) AS hlever,
				UNIX_TIMESTAMP(addtime(DATE_FORMAT(MAKEDATE(YEAR(NOW()),DAYOFYEAR(NOW())),'%Y-%m-%d %H:%i:%s') , `coucher`)) AS hcoucher_unix, 
					addtime(DATE_FORMAT(MAKEDATE(YEAR(NOW()),DAYOFYEAR(NOW())),'%Y-%m-%d %H:%i:%s') , `coucher`) AS hcoucher
		FROM `ephemerides` 
		WHERE DATE_FORMAT( `mois-jour` , '%%m-%%d' ) = DATE_FORMAT( NOW( ) , '%%m-%%d' ) 
		LIMIT 1";
		
		$RESULT = @mysql_query($SQL);
		$myrow=@mysql_fetch_array($RESULT); 
		mysql_close();
		//on récupère l'heure de levé de soleil
		$unixtime_leve = $myrow["hlever_unix"];
		$h_leve = $myrow["hlever"];
		echo"<br/>Heure de levé = 	$unixtime_leve - $h_leve<br>";
		//on récupère l'heure de couché de soleil, ajoute 1h pour profiter de lumière encore un peu
		$unixtime_couche = $myrow["hcoucher_unix"] + 3600;
		$h_couche = $myrow["hcoucher"];
		echo"<br/>Heure de couché = 	$unixtime_couche - $h_couche (+1h)<br>";

		//si il fait jour 
		if( $unixtime >= $unixtime_leve && $unixtime < $unixtime_couche){
			//Alors on ouvre les volets
			$mode = 3; // mode Tous Ouverts
			$etat_vr_bureau_bdd = 1;
			$etat_vr_salon_bdd = 1;
			$etat_vr_chm_bdd = 1;
			$etat_vr_chjf_bdd = 1;
			echo"<br/>Il fait jour, on met les volets ouvert , mode manuel<br>";
		}
		//sinon c'est qu'il fait nuit
		else{
			//on ferme les volets (ou on laisse en l'état?)
			$mode = 3; // mode Tous Fermés
			$etat_vr_bureau_bdd = 0;
			$etat_vr_salon_bdd = 0;
			$etat_vr_chm_bdd = 0;
			$etat_vr_chjf_bdd = 0;
			echo"<br/>Il fait noir, on met les volets fermes , mode manuel<br>";
		}
		$update_bdd = true;
	}
	// le tag NFC a envoyé la consigne 'automatique
	else if($etat == "auto"){
		// on met les volets en auto
		$mode = 0; // mode Automatique
		// je positionne les volets en immobile par défaut (pour éviter le warning dans la requete ci-aprés),
		// mais gestion_vr.c positionnera automatiquement les volets dans la bonne position (mode auto).
		$etat_vr_bureau_bdd = 3;
		$etat_vr_salon_bdd = 3;
		$etat_vr_chm_bdd = 3;
		$etat_vr_chjf_bdd = 3;
		echo"<br/>mode automatique<br>";
		$update_bdd = true;
	}
	else{
		echo"Pas de paramètre valide, on ne fait rien !";
	}

	if($update_bdd == true) {
		// applique les changements
		// Connexion à la BDD
		@mysql_connect($host,$login,$passe) or die("Impossible de se connecter à la base de données");
		@mysql_select_db("$bdd") or die("Impossible de se connecter à la base de données");

		// requete MySQL pour stocker les nouvelles valeurs
		$SQL="UPDATE `domotique`.`voletroulant_statut` SET `date_time` = NOW(),
				`mode` = '$mode',
				`bureau` = '$etat_vr_bureau_bdd',
				`salon` = '$etat_vr_salon_bdd',
				`chambreM` = '$etat_vr_chm_bdd',
				`chambreJF` = '$etat_vr_chjf_bdd'
				WHERE `voletroulant_statut`.`id` = 1;";
		//Execution de la requete
		mysql_query($SQL) or die('Erreur SQL !'.$SQL.'<br>'.mysql_error());
		mysql_close();
		echo"<br/>Base de données mise à jour.<br>";
		
		//on force le lancement de deux logiciels pour que l'action sur les VR soit immédiate
		// on lance le soft qui rempli la table d'état des VR
		exec("/var/www/domini/bin/gestion_vr",$result);
		// on force l'envoi des messages
		exec("/var/www/domini/bin/emitter /dev/ttyUSB0",$result);
		echo"<br/>Consignes envoyées aux volets<br>";
		
		//si le mode est Manuel
		if($mode == 3){
			//on met à jour la BDD	- on force l'état à IMMOBILE pour ne pas que les volets bougent alors qu'on est manuel		
			// Connexion à la BDD
			@mysql_connect($host,$login,$passe) or die("Impossible de se connecter à la base de données");
			@mysql_select_db("$bdd") or die("Impossible de se connecter à la base de données");

			// requete MySQL pour stocker les nouvelles valeurs
			$SQL="UPDATE `domotique`.`voletroulant_statut` SET `date_time` = NOW(),
					`mode` = '$mode',
					`bureau` = '3',
					`salon` = '3',
					`chambreM` = '3',
					`chambreJF` = '3'
					WHERE `voletroulant_statut`.`id` = 1;"; 

			//Execution de la requete
			mysql_query($SQL) or die('Erreur SQL !'.$SQL.'<br>'.mysql_error());
			mysql_close();	
			echo"<br/>Mode manuel, volets positionnés en Immobiles<br>";
		}
	}
?>
