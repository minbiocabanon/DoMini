<?php // content="text/plain; charset=utf-8"
	include("../../infos/config.inc.php"); // on inclu le fichier de config
	$filename = "../../csv/puissance_pc.csv";
	
	//on prépare l'entete du fichier CSV	
	$fp = fopen($filename, "w");

	
	// requete MySQL pour obtenir les données de la BDD
	//echo" $host, $login, $passe, $bdd \n";
		$link = mysqli_connect($host,$login,$passe,$bdd);
	if (!$link) {
		die('Erreur de connexion (' . mysqli_connect_errno() . ') '
				. mysqli_connect_error());
	}
	

	// ------------------- Données Température ext. -------------------------------------
	//$SQL="SELECT TIME(date_time) AS Heure, puissance FROM  teleinfo WHERE  DAY(date_time)=$jour AND YEAR(date_time)=$annee AND MONTH(date_time)=$mois ORDER BY date_time"; 
	$SQL="SELECT DATE_FORMAT(date_time, '%d-%m-%Y') AS DATE, DATE_FORMAT(date_time, '%H:%i:%s') AS Heure, puissance, puissance_simple
		FROM puissance_pc
		WHERE date_time <= NOW( ) 
		AND date_time <=  NOW( ) AND date_time >= SUBDATE(NOW(),40) 
		ORDER BY date_time"; 

	// on lance la requête
	$RESULT = @mysqli_query($link, $SQL);
	// fetch a row and write the column names out to the file
	$row = mysqli_fetch_assoc($RESULT);
	$line = "";
	$comma = "";
	$fp = fopen($filename, "w");

	if (is_array($row)){
		foreach($row as $name => $value) {
			$line .= $comma .  str_replace('"', '""', $name) ;
			$comma = ";";
		}
	}
	$line .= "\n";
	fputs($fp, $line);

	// remove the result pointer back to the start
	mysqli_data_seek($RESULT, 0);

	// and loop through the actual data
	while($row = mysqli_fetch_assoc($RESULT)) {
	   
		$line = "";
		$comma = "";
		foreach($row as $value) {
			$line .= $comma . str_replace('"', '""', $value);
			$comma = ";";
		}
		$line .= "\n";
		fputs($fp, $line);
	}
	
	//on ferme le fichier
	fclose($fp);		 
	echo"CSV puissance pc exporté.<br>";

	//on quite la session mysql
	mysqli_free_result($RESULT) ;
	mysqli_close($link);
?>