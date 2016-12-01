<?php 
	include("../../infos/config.inc.php"); // on inclu le fichier de config
	include("../../infos/pyrano.inc.php"); // on inclu le fichier pour avoir les variables/constantes du pyranomètre

	$filename = "../../csv/pyrano.csv";

		$link = mysqli_connect($host,$login,$passe,$bdd);
	if (!$link) {
		die('Erreur de connexion (' . mysqli_connect_errno() . ') '
				. mysqli_connect_error());
	}
	

	//requete pour récupérer les infos du mois en cours
	$SQL="SELECT DATE_FORMAT(date_time, '%d-%m-%Y') AS DATE, DATE_FORMAT(date_time, '%H:%i:%s') AS Heure, ana1 * $Coef_V_to_W
		FROM pyranometre
		WHERE date_time <=  NOW( ) AND date_time >= SUBDATE(NOW(),40) 
		ORDER BY date_time"; 

	// on execute la requete
	$RESULT = @mysqli_query($link, $SQL);

	// on récupère le resultat de la requete
	$row = mysqli_fetch_assoc($RESULT);
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
	mysqli_data_seek($RESULT, 0);

	// tant qu'il y a des données
	while($row = mysqli_fetch_assoc($RESULT)) {
	   
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
	mysqli_free_result($RESULT) ;
	// on ferme la connexion à mysql
	mysqli_close($link);	 
	 
echo"CSV pyranomètre exporté.<br>";

?>