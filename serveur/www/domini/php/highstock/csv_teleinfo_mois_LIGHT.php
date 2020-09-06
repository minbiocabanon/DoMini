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

		$link = mysqli_connect($host,$login,$passe,$bdd);
	if (!$link) {
		die('Erreur de connexion (' . mysqli_connect_errno() . ') '
				. mysqli_connect_error());
	}
	
	$SQL="SET lc_time_names = 'fr_FR'" ; // Pour afficher date en français dans MySql. 
	mysqli_query($link, $SQL) ;

	
	//on récupère le numéro mois précédent
	$SQL="SELECT MONTH( SUBDATE( NOW( ) , INTERVAL 1 MONTH )) AS MOIS_PREC, YEAR(SUBDATE( NOW( ) , INTERVAL 1 MONTH )) AS ANNEE";
	$RESULT = @mysqli_query($link, $SQL);
	$myrow=@mysqli_fetch_array($RESULT);
	$mois_prec[] = $myrow["MOIS_PREC"];
	$annee[] = $myrow["ANNEE"];
	$mois_prec[0] = 9;
	echo"mois_prec: $mois_prec[0] <br/> annee :$annee[0] <br/>";
	
	$n=0;
	
	// requete MySQL pour obtenir les HP et HC du début du mois précédent
	$SQL="SELECT DATE_FORMAT(date_time, '%d-%m-%Y') AS DATE, DATE_FORMAT(date_time, '%m-%Y') AS MOISANNEE,	HC, HP
			FROM  `teleinfo` 
			WHERE MONTH( date_time ) = $mois_prec[0] AND YEAR( date_time ) = $annee[0]
			ORDER BY date_time
			LIMIT 0 , 1";
	// echo "<br>requete SQL : $SQL<br>";
	$RESULT = @mysqli_query($link, $SQL);
	//on ne prend que le premier élement de la table HC et HP
	if($myrow=@mysqli_fetch_array($RESULT)){
	
		// echo "<br>resultat : $myrow[1]";
		$data_date[] = $myrow["DATE"];
		$data_moisannee = $myrow["MOISANNEE"];
		$data_HC_deb[0] = $myrow["HC"] / 1000; // on divise par mille pour avoir des kWh
		$data_HP_deb[0] = $myrow["HP"] / 1000; // on divise par mille pour avoir des kWh
		//echo "data_date : $data_date   <br> data_HC_deb : $data_HC_deb[0]     data_HP_deb : $data_HP_deb[0] <br>";

		// LINKY : Septembre 2020 , installation LINKY qui a remis à O le compteur HC/HP : ajout d'un offset avec les valeurs HC/HP du précédent compteur
		$SQL="SELECT DATE_FORMAT(date_time, '%m-%Y') AS DATE_LINKY FROM `linky_offset`";
		// echo "<br>requete SQL : $SQL<br>";
		$RESULT = @mysqli_query($link, $SQL);	
		// on recupére le resultat
		$myrow = @mysqli_fetch_array($RESULT);
		// on stocke la date de changement du linky
		$date_linky = $myrow["DATE_LINKY"];
		echo "<br>resultat : date_linky = $date_linky  , data_moisannee = $data_moisannee<br>";
		// // on clean et on ferme la BDD
		mysqli_free_result($RESULT) ;		
		
		//si on calcul la conso sur le mois pendant le lequel le linky a été posé, il faut ajouter récupérer l'offset pour l'ajouter au calcul
		if($data_moisannee == $date_linky){
			// requete MySQL pour obtenir les HP et HC avant la pose du linky
			$SQL="SELECT `linky_hc`,`linky_hp` FROM `linky_offset`";
			echo "<br>requete SQL : $SQL<br>";
			$RESULT = @mysqli_query($link, $SQL);	
			// on recupére le resultat
			$myrow = @mysqli_fetch_array($RESULT);
			// on stocke la quantité de HP et HC consommés
			$linky_offset_HC = $myrow["linky_hc"] ;
			$linky_offset_HP = $myrow["linky_hp"] ;
			echo "<br>resultat : linky_offset_HC = $linky_offset_HC  , linky_offset_HP = $linky_offset_HP<br>";
			// on clean et on ferme la BDD
			mysqli_free_result($RESULT) ;				
		}
		else{
			$linky_offset_HC = 0;
			$linky_offset_HP = 0;
		}		
		
		
		// requete MySQL pour obtenir les HP et HC de la fin du mois précédent
		$SQL="SELECT DATE_FORMAT(date_time, '%d-%m-%Y') AS DATE, HC, HP
				FROM  `teleinfo` 
				WHERE MONTH( date_time ) = $mois_prec[0] AND YEAR( date_time ) = $annee[0]
				ORDER BY date_time DESC
				LIMIT 0 , 1";
		// echo "<br>requete SQL : $SQL<br>";
		$RESULT = @mysqli_query($link, $SQL);
		
		//on lit la seconde ligne avec les HP+HC de fin d'année
		if($myrow=@mysqli_fetch_array($RESULT)){
			
			$data_HC_fin[0] = $myrow["HC"] / 1000; // on divise par mille pour avoir des kWh
			$data_HP_fin[0] = $myrow["HP"] / 1000; // on divise par mille pour avoir des kWh
			// echo "data_HC_fin : $data_HC_fin[0]     data_HP_fin : $data_HP_fin[0] <br>";

			// On calcul la conso mensuelle
			$data_consomoisHC[$n] = $data_HC_fin[0] - $data_HC_deb[0] + $linky_offset_HC;
			$data_consomoisHP[$n] = $data_HP_fin[0] - $data_HP_deb[0] + $linky_offset_HP;
			
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
	
	mysqli_free_result($RESULT) ;
	mysqli_close($link);
	fclose($fp);
	echo"CSV teleinfo_mois_LIGHT exporté.<br>";
?>