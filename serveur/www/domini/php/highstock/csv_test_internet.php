<?php 
	include("../../infos/config.inc.php"); // on inclu le fichier de config

	$filename = "../../csv/test_internet.csv";

	@	$link = mysqli_connect($host,$login,$passe,$bdd);
	if (!$link) {
		die('Erreur de connexion (' . mysqli_connect_errno() . ') '
				. mysqli_connect_error());
	}
	

	//requete pour recuperer les infos du mois en cours
	// $SQL="SELECT DATE_FORMAT(date_time, '%d-%m-%Y') AS DATE, DATE_FORMAT(date_time, '%H:%i:%s') AS Heure, ping
		// FROM `internet_connex`
		// WHERE date_time <=  NOW( ) AND date_time >= SUBDATE(NOW(),60) 
		// ORDER BY date_time"; 

	$SQL="SELECT DATE_FORMAT(date_time, '%d-%m-%Y') AS DATE, DATE_FORMAT(date_time, '%H:%i:%s') AS Heure, ping
		FROM `internet_connex`
		ORDER BY date_time"; 
		
	// on execute la requete
	$RESULT = @mysqli_query($link, $SQL);

	// on r�cup�re le resultat de la requete
	$row = mysqli_fetch_assoc($RESULT);
	$line = "";
	$comma = "";
	$fp = fopen($filename, "w");

	//pour la premi�re ligne, on �crit le nom des entetes
	foreach($row as $name => $value) {
		$line .= $comma .  str_replace('"', '""', $name) ;
		$comma = ";";
	}
	$line .= "\n";
	fputs($fp, $line);

	// on se replace au d�but du r�sultat de la requete
	mysqli_data_seek($RESULT, 0);

	// tant qu'il y a des donn�es
	while($row = mysqli_fetch_assoc($RESULT)) {
	   
		$line = "";
		$comma = "";
		// on �crit chaque champs s�par� d'une virgule dans une variable
		foreach($row as $value) {
			$line .= $comma . str_replace('"', '""', $value);
			$comma = ";";
		}
		// on saute une ligne dans le fichier
		$line .= "\n";
		echo $line.'</br>';
		// on �crit la ligne
		fputs($fp, $line);
		
	}

	//fermeture du fichier
	fclose($fp);
	// on lib�re la m�moire
	mysqli_free_result($RESULT) ;
	// on ferme la connexion � mysql
	mysqli_close($link);	 
	 
echo"CSV test_internet export�.<br>";

?>