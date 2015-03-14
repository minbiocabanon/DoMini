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
	$fp = fopen($filename, "a");

	@mysql_connect($host,$login,$passe) or die("Impossible de se connecter à la base de données");
	@mysql_select_db("$bdd") or die("Impossible de se connecter à la base de données");
	$SQL="SET lc_time_names = 'fr_FR'" ; // Pour afficher date en français dans MySql. 
	mysql_query($SQL) ;

	//on récupère le numéro mois précédent
	$SQL="SELECT MONTH( SUBDATE( NOW( ) , INTERVAL 1 MONTH )) AS MOIS_PREC, YEAR(SUBDATE( NOW( ) , INTERVAL 1 MONTH )) AS ANNEE";
	$RESULT = @mysql_query($SQL);
	$myrow=@mysql_fetch_array($RESULT);
	$mois_prec[] = $myrow["MOIS_PREC"];
	$annee[] = $myrow["ANNEE"];
	
	echo"$mois_prec[0] <br/> $annee[0] <br/>";
	
	$n=0;
	
	// requete MySQL pour obtenir les HP et HC du début d'année (ou début de relevé de l'année)
	$SQL="SELECT DATE_FORMAT(date_time, '%d-%m-%Y') AS DATE, HC, HP
			FROM  `teleinfo` 
			WHERE MONTH( date_time ) = $mois_prec[0] AND YEAR( date_time ) = $annee[0]
			ORDER BY date_time
			LIMIT 0 , 1";
	// echo "<br>requete SQL : $SQL<br>";
	$RESULT = @mysql_query($SQL);
	//on ne prend que le premier élement de la table HC et HP
	if($myrow=@mysql_fetch_array($RESULT)){
	
		// echo "<br>resultat : $myrow[1]";
		$data_date[] = $myrow["DATE"];
		$data_HC_deb[0] = $myrow["HC"] / 1000; // on divise par mille pour avoir des kWh
		$data_HP_deb[0] = $myrow["HP"] / 1000; // on divise par mille pour avoir des kWh
		//echo "data_date : $data_date   <br> data_HC_deb : $data_HC_deb[0]     data_HP_deb : $data_HP_deb[0] <br>";

		// requete MySQL pour obtenir les HP et HC du début d'année (ou début de relevé de l'année)
		$SQL="SELECT DATE_FORMAT(date_time, '%d-%m-%Y') AS DATE, HC, HP
				FROM  `teleinfo` 
				WHERE MONTH( date_time ) = $mois_prec[0] AND YEAR( date_time ) = $annee[0]
				ORDER BY date_time DESC
				LIMIT 0 , 1";
		// echo "<br>requete SQL : $SQL<br>";
		$RESULT = @mysql_query($SQL);
		
		//on lit la seconde ligne avec les HP+HC de fin d'année
		if($myrow=@mysql_fetch_array($RESULT)){
			
			$data_HC_fin[0] = $myrow["HC"] / 1000; // on divise par mille pour avoir des kWh
			$data_HP_fin[0] = $myrow["HP"] / 1000; // on divise par mille pour avoir des kWh
			// echo "data_HC_fin : $data_HC_fin[0]     data_HP_fin : $data_HP_fin[0] <br>";

			// On calcul la conso mensuelle
			$data_consomoisHC[$n] = $data_HC_fin[0] - $data_HC_deb[0] ;
			$data_consomoisHP[$n] = $data_HP_fin[0] - $data_HP_deb[0] ;
			
			// On converti en euros
			$data_consomoisHC[$n] = round($tarif_HC * $data_consomoisHC[$n]  ,2);
			$data_consomoisHP[$n] = round($tarif_HP * $data_consomoisHP[$n] , 2);

			//on écrit les données dans le fichier CSV		
			$line = $data_date[$n] .";00:00:01;". $data_consomoisHP[$n] .";". $data_consomoisHC[$n]."\n"; 
			fputs($fp, $line);
		}
		
		$n++;
		$myrow=0;
	}	 
	
	echo" ligne ajoutée : $line <br>";
	
	mysql_free_result($RESULT) ;
	mysql_close();
	fclose($fp);
	echo"CSV teleinfo_mois_LIGHT exporté.<br>";
?>