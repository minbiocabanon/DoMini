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

	@mysql_connect($host,$login,$passe) or die("Impossible de se connecter à la base de données");
	@mysql_select_db("$bdd") or die("Impossible de se connecter à la base de données");
	$SQL="SET lc_time_names = 'fr_FR'" ; // Pour afficher date en français dans MySql. 
	mysql_query($SQL) ;
	//on récupére l'année en cours
	$annee = date('Y');
	//on récupére le mois en cours (1-12)
	$mois = date('m');
	//on récupére le mois en cours (Janvier-Decembre)
	//$moisM = date('F');
	$moisM = strftime("%B");
	
	
	//on récupére le mois en cours (1-31)
	$jour = date('j');
	$n=0;

	//DEBUG
	for($m=1 ; $m<=$mois ; $m++){

		// requete MySQL pour obtenir le nb de jour dans le mois en cours
		$SQL="SELECT COUNT( DISTINCT DAY( date_time ) ) AS NBJOUR
			FROM teleinfo
			WHERE MONTH( date_time ) = $m";
		// echo "<br>requete SQL : $SQL<br>";
		$RESULT = @mysql_query($SQL);
		$myrow=@mysql_fetch_array($RESULT);
		$jour = $myrow["NBJOUR"];
		
		// Pour les jours du mois en cours
		 for($j=1 ; $j<=$jour ; $j++){
			// echo" jour : $i, n: $n<br>";
			//requete pour récupérer les infos du mois en cours
			$SQL="SELECT  DATE_FORMAT(date_time, '%d-%m-%Y') AS DATE, HC,HP 
			FROM  teleinfo 
			WHERE  DAY(date_time)=$j AND YEAR(date_time)=$annee AND MONTH(date_time)=$m 
			ORDER BY date_time
			LIMIT 0,1"; 
			$RESULT = @mysql_query($SQL);
			
			//on ne prend que le premier élement de la table HC et HP
			if($myrow=@mysql_fetch_array($RESULT)){
				// echo "<br>resultat : $myrow[1]";
				$data_date[$n]= $myrow["DATE"];
				$data_HC_deb[0] = $myrow["HC"] / 1000; // on divise par mille pour avoir des kWh
				$data_HP_deb[0] = $myrow["HP"] / 1000; // on divise par mille pour avoir des kWh
				// echo "data_date : $data_date[$n]   <br> data_HC_deb : $data_HC_deb[0]     data_HP_deb : $data_HP_deb[0] <br>";

				//requete pour récupérer les infos du mois en cours
				$SQL="SELECT DATE_FORMAT(date_time, '%d-%m-%Y') AS DATE, HC,HP 
				FROM  teleinfo 
				WHERE  DAY(date_time)=$j AND YEAR(date_time)=$annee AND MONTH(date_time)=$m 
				ORDER BY date_time DESC
				LIMIT 0,1"; 
				$RESULT = @mysql_query($SQL);
				
				//on lit la seconde ligne avec les HP+HC de fin d'année
				if( $myrow=@mysql_fetch_array($RESULT)){
					
					$data_HC_fin[0] = $myrow["HC"] / 1000; // on divise par mille pour avoir des kWh
					$data_HP_fin[0] = $myrow["HP"] / 1000; // on divise par mille pour avoir des kWh
					// echo "data_HC_fin : $data_HC_fin[0]     data_HP_fin : $data_HP_fin[0] <br>";

					// On calcul la conso mensuelle
					$data_consojourHC[$n] = $data_HC_fin[0] - $data_HC_deb[0] ;
					$data_consojourHP[$n] = $data_HP_fin[0] - $data_HP_deb[0] ;
					
					//DEBUG
					//echo "data_consojourHC : $data_consojourHC[$n]; <br> data_consojourHP : $data_consojourHP[$n];<br>";
							
					// On converti en euros
					$data_consojourHC[$n] = round($tarif_HC * $data_consojourHC[$n] , 2);
					$data_consojourHP[$n] = round($tarif_HP * $data_consojourHP[$n] , 2);
					
					//DEBUG
					//echo "data_consojourHC : $data_consojourHC[$n]; <br> data_consojourHP : $data_consojourHP[$n];<br>";
					
					//on écrit les données dans le fichier CSV		
					$line = $data_date[$n] .";00:00:01;". $data_consojourHPr[$n] .";". $data_consojourHCr[$n]."\n"; 
					fputs($fp, $line);
				}
				
				$n++;
				$myrow=0;
			}
		 }
	}
	
	mysql_free_result($RESULT) ;
	mysql_close();

	echo"CSV teleinfo_jour exporté.<br>";
?>