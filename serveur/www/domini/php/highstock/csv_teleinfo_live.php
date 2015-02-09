<?php // content="text/plain; charset=utf-8"
	include("../../infos/config.inc.php"); // on inclu le fichier de config
	$filename = "../../csv/teleinfo_live.csv";

	// requete MySQL pour obtenir les données de la BDD
	//echo" $host, $login, $passe, $bdd \n";

	@mysql_connect($host,$login,$passe) or die("Impossible de se connecter à la base de données");
	@mysql_select_db("$bdd") or die("Impossible de se connecter à la base de données");

	// echo" jour : $i, n: $n<br>";
	//requete pour récupérer les infos du mois en cours
	//$SQL="SELECT TIME(date_time) AS Heure, puissance FROM  teleinfo WHERE  DAY(date_time)=$jour AND YEAR(date_time)=$annee AND MONTH(date_time)=$mois ORDER BY date_time"; 
	$SQL="SELECT DATE_FORMAT(date_time, '%d-%m-%Y') AS DATE, DATE_FORMAT(date_time, '%H:%i:%s') AS Heure, puissance, tarif 
	FROM teleinfo 
	WHERE date_time <= NOW( ) AND date_time >= SUBDATE(NOW(),2) 
	ORDER BY date_time"; 
	
	$RESULT = @mysql_query($SQL);
	
	// fetch a row and write the column names out to the file
	$row = mysql_fetch_assoc($RESULT);
	$fp = fopen($filename, "w");
	$line = "DATE;Heure;puissance;HP;HC\n";
	fputs($fp, $line);
	$n=0;
	
	// remove the result pointer back to the start
	mysql_data_seek($RESULT, 0);
	
	while($myrow = @mysql_fetch_array($RESULT)) {
		 $data_timestamp[] = $myrow["DATE"];
		 $data_date[] = $myrow["Heure"];
		if($myrow["tarif"] == "HP.."){
			$data_consojour_HP[] = $myrow["puissance"];
			$data_consojour_HC[] = 0;
		}else{
			$data_consojour_HP[] = 0;
			$data_consojour_HC[] = $myrow["puissance"];
		}
		//echo" <br> data_timestamp:$data_timestamp, data_date:$data_date, data_consojour_HP:$data_consojour_HP, data_consojour_HC:$data_consojour_HC";
		
		$line = $data_timestamp[$n] .";". $data_date[$n] .";". $data_consojour_HP[$n] .";". $data_consojour_HC[$n]."\n"; 
		fputs($fp, $line);
		$n = $n + 1;
	}

	fclose($fp);	 
		 
	echo"CSV teleinfo_live exporté.<br>";

	//on quite la session mysql
	mysql_free_result($RESULT) ;
	mysql_close();
?>