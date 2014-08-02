<?php
	@include("../infos/config.inc.php"); // on inclu le fichier de config 
	
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
	
	echo "$temp_consigne_calendrier";
?>