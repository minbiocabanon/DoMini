<?php // content="text/plain; charset=utf-8"
	include("../../infos/config.inc.php"); // on inclu le fichier de config
	$filename = "../../csv/pellet_rsv.csv";

	//on prépare l'entete du fichier CSV
	$fp = fopen($filename, "w");
	$line = "DATE;heure;credit;débit;capital\n";
	fputs($fp, $line);
	// requete MySQL pour obtenir les données de la BDD
	//echo" $host, $login, $passe, $bdd \n";

	@mysql_connect($host,$login,$passe) or die("Impossible de se connecter à la base de données");
	@mysql_select_db("$bdd") or die("Impossible de se connecter à la base de données");
	$SQL="SET lc_time_names = 'fr_FR'" ; // Pour afficher date en français dans MySql. 
	mysql_query($SQL) ;
	
	//on récupére l'année en cours
	$annee = date('Y');
	//on récupére le mois en cours (0-12)
	$mois = date('m');
	$n=0;

	// requete MySQL pour obtenir les données de pellets pour les 12 derniers mois
	// pour les 12 derniers mois

	$SQL="SELECT DATE_FORMAT(date_time, '%d-%m-%Y') AS DATE, DATE_FORMAT(date_time, '%H:%i:%s') AS Heure, nvg
			FROM  `pellets_rsv` 
			WHERE date_time
			BETWEEN DATE_SUB( NOW( ) , INTERVAL 1800 DAY ) 
			AND NOW( )
			ORDER BY date_time ASC"; 

	$RESULT = @mysql_query($SQL);
		
	// fetch a row and write the column names out to the file
	$row = mysql_fetch_assoc($RESULT);
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
	mysql_data_seek($RESULT, 0);

	// and loop through the actual data
	while($row = mysql_fetch_assoc($RESULT)) {
	   
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
	
	echo"CSV pellet exporté.<br>";
	mysql_free_result($RESULT) ;
	mysql_close();
?>