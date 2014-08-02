<?php 
	include("../../infos/config.inc.php"); // on inclu le fichier de config

	$filename = "../../csv/temp_stat.csv";

	//on récupére l'année en cours
	$annee = date('Y');
	
	$line = "";
	$comma = "";
	$fp = fopen($filename, "w");
	
	@mysql_connect($host,$login,$passe) or die("Impossible de se connecter à la base de données");
	@mysql_select_db("$bdd") or die("Impossible de se connecter à la base de données");
	
	for($i = 2011; $i <= $annee ; $i++){
		//requete pour récupérer la température min/max/moy par mois et pour chaque année
		$SQL="SELECT sum( `ana1` ) 
				FROM `pyranometre` 
				WHERE year( `date_time` ) = $i
				GROUP BY month( `date_time` ) 
				ORDER BY month( `date_time` ) ASC
				LIMIT 0 , 30"; 
		
		// on execute la requete
		$RESULT = @mysql_query($SQL);	
		$line = "$i;";
		while($row = mysql_fetch_assoc($RESULT)) {
			
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
	// on libère la mémoire
	mysql_free_result($RESULT) ;
	// on ferme la connexion à mysql
	mysql_close();	 
	 
echo"CSV pyranomètre mensuel exporté.<br>";

?>