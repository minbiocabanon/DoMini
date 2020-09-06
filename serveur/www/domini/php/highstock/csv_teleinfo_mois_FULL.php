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

		$link = mysqli_connect($host,$login,$passe,$bdd);
	if (!$link) {
		die('Erreur de connexion (' . mysqli_connect_errno() . ') '
				. mysqli_connect_error());
	}
	
	$SQL="SET lc_time_names = 'fr_FR'" ; // Pour afficher date en français dans MySql. 
	mysqli_query($link, $SQL) ;

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
			// LINKY : Septembre 2020 , installation LINKY qui a remis à O le compteur HC/HP : ajout d'un offset avec les valeurs HC/HP du précédent compteur
			if($annee == 2020 && $m == 9){
				// requete MySQL pour obtenir les HP et HC avant la pose du linky
				$SQL="SELECT `linky_hc`,`linky_hp` FROM `linky_offset`";
				// echo "<br>requete SQL : $SQL<br>";
				$RESULT = @mysqli_query($link, $SQL);	
				// on recupére le resultat
				$myrow = @mysqli_fetch_array($RESULT);
				// on stocke la quantité de HP et HC consommés (en kWh)
				$linky_offset_HC = $myrow["linky_hc"] ;
				$linky_offset_HP = $myrow["linky_hp"] ;

				// on clean et on ferme la BDD
				mysqli_free_result($RESULT) ;				
			}
			else{
				$linky_offset_HC = 0;
				$linky_offset_HP = 0;
			}
			
			
			
			// requete MySQL pour obtenir les HP et HC du mois
			$SQL="SELECT DATE_FORMAT(date_time, '%d-%m-%Y') AS DATE, HC, HP
					FROM  `teleinfo` 
					WHERE MONTH( date_time ) = $m AND YEAR( date_time ) = $annee
					ORDER BY date_time ASC
					LIMIT 0 , 1";
			// echo "<br>requete SQL : $SQL<br>";
			$RESULT = @mysqli_query($link, $SQL);
			
			//on ne prend que le premier élement de la table HC et HP
			if($myrow = @mysqli_fetch_assoc($RESULT)){
			
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
				$RESULT = @mysqli_query($link, $SQL);
				
				//on lit la seconde ligne avec les HP+HC de fin d'année
				if($myrow=@mysqli_fetch_assoc($RESULT)){
					
					$data_HC_fin = $myrow["HC"] / 1000; // on divise par mille pour avoir des kWh
					$data_HP_fin = $myrow["HP"] / 1000; // on divise par mille pour avoir des kWh
					// echo "$data_date - $data_HC_fin , $data_HP_fin <br>";

					// On calcul la conso mensuelle
					$data_consomoisHC = $data_HC_fin - $data_HC_deb + $linky_offset_HC;
					$data_consomoisHP = $data_HP_fin - $data_HP_deb + $linky_offset_HP;
					
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
	mysqli_free_result($RESULT) ;
	mysqli_close($link);

	echo"CSV teleinfo_mois_FULL exporté.<br>";
?>