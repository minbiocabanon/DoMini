<?php // content="text/plain; charset=utf-8"
	include("../../infos/config.inc.php"); // on inclu le fichier de config
	$filename = "../../csv/pellet_conso.csv";

	//on prépare l'entete du fichier CSV
	$fp = fopen($filename, "w");
	$line = "DATE;heure;nbsac\n";
	fputs($fp, $line);
	// requete MySQL pour obtenir les données de la BDD
	//echo" $host, $login, $passe, $bdd \n";

		$link = mysqli_connect($host,$login,$passe,$bdd);
	if (!$link) {
		die('Erreur de connexion (' . mysqli_connect_errno() . ') '
				. mysqli_connect_error());
	}
	
	$SQL="SET lc_time_names = 'fr_FR'" ; // Pour afficher date en français dans MySql. 
	mysqli_query($link, $SQL) ;
	
	//on récupére l'année en cours
	$annee = date('Y');
	//on récupére le mois en cours (0-12)
	$mois = date('m');
	$n=0;

	// requete MySQL pour obtenir les données de pellets pour les 12 derniers mois
	// pour les 12 derniers mois

	$SQL="SELECT DATE_FORMAT(date_time, '%d-%m-%Y') AS DATE, DATE_FORMAT(date_time, '%H:%i:%s') AS Heure, SUM(`debit`) as Conso 
			FROM `pellets`
			GROUP BY YEAR(`date_time`)
			ORDER BY YEAR(`date_time`)"; 

	$RESULT = @mysqli_query($link, $SQL);
		
	// fetch a row and write the column names out to the file
	$row = mysqli_fetch_assoc($RESULT);
	$line = "";
	$comma = "";
	$fp = fopen($filename, "w");

	foreach($row as $name => $value) {
		$line .= $comma .  str_replace('"', '""', $name) ;
		$comma = ";";
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
	
	echo"CSV pellet conso exporté.<br>";
	mysqli_free_result($RESULT) ;
	mysqli_close($link);
?>