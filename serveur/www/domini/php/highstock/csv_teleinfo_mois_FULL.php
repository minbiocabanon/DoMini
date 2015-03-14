<?php // content="text/plain; charset=utf-8"

// ---------------------------------
//   !!!!! ATTENTION !!!!
// ---------------------------------
//
// CE SCRIPT NE FAIT QU'AJOUTER UNE LIGNE A LA FIN DU FICHIER EXISTANT
// AFIN DE NE PAS RELANCER LES REQUETES DEPUIS LA CREATION DES BASES -> TRES LONG !!!

	include("../../infos/config.inc.php"); // on inclu le fichier de config
	include("../../infos/tarifedf.inc.php"); // on inclu le fichier avec les tarifs edf
	$filename = "../../csv/teleinfo_mois.csv";

	//on prépare l'entete du fichier CSV	
	// on ouvre le fichier en écriture seule et on place le pointeur à la fin du fichier -> option "a"
	$fp = fopen($filename, "w");
	$line = "DATE;heure;consoHP;consoHC\n";
	fputs($fp, $line);

	@mysql_connect($host,$login,$passe) or die("Impossible de se connecter à la base de données");
	@mysql_select_db("$bdd") or die("Impossible de se connecter à la base de données");
	$SQL="SET lc_time_names = 'fr_FR'" ; // Pour afficher date en français dans MySql. 
	mysql_query($SQL) ;

	//on récupére l'année en cours
	$annee_crte = date('Y');
	//on récupére le mois en cours (1-12)
	$mois_crt = date('m');	
	
	//Pour chaque année depuis 2010 jusqu'à aujourd'hui
	for($annee = 2010; $annee <= $annee_crte; $annee++){
	
		//pour l'année en cours, on s'arrête au mois en cours
		if($annee == $annee_crte)
			//on récupére le mois en cours (1-12)
			$mois = date('m');
		else
			// sinon on fait pour les 12 mois
			$mois = 12;	
			
		// pour tous les mois
		for($m=1 ; $m <= $mois ; $m++){
			// requete MySQL pour obtenir les HP et HC du mois
			$SQL="SELECT DATE_FORMAT(date_time, '%d-%m-%Y') AS DATE, HC, HP
					FROM  `teleinfo` 
					WHERE MONTH( date_time ) = $m AND YEAR( date_time ) = $annee
					ORDER BY date_time ASC
					LIMIT 0 , 1";
			// echo "<br>requete SQL : $SQL<br>";
			$RESULT = @mysql_query($SQL);
			
			//on ne prend que le premier élement de la table HC et HP
			if($myrow = @mysql_fetch_assoc($RESULT)){
			
				// echo "<br>resultat : $myrow[1]";
				$data_date = $myrow["DATE"];
				$data_HC_deb = $myrow["HC"] / 1000; // on divise par mille pour avoir des kWh
				$data_HP_deb = $myrow["HP"] / 1000; // on divise par mille pour avoir des kWh
				// echo "$data_date - $data_HC_deb , $data_HP_deb<br>";

				// requete MySQL pour obtenir les HP et HC du mois
				$SQL="SELECT DATE_FORMAT(date_time, '%d-%m-%Y') AS DATE, HC, HP
						FROM  `teleinfo` 
						WHERE MONTH( date_time ) = $m AND YEAR( date_time ) = $annee
						ORDER BY date_time DESC
						LIMIT 0 , 1";
				// echo "<br>requete SQL : $SQL<br>";
				$RESULT = @mysql_query($SQL);
				
				//on lit la seconde ligne avec les HP+HC de fin d'année
				if($myrow=@mysql_fetch_assoc($RESULT)){
					
					$data_HC_fin = $myrow["HC"] / 1000; // on divise par mille pour avoir des kWh
					$data_HP_fin = $myrow["HP"] / 1000; // on divise par mille pour avoir des kWh
					// echo "$data_date - $data_HC_fin , $data_HP_fin <br>";

					// On calcul la conso mensuelle
					$data_consomoisHC = $data_HC_fin - $data_HC_deb ;
					$data_consomoisHP = $data_HP_fin - $data_HP_deb ;
					
					// On converti en euros
					$data_consomoisHC = round($tarif_HC * $data_consomoisHC , 2);
					$data_consomoisHP = round($tarif_HP * $data_consomoisHP , 2);

					//on écrit les données dans le fichier CSV		
					echo "$data_date - $data_consomoisHP , $data_consomoisHC <br>";
					$line = $data_date .";00:00:01;". $data_consomoisHP .";". $data_consomoisHC."\n";
					fputs($fp, $line);
				}
				$myrow=0;
			}
		}	
	}
	
	fclose($fp);	
	mysql_free_result($RESULT) ;
	mysql_close();

	echo"CSV teleinfo_mois_FULL exporté.<br>";
?>