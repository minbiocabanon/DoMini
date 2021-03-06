<?php 
	include("../../infos/config.inc.php"); // on inclu le fichier de config

	$filename = "../../csv/temp_stat.csv";

	//on r�cup�re l'ann�e en cours
	$annee = date('Y');
	
	$line = "";
	$comma = "";
	$fp = fopen($filename, "w");
	
		$link = mysqli_connect($host,$login,$passe,$bdd);
	if (!$link) {
		die('Erreur de connexion (' . mysqli_connect_errno() . ') '
				. mysqli_connect_error());
	}
	
	
	for($i = 2011; $i <= $annee ; $i++){
		//requete pour r�cup�rer la temp�rature min/max/moy par mois et pour chaque ann�e
		$SQL="SELECT sum( `ana1` ) 
				FROM `pyranometre` 
				WHERE year( `date_time` ) = $i
				GROUP BY month( `date_time` ) 
				ORDER BY month( `date_time` ) ASC
				LIMIT 0 , 30"; 
		
		// on execute la requete
		$RESULT = @mysqli_query($link, $SQL);	
		$line = "$i;";
		while($row = mysqli_fetch_assoc($RESULT)) {
			
			foreach($row as $value) {
				$line .= $comma .  str_replace('"', '""', $value) ;
				$comma = ";";
			}			
		}
		$line .= "\n";
		fputs($fp, $line);
		echo "$line </br>";
		$line = "";
		$comma = "";
	}

	//fermeture du fichier
	fclose($fp);
	// on lib�re la m�moire
	mysqli_free_result($RESULT) ;
	// on ferme la connexion � mysql
	mysqli_close($link);	 
	 
echo"CSV pyranom�tre mensuel export�.<br>";

?>