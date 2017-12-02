<?php // content="text/plain; charset=utf-8"
	include("../../infos/config.inc.php"); // on inclu le fichier de config
	$filename = "../../csv/pellet_bilan_hiver.csv";

	//on prépare l'entete du fichier CSV
	$fp = fopen($filename, "w");
	$line="DATE;Conso\n";
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

	// CAS PARTICULIER DE LA PREMIERE ANNEE:
	$annee_courante = 2009;
	$annee_suivante = $annee_courante + 1;
					
	$SQL="SELECT SUM(`debit`) AS CUMUL 
			FROM `pellets` 
			WHERE date_time between '$annee_courante-06-01' and '$annee_suivante-05-31'"; 
	echo "$SQL<br>";
	$RESULT = mysqli_query($link, $SQL);
	$debit_annee = mysqli_fetch_assoc($RESULT);
	mysqli_free_result($RESULT);	
	$cumul = $debit_annee["CUMUL"];
	$line = "$annee_courante;$cumul\n";
	echo"$line<br>";
	fputs($fp, $line);

	// POUR LES AUTRES ANNEES
	// requete MySQL pour obtenir un tableau des années pour la prochaine requete
	$SQL="SELECT YEAR(date_time) 
			FROM `pellets` 
			GROUP BY YEAR(`date_time`) 
			ORDER BY YEAR(`date_time`)"; 

	$RESULT = mysqli_query($link, $SQL);
	

	
	 // pour chaque année, on compte le nb de sac entre le 01 juin de l'année et le 31 mai de l'année suivante
	while ($row = mysqli_fetch_assoc($RESULT)) {
		
		foreach($row as $annee_courante) {
		
			$annee_suivante = $annee_courante + 1;
							
			$SQL="SELECT SUM(`debit`) AS CUMUL 
					FROM `pellets` 
					WHERE date_time between '$annee_courante-06-01' and '$annee_suivante-05-31'"; 
			echo "$SQL<br>";
			$RESULT1 = mysqli_query($link, $SQL);
			$debit_annee = mysqli_fetch_assoc($RESULT1);	
			$cumul = $debit_annee["CUMUL"];
			$line = "$annee_courante;$cumul\n";
			echo"$line<br>";
			fputs($fp, $line);
		}
		mysqli_free_result($RESULT1);
	}

	fclose($fp);		 
	
	echo"CSV pellet conso exporté.<br>";
	mysqli_free_result($RESULT);
	mysqli_close($link);
?>