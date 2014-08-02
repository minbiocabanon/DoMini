<?php
	include("../infos/config.inc.php"); // on inclu le fichier de config 
	
	$IdCell = isset($_GET['IdCell']) ? $_GET['IdCell'] : '';
	//$IdCell=$_GET["IdCell"];
	//echo "$IdCell";
				
	@mysql_connect($host,$login,$passe) or die("Impossible de se connecter à la base de données");
	@mysql_select_db("$bdd") or die("Impossible de se connecter à la base de données");
	//on va récupérer la consigne de la cellule cliquée
	$SQL="SELECT `temperature` FROM `calendrier_30min` WHERE `calendrier_30min`.`id` = $IdCell ;";
	//on envoie la requete
	$RESULT = @mysql_query($SQL);
	//on récupère le résultat
	$myrow=@mysql_fetch_array($RESULT); 
	// on extrait le résultat et on le stocke dans une variable
	$temp_consigne_calendrier = $myrow["temperature"];
	//echo "$temp_consigne_calendrier";
	//libération de la variable
	mysql_free_result($RESULT) ;
	//fermeture session MySQL
	mysql_close();
	
	//si la consigne est vide (0.0) alors il faut appliquer la consigne de la saison`
	if($temp_consigne_calendrier == "0.0"){
		mysql_connect($host,$login,$passe) or die("Impossible de se connecter à la base de données");
		@mysql_select_db("$bdd") or die("Impossible de se connecter à la base de données");
		//requete pour récupérer la température de consigne de saison
		$SQL="	SELECT `consigne_temperature`
				FROM `calendrier_saison` 
				WHERE `type` = ( 
					SELECT `saison` 
					FROM `calendrier` 
					WHERE `id` = (
						SELECT DAYOFYEAR(`date`) 
						FROM `calendrier_30min` 
						WHERE `id`= $IdCell) ) 
				LIMIT 0 , 1"; 
		
		//on envoie la requete
		$RESULT = @mysql_query($SQL);
		//on récupère le résultat
		$myrow=@mysql_fetch_array($RESULT); 
		// on extrait le résultat et on le stocke dans une variable
		$temp_consigne = $myrow["consigne_temperature"];
		//libération de la variable
		mysql_free_result($RESULT) ;
		//fermeture session MySQL
		mysql_close();
		echo "$temp_consigne";
	}
	// sinon, la consigne de chauffe est à enlever, on écrit 0.0 dans la table MySQL et on envoie du vide dans la page html
	else{
		$temp_consigne = "0.0";
		echo " ";
	}
	
	@mysql_connect($host,$login,$passe) or die("Impossible de se connecter à la base de données");
	@mysql_select_db("$bdd") or die("Impossible de se connecter à la base de données");
	$SQL="UPDATE `domotique`.`calendrier_30min` SET `temperature` = '$temp_consigne' WHERE `calendrier_30min`.`id` = $IdCell;";
	//on envoie la requete
	mysql_query($SQL) or die('Erreur SQL !'.$SQL.'<br>'.mysql_error());
	//fermeture session MySQL
	mysql_close();
?>