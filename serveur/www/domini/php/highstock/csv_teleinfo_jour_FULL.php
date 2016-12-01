<?php // content="text/plain; charset=utf-8"
	include("../../infos/config.inc.php"); // on inclu le fichier de config
	include("../../infos/tarifedf.inc.php"); // on inclu le fichier avec les tarifs edf
	$filename = "../../csv/teleinfo_jour_full.csv";
	
	//on prépare l'entete du fichier CSV
	$fp = fopen($filename, "w");
	$line = "DATE;heure;consoHP;consoHC\n";
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
	$annee_crte = date('Y');
	//on récupére le mois en cours (1-12)
	$mois_crt = date('m');	
	
	$n=0;

	//Pour chaque année depuis 2010 jusqu'à aujourd'hui
	for($annee = 2010; $annee <= $annee_crte; $annee++){
		
		//pour l'année en cours, on s'arrête au mois en cours
		if($annee == $annee_crte)
			//on récupére le mois en cours (1-12)
			$mois = date('m');
		else
			// sinon on fait pour les 12 mois
			$mois = 12;
	
		for($m=1 ; $m<=$mois ; $m++){
			//pour l'année en cours et le mois, on s'arrête au jour en cours
			if($annee == $annee_crte && $m = $mois_crt){
				//on récupére le mois en cours (1-12)
				$jour = date('j');
			}
			else{
				// requete MySQL pour obtenir le nb de jour dans le mois en cours
				$SQL="SELECT COUNT( DISTINCT DAY( date_time ) ) AS NBJOUR
					FROM teleinfo
					WHERE MONTH( date_time ) = $m";
				// echo "<br>requete SQL : $SQL<br>";
				$RESULT = @mysqli_query($link, $SQL);
				$myrow=@mysqli_fetch_assoc($RESULT);
				$jour = $myrow["NBJOUR"];
			}
			
			// Pour les jours du mois en cours
			 for($j=1 ; $j<=$jour ; $j++){
				// echo" jour : $i, n: $n<br>";
				//requete pour récupérer les infos du mois en cours
				$SQL="SELECT  DATE_FORMAT(date_time, '%d-%m-%Y') AS DATE, HC,HP 
				FROM  teleinfo 
				WHERE  DAY(date_time)=$j AND YEAR(date_time)=$annee AND MONTH(date_time)=$m 
				ORDER BY date_time
				LIMIT 0,1"; 
				$RESULT = @mysqli_query($link, $SQL);
				
				//on ne prend que le premier élement de la table HC et HP
				if($myrow=@mysqli_fetch_assoc($RESULT)){
					// echo "<br>resultat : $myrow[1]";
					$data_date = $myrow["DATE"];
					$data_HC_deb = $myrow["HC"] / 1000; // on divise par mille pour avoir des kWh
					$data_HP_deb = $myrow["HP"] / 1000; // on divise par mille pour avoir des kWh
					// echo "data_date : $data_date[$n]   <br> data_HC_deb : $data_HC_deb[0]     data_HP_deb : $data_HP_deb[0] <br>";

					//requete pour récupérer les infos du mois en cours
					$SQL="SELECT HC,HP 
					FROM  teleinfo 
					WHERE  DAY(date_time)=$j AND YEAR(date_time)=$annee AND MONTH(date_time)=$m 
					ORDER BY date_time DESC
					LIMIT 0,1"; 
					$RESULT = @mysqli_query($link, $SQL);
					
					//on lit la seconde ligne avec les HP+HC de fin d'année
					if( $myrow=@mysqli_fetch_assoc($RESULT)){
						
						$data_HC_fin = $myrow["HC"] / 1000; // on divise par mille pour avoir des kWh
						$data_HP_fin = $myrow["HP"] / 1000; // on divise par mille pour avoir des kWh
						// echo "data_HC_fin : $data_HC_fin[0]     data_HP_fin : $data_HP_fin[0] <br>";

						// On calcul la conso mensuelle
						$data_consojourHC = $data_HC_fin - $data_HC_deb ;
						$data_consojourHP = $data_HP_fin - $data_HP_deb ;
						
						//DEBUG
						//echo "data_consojourHC : $data_consojourHC[$n]; <br> data_consojourHP : $data_consojourHP[$n];<br>";
								
						// On converti en euros
						$data_consojourHC = round($tarif_HC * $data_consojourHC , 2);
						$data_consojourHP = round($tarif_HP * $data_consojourHP , 2);
						
						//DEBUG
						//echo "data_consojourHC : $data_consojourHC; <br> data_consojourHP : $data_consojourHP;<br>";
						
						//on écrit les données dans le fichier CSV		
						$line = $data_date .";00:00:01;". $data_consojourHP .";". $data_consojourHC."\n"; 
						fputs($fp, $line);
					}
					$myrow=0;
				}
			 }
		}
	}
	mysqli_free_result($RESULT) ;
	mysqli_close($link);

	fclose($fp);
	
	echo"CSV teleinfo_jour exporté.<br>";
?>