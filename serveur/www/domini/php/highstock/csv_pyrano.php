<?php 
	include("../../infos/config.inc.php"); // on inclu le fichier de config
	include("../../infos/pyrano.inc.php"); // on inclu le fichier pour avoir les variables/constantes du pyranomètre

	$filename = "../../csv/pyrano.csv";

	@mysql_connect($host,$login,$passe) or die("Impossible de se connecter à la base de données");
	@mysql_select_db("$bdd") or die("Impossible de se connecter à la base de données");

	//requete pour récupérer les infos du mois en cours
	$SQL="SELECT DATE_FORMAT(date_time, '%d-%m-%Y') AS DATE, DATE_FORMAT(date_time, '%H:%i:%s') AS Heure, ana1 * $Coef_V_to_W
		FROM pyranometre
		WHERE date_time <=  NOW( ) AND date_time >= SUBDATE(NOW(),40) 
		ORDER BY date_time"; 

	// on execute la requete
	$RESULT = @mysql_query($SQL);

	// on récupère le resultat de la requete
	$row = mysql_fetch_assoc($RESULT);
	$line = "";
	$comma = "";
	$fp = fopen($filename, "w");

	//pour la première ligne, on écrit le nom des entetes
	foreach($row as $name => $value) {
		$line .= $comma .  str_replace('"', '""', $name) ;
		$comma = ";";
	}
	$line .= "\n";
	fputs($fp, $line);

	// on se replace au début du résultat de la requete
	mysql_data_seek($RESULT, 0);

	// tant qu'il y a des données
	while($row = mysql_fetch_assoc($RESULT)) {
	   
		$line = "";
		$comma = "";
		// on écrit chaque champs séparé d'une virgule dans une variable
		foreach($row as $value) {
			$line .= $comma . str_replace('"', '""', $value);
			$comma = ";";
		}
		// on saute une ligne dans le fichier
		$line .= "\n";
		// on écrit la ligne
		fputs($fp, $line);
	}

	//fermeture du fichier
	fclose($fp);
	// on libère la mémoire
	mysql_free_result($RESULT) ;
	// on ferme la connexion à mysql
	mysql_close();	 
	 
echo"CSV pyranomètre exporté.<br>";

?>