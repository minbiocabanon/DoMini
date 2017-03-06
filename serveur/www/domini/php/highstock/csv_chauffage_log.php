<?php // content="text/plain; charset=utf-8"
	include("../../infos/config.inc.php"); // on inclu le fichier de config
	$filename = "../../csv/chauffage_log.csv";

		$link = mysqli_connect($host,$login,$passe,$bdd);
	if (!$link) {
		die('Erreur de connexion (' . mysqli_connect_errno() . ') '
				. mysqli_connect_error());
	}
	

	// on compte le nombre de ligne pour les données des relevés intérieurs
	// on prépare la requête
	// on crée une table temporaire avec tous les champs nécessaires pour extraire les tableaux pour JPGRAPH
	$SQL="SELECT DATE_FORMAT(date_time, '%d-%m-%Y') AS DATE, DATE_FORMAT(date_time, '%H:%i:%s') AS Heure,temp_int, temp_consigne, puissance, mode
	FROM chauffage_log
	ORDER BY date_time"; 

	// on execute la requete
	$RESULT = @mysqli_query($link, $SQL);
	$n=0;

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

	fclose($fp); 
		 
	echo"CSV chauffage log exporté.<br>";

	//on quite la session mysql
	mysqli_free_result($RESULT) ;
	mysqli_close($link);

?>