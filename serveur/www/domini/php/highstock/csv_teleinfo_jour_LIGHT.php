<?php // content="text/plain; charset=utf-8"

// ---------------------------------
//   !!!!! ATTENTION !!!!
// ---------------------------------
//
// CE SCRIPT NE FAIT QU'AJOUTER UNE LIGNE A LA FIN DU FICHIER EXISTANT
// AFIN DE NE PAS RELANCER LES REQUETES DEPUIS LA CREATION DES BASES -> TRES LONG !!!

	include("../../infos/config.inc.php"); // on inclu le fichier de config
	include("../../infos/tarifedf.inc.php"); // on inclu le fichier avec les tarifs edf
	$filename = "../../csv/teleinfo_jour.csv";
	
	//on prépare l'entete du fichier CSV
	// on ouvre le fichier en écriture seule et on place le pointeur à la fin du fichier -> option "a"
	$fp = fopen($filename, "a");
	
	@mysql_connect($host,$login,$passe) or die("Impossible de se connecter à la base de données");
	@mysql_select_db("$bdd") or die("Impossible de se connecter à la base de données");
	$SQL="SET lc_time_names = 'fr_FR'" ; // Pour afficher date en français dans MySql. 
	mysql_query($SQL) ;
	
	//on récupère la date de la veille
	$SQL="SELECT DATE_FORMAT( SUBDATE( NOW( ) , 1 ) , '%d-%m-%Y' ) AS DATE_VEILLE";
	$RESULT = @mysql_query($SQL);
	$myrow=@mysql_fetch_array($RESULT);
	$date_veille[] = $myrow["DATE_VEILLE"];
	
	echo"$date_veille[0] <br>";
	
	$n=0;

	//requete pour récupérer les infos de la veille
	$SQL="SELECT  DATE_FORMAT(date_time, '%d-%m-%Y') AS DATE, HC,HP 
	FROM  teleinfo 
	WHERE  DATE_FORMAT( date_time, '%d-%m-%Y' ) = '$date_veille[0]'
	ORDER BY date_time
	LIMIT 0,1"; 
	//echo "$SQL";
	$RESULT = @mysql_query($SQL);

	//on ne prend que le premier élement de la table HC et HP
	if($myrow=@mysql_fetch_array($RESULT)){
		//echo "<br>resultat : $myrow[1]";
		$data_date[$n]= $myrow["DATE"];
		$data_HC_deb[0] = $myrow["HC"] / 1000; // on divise par mille pour avoir des kWh
		$data_HP_deb[0] = $myrow["HP"] / 1000; // on divise par mille pour avoir des kWh
		// echo "data_date : $data_date[$n]   <br> data_HC_deb : $data_HC_deb[0]     data_HP_deb : $data_HP_deb[0] <br>";

		//requete pour récupérer les infos de la veille
		$SQL="SELECT DATE_FORMAT(date_time, '%d-%m-%Y') AS DATE, HC,HP 
		FROM  teleinfo 
		WHERE  DATE_FORMAT( date_time, '%d-%m-%Y' ) = '$date_veille[0]'  
		ORDER BY date_time DESC
		LIMIT 0,1"; 
		$RESULT = @mysql_query($SQL);
		
		//on lit la seconde ligne avec les HP+HC de fin d'année
		if( $myrow=@mysql_fetch_array($RESULT)){
			
			$data_HC_fin[0] = $myrow["HC"] / 1000; // on divise par mille pour avoir des kWh
			$data_HP_fin[0] = $myrow["HP"] / 1000; // on divise par mille pour avoir des kWh
			//echo "data_HC_fin : $data_HC_fin[0]     data_HP_fin : $data_HP_fin[0] <br>";

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
			$line = $data_date[$n] .";00:00:01;". $data_consojourHP[$n] .";". $data_consojourHC[$n]."\n"; 
			fputs($fp, $line);
			//echo "<br> ligne ajoutée : $line <br>";
		}
		
		$n++;
		$myrow=0;
	}

	
	mysql_free_result($RESULT) ;
	mysql_close();

	echo"CSV teleinfo_jour exporté.<br>";
?>