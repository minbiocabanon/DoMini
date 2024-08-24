<?PHP 

	//--------------------------------------------------
	//! \file    instant_data.php
	//! \brief	cette page PHP est appel�e r�guli�rement par un CRON afin de r�cup�rer quelques infos essentielles affich�e sur la page principale de la domotique
	//! \brief 	Cela permet de r�duire le temps d'affichage de la page principale  (une seule requete dans une petite table plutot que plusieurs requetes dans des tables parfois volumineuses)
	//! \date     2012-03
	//! \author   minbiocabanon
	//--------------------------------------------------


	include("../infos/config.inc.php"); // on inclu le fichier de config 
	include("../infos/pyrano.inc.php"); // on inclu le fichier pour avoir les variables/constantes du pyranom�tre

	// ------------------- Donn�es TELEINFO -------------------------------------
	// requete MySQL pour obtenir les donn�es de la BDD
	echo " $host, $login, $passe, $bdd \n";
	$link = mysqli_connect($host,$login,$passe,$bdd);
	if (!$link) {
		die('Erreur de connexion (' . mysqli_connect_errno() . ') '
				. mysqli_connect_error());
	}
	
	//requete pour r�cup�rer la derni�re consommation instantan�e
	$SQL="SELECT date_time AS Heure, puissance, tarif 
	FROM teleinfo 
	ORDER BY date_time DESC
	LIMIT 0,1"; 
	//Envoie de la requete
	$RESULT = @mysqli_query($link,$SQL);
	// //lecture du resultat de la requete
	$myrow=@mysqli_fetch_array($RESULT); 
	//on r�cup�re l'heure
	$data_teleinfo_date[] = $myrow["Heure"];
	echo"<br/>Heure t�l�info = 	$data_teleinfo_date[0] <br>";
	//on r�cup�re la puissance consomm�
	$data_consojour[] = $myrow["puissance"];
	$data_tarif[] = $myrow["tarif"];
	
	//requete pour r�cup�rer d�terminer si les donn�es sont r�centes
	$SQL="SELECT date_time 
	FROM teleinfo 
	WHERE date_time >= SUBTIME( NOW( ) ,  '1:00:00' )"; 
	//Envoie de la requete
	$RESULT = @mysqli_query($link,$SQL);
	// on r�cup�re le nombre de r�sultat
	$numrows = mysqli_num_rows($RESULT);
	echo"nb de ligne teleinfo = $numrows <br>";
	//si le nombre de mesures est >= 10 
	if($numrows >= 200){
		//on affiche une led verte
		$str_lededf="label-success";
	//si le nombre de mesures est compris entre 5 et 10
	}elseif($numrows >= 50){
		//on affiche une led orange
		$str_lededf="label-warning";
	}else{
		//on affiche une led rouge
		$str_lededf="label-important";
	}


	// ------------------- Donn�es Temp�rature int�rieure  -------------------------------------

	//requete pour r�cup�rer la derni�re consommation instantan�e
	$SQL="SELECT date_time, ana1, ana2 
	FROM analog2 
	ORDER BY date_time DESC
	LIMIT 0,1"; 
	//Envoie de la requete
	$RESULT = @mysqli_query($link,$SQL);
	// lecture du resultat de la requete
	$myrow=@mysqli_fetch_array($RESULT); 
	//on r�cup�re la derni�re temp�rature relev�e
	$data_dateint[0] = $myrow["date_time"];
	$data_tempint[0] = round($myrow["ana1"],2);
	$data_humint[0] = round($myrow["ana2"],2);
	
	//requete pour r�cup�rer d�terminer si les donn�es sont r�centes
	$SQL="SELECT date_time 
	FROM analog2 
	WHERE date_time >= SUBTIME( NOW( ) ,  '1:00:00' )"; 
	//Envoie de la requete
	$RESULT = @mysqli_query($link,$SQL);
	// on r�cup�re le nombre de r�sultat
	$numrows = mysqli_num_rows($RESULT);
	echo"nb de ligne Ti = $numrows <br>";
	//si le nombre de mesures est >= 10 
	if($numrows >= 10){
		//on affiche une led verte
		$str_ledTi="label-success";
	//si le nombre de mesures est compris entre 5 et 10
	}else if($numrows >= 1){
		//on affiche une led orange
		$str_ledTi="label-warning";
	}else{
		//on affiche une led rouge
		$str_ledTi="label-important";
	}

	//Requete pour d�terminer si la temp�rature monte ou descend
	$SQL="SELECT  ana1  AS TempMoy, AVG( ana2 ) AS HumMoy
	FROM  `analog2` 
	WHERE date_time <= NOW( ) 
	AND date_time >= SUBTIME( NOW( ) ,  '01:00:00' )"; 
	//Envoie de la requete
	$RESULT = @mysqli_query($link, $link,$SQL);
		
	// lecture du resultat de la requete
	$myrow=@mysqli_fetch_array($RESULT);
	//on r�cup�re la temp�rature pr�c�dente
	$data_tempmoy[0] = $myrow["TempMoy"];
	$data_hummoy[0] = $myrow["HumMoy"];
	//on calcul la diff�rence pour savoir si la T� monte ou descend
	$data_deltatemp = $data_tempint[0] - $data_tempmoy[0];
	$data_deltahum = $data_humint[0] - $data_hummoy[0];

	//si le delta T est positif
	if($data_deltatemp > 0){
		//la temp�rature monte
		$str_iconTi="flecheup.png";
	}else{
		//la temp�rature descend 
		$str_iconTi="flechedown.png";
	}

	//si le delta H est positif
	if($data_deltahum > 0){
		//la temp�rature monte
		$str_iconHi="flecheup.png";
	}else{
		//la temp�rature descend 
		$str_iconHi="flechedown.png";
	}

	//Requete pour d�terminer la pente sur la derni�re heure
	$SQL="SELECT ana1 
	FROM analog2
	WHERE date_time >= SUBTIME( NOW( ) ,  '01:00:00' )						
	ORDER BY date_time ASC
	LIMIT 0,1"; 
	// //Envoie de la requete
	$RESULT = @mysqli_query($link,$SQL);
	// lecture du resultat de la requete
	$myrow=@mysqli_fetch_array($RESULT); 
	//on r�cup�re la temp�rature relev�e il y a 1h
	$data_temp1h = $myrow["ana1"];
	//on calcul la pente sur la derni�re heure
	$data_pentetemp = round($data_tempint[0] - $data_temp1h , 2);
	echo "data_temp1h : $data_temp1h  - data_tempint[0] : $data_tempint[0] -  pente : $data_pentetemp";

	// ------------------- Donn�es Temp�rature ext�rieure  -------------------------------------

	//requete pour r�cup�rer la derni�re consommation instantan�e
	$SQL="SELECT date_time, ana1, ana2 
	FROM analog1 
	ORDER BY date_time DESC
	LIMIT 0,1"; 
	//Envoie de la requete
	$RESULT = mysqli_query($link,$SQL);
	// lecture du resultat de la requete
	$myrow=@mysqli_fetch_array($RESULT); 
	//on r�cup�re la derni�re temp�rature relev�e
	$data_dateext[0] = $myrow["date_time"];
	$data_tempext[0] = round($myrow["ana1"],1);
	$data_humext[0] = round($myrow["ana2"],1);

	//requete pour r�cup�rer d�terminer si les donn�es sont r�centes
	$SQL="SELECT date_time 
	FROM analog1 
	WHERE date_time >= SUBTIME( NOW( ) ,  '1:00:00' )";
	//Envoie de la requete
	$RESULT = mysqli_query($link,$SQL);	
	// on r�cup�re le nombre de r�sultat
	$numrows = mysqli_num_rows($RESULT);
	echo"<br/>nb de ligne Te = $numrows <br>";
	//si le nombre de mesures est >= 10 
	if($numrows >= 10){
		//on affiche une led verte
		$str_ledTe="label-success";
	//si le nombre de mesures est compris entre 5 et 10
	}else if($numrows >= 1){
		//on affiche une led orange
		$str_ledTe="label-warning";
	}else{
		//on affiche une led rouge
		$str_ledTe="label-important";
	}	
	
	

	//Requete pour d�terminer si la temp�rature monte ou descend
	$SQL="SELECT  ana1 AS TempMoy, AVG( ana2 ) AS HumMoy
	FROM  `analog1` 
	WHERE date_time <= NOW( ) 
	AND date_time >= SUBTIME( NOW( ) ,  '01:00:00' )"; 
	//Envoie de la requete
	$RESULT = mysqli_query($link,$SQL);
	
	// lecture du resultat de la requete
	$myrow=@mysqli_fetch_array($RESULT);
	//on r�cup�re la temp�rature pr�c�dente
	$data_tempmoy[0] = $myrow["TempMoy"];
	$data_hummoy[0] = $myrow["HumMoy"];
	//on calcul la diff�rence pour savoir si la T� monte ou descend
	$data_deltatemp = $data_tempext[0] - $data_tempmoy[0];
	$data_deltahum = $data_humext[0] - $data_hummoy[0];

	//si le delta T est positif
	if($data_deltatemp > 0){
		//la temp�rature monte
		$str_iconTe="flecheup.png";
	}else{
		//la temp�rature descend 
		$str_iconTe="flechedown.png";
	}

	//si le delta H est positif
	if($data_deltahum > 0){
		//la temp�rature monte
		$str_iconHe="flecheup.png";
	}else{
		//la temp�rature descend 
		$str_iconHe="flechedown.png";
	}

	//Requete pour d�terminer la pente sur la derni�re heure
	$SQL="SELECT ana1 
	FROM analog1
	WHERE date_time >= SUBTIME( NOW( ) ,  '01:00:00' )						
	ORDER BY date_time ASC
	LIMIT 0,1"; 
	// //Envoie de la requete
	$RESULT = @mysqli_query($link,$SQL);
	// lecture du resultat de la requete
	$myrow=@mysqli_fetch_array($RESULT); 
	//on r�cup�re la temp�rature relev�e il y a 1h
	$data_tempext1h = $myrow["ana1"];
	//on calcul la pente sur la derni�re heure
	$data_pentetempext = round($data_tempext[0] - $data_tempext1h , 2);
	echo "data_tempext1h : $data_tempext1h  - data_tempext[0] : $data_tempext[0] -  pente : $data_pentetempext";


	// ------------------- Donn�es Temp�rature du Puits Canadien  -------------------------------------
	// requete MySQL pour obtenir les donn�es de la BDD
	//echo" $host, $login, $passe, $bdd \n";
	@mysqli_connect($host,$login,$passe,$bdd);
	//requete pour r�cup�rer la derni�re consommation instantan�e
	$SQL="SELECT date_time, ana1, ana2 
	FROM analog3 
	WHERE date_time
	ORDER BY date_time DESC
	LIMIT 0,1"; 
	//Envoie de la requete
	$RESULT = @mysqli_query($link,$SQL);
	// lecture du resultat de la requete
	$myrow=@mysqli_fetch_array($RESULT); 
	//on r�cup�re la derni�re temp�rature relev�e
	$data_datePC[0] = $myrow["date_time"];
	$data_tempPC[0] = round($myrow["ana1"],1);
	$data_humPC[0] = round($myrow["ana2"],1);
	
	
	//requete pour r�cup�rer d�terminer si les donn�es sont r�centes
	$SQL="SELECT date_time 
	FROM analog3 
	WHERE date_time >= SUBTIME( NOW( ) ,  '1:00:00' )"; 
	//Envoie de la requete
	$RESULT = @mysqli_query($link,$SQL);
	//on r�cup�re le nombre de r�sultat
	//on r�cup�re le nombre de r�sultat
	$numrows = mysqli_num_rows($RESULT);
	echo"<br/>nb de ligne PC = $numrows <br>";
	//si le nombre de mesures est >= 10 
	if($numrows >= 10){
		//on affiche une led verte
		$str_ledPC="label-success";
	//si le nombre de mesures est compris entre 5 et 10
	}elseif($numrows >= 1){
		//on affiche une led orange
		$str_ledPC="label-warning";
	}else{
		//on affiche une led rouge
		$str_ledPC="label-important";
	}

	// ------------------- R�cup�re puissance pc actuelle -------------------------------------
	//Requete pour r�cup�rer la valeur 15 minutes auparavent
	$SQL="SELECT date_time, puissance
	FROM puissance_pc
	WHERE date_time >= SUBTIME( NOW( ) ,  '00:15:00' ) 
	LIMIT 0,1						
	"; 
	// //Envoie de la requete
	$RESULT = @mysqli_query($link,$SQL);
	// lecture du resultat de la requete
	$myrow=@mysqli_fetch_array($RESULT); 
	//on r�cup�re la derni�re temp�rature relev�e
	if($myrow["puissance"] != '')
		$puissance_pc = $myrow["puissance"];
	else
		$puissance_pc = 0.0 ;
	
	
	// ------------------- R�cup�re puissance pc actuelle -------------------------------------
	//Requete pour d�terminer la valeur moyenne sur les derni�res 20 minutes
	$SQL="SELECT puissance
	FROM chauffage_log
	ORDER BY date_time DESC 
	LIMIT 0,1						
	"; 
	// //Envoie de la requete
	$RESULT = @mysqli_query($link,$SQL);
	// lecture du resultat de la requete
	$myrow=@mysqli_fetch_array($RESULT); 
	//on r�cup�re la derni�re temp�rature relev�e
	if($myrow["puissance"] != '')
		$puissance_poele = $myrow["puissance"];
	else
		$puissance_poele = 0 ;
	
	// ------------------- R�cup�re les infos des temp�ratures de la VMCDF -------------------------------------
	//Requete pour r�cu�re les derni�res valeurs
	$SQL="SELECT ana1 AS temp_air_neuf, ana2 AS temp_air_extrait, ana3 AS temp_air_repris, ana4 AS temp_air_ext
	FROM analog6
	ORDER BY date_time DESC 
	LIMIT 0,1						
	"; 
	// //Envoie de la requete
	$RESULT = @mysqli_query($link,$SQL);
	// lecture du resultat de la requete
	$myrow=@mysqli_fetch_array($RESULT); 
	//on r�cup�re la derni�re temp�rature relev�e
	$vmc_temp_air_neuf = $myrow["temp_air_neuf"];	
	$vmc_temp_air_ext = $myrow["temp_air_ext"];
	$vmc_temp_air_repris = $myrow["temp_air_repris"];
	$vmc_temp_air_extrait = $myrow["temp_air_extrait"];
	
	//echo "<br> vmc_temp_air_neuf = $vmc_temp_air_neuf ; vmc_temp_air_ext=$vmc_temp_air_ext ; vmc_temp_air_repris=$vmc_temp_air_repris ; vmc_temp_air_extrait=$vmc_temp_air_extrait.<br>";
	
	// ------------------- R�cup�re la temp�rature de sortie de bouche ventilation-------------------------------------
	//Requete pour r�cu�re les derni�res valeurs
	$SQL="SELECT ana1 AS temp_air_bouche
	FROM analog4
	ORDER BY date_time DESC 
	LIMIT 0,1						
	"; 
	// //Envoie de la requete
	$RESULT = @mysqli_query($link,$SQL);
	// lecture du resultat de la requete
	$myrow=@mysqli_fetch_array($RESULT); 
	//on r�cup�re la derni�re temp�rature relev�e
	$vmc_temp_air_bouche = $myrow["temp_air_bouche"];

	//requete pour r�cup�rer d�terminer si les donn�es sont r�centes
	$SQL="SELECT date_time 
	FROM analog4 
	WHERE date_time >= SUBTIME( NOW( ) ,  '1:00:00' )"; 
	//Envoie de la requete
	$RESULT = @mysqli_query($link,$SQL);
	//on r�cup�re le nombre de r�sultat
	//on r�cup�re le nombre de r�sultat
	$numrows = mysqli_num_rows($RESULT);
	echo"<br/>nb de ligne analog4 (air bouche insuflation) = $numrows <br>";
	//si le nombre de mesures est >= 10 
	if($numrows >= 10){
		//on affiche une led verte
		$str_ledAirneuf="label-success";
	//si le nombre de mesures est compris entre 5 et 10
	}elseif($numrows >= 1){
		//on affiche une led orange
		$str_ledAirneuf="label-warning";
	}else{
		//on affiche une led rouge
		$str_ledAirneuf="label-important";
	}	

	// ------------------- R�cup�re la temp�rature de sortie du garage (capteur dans bypass PC) -------------------------------------
	//Requete pour r�cu�re les derni�res valeurs
	$SQL="SELECT ana1 AS temp_garage
	FROM analog5
	ORDER BY date_time DESC 
	LIMIT 0,1						
	"; 
	// //Envoie de la requete
	$RESULT = @mysqli_query($link,$SQL);
	// lecture du resultat de la requete
	$myrow=@mysqli_fetch_array($RESULT); 
	//on r�cup�re la derni�re temp�rature relev�e
	$temp_garage = $myrow["temp_garage"];
	
	//requete pour r�cup�rer d�terminer si les donn�es sont r�centes
	$SQL="SELECT date_time 
	FROM analog5 
	WHERE date_time >= SUBTIME( NOW( ) ,  '1:00:00' )"; 
	//Envoie de la requete
	$RESULT = @mysqli_query($link,$SQL);
	//on r�cup�re le nombre de r�sultat
	$numrows = mysqli_num_rows($RESULT);
	echo"<br/>nb de ligne analog5 = $numrows <br>";
	//si le nombre de mesures est >= 10 
	if($numrows >= 10){
		//on affiche une led verte
		$str_ledGarage="label-success";
	//si le nombre de mesures est compris entre 5 et 10
	}elseif($numrows >= 1){
		//on affiche une led orange
		$str_ledGarage="label-warning";
	}else{
		//on affiche une led rouge
		$str_ledGarage="label-important";
	}	
	
	// ------------------- R�cup�re la temp�rature des combles-------------------------------------
	//Requete pour r�cu�re les derni�res valeurs

	// anti bug 
	$temp_comble = 0.0;

	// $SQL="SELECT ana1 AS temp_comble
	// FROM analog5
	// ORDER BY date_time DESC 
	// LIMIT 0,1						
	// "; 
	// // //Envoie de la requete
	// $RESULT = @mysqli_query($link,$SQL);
	// // lecture du resultat de la requete
	// $myrow=@mysqli_fetch_array($RESULT); 
	// //on r�cup�re la derni�re temp�rature relev�e
	// $temp_comble = $myrow["temp_comble"];
	
	// //requete pour r�cup�rer d�terminer si les donn�es sont r�centes
	// $SQL="SELECT date_time 
	// FROM analog5 
	// WHERE date_time >= SUBTIME( NOW( ) ,  '1:00:00' )"; 
	// //Envoie de la requete
	// $RESULT = @mysqli_query($link,$SQL);
	// //on r�cup�re le nombre de r�sultat
	// //on r�cup�re le nombre de r�sultat
	// $numrows = mysqli_num_rows($RESULT);
	// echo"<br/>nb de ligne analog5 = $numrows <br>";
	// //si le nombre de mesures est >= 10 
	// if($numrows >= 2){
	// 	//on affiche une led verte
	// 	$str_ledCombles="label-success";
	// //si le nombre de mesures est compris entre 5 et 10
	// }elseif($numrows >= 1){
	// 	//on affiche une led orange
	// 	$str_ledCombles="label-warning";
	// }else{
	// 	//on affiche une led rouge
	// 	$str_ledCombles="label-important";
	// }
	
	
	
	// ------------------- R�cup�re le flux solaire-------------------------------------
	//Requete pour r�cu�re les derni�res valeurs
	$SQL="SELECT ana1 as flux_solaire
	FROM pyranometre
	ORDER BY date_time DESC 
	LIMIT 0,1						
	"; 
	// //Envoie de la requete
	$RESULT = @mysqli_query($link,$SQL);
	// lecture du reisultat de la requete
	$myrow=@mysqli_fetch_array($RESULT); 
	//on r�cup�re la derni�re temp�rature relev�e
	$flux_solaire = $myrow["flux_solaire"] * $Coef_V_to_W;
	echo"<br/>flux solaire = $flux_solaire W/m�, coef=$Coef_V_to_W";
	
	// ------------------- R�cup�re l'�tat du bypass puits canadien -------------------------------------
	//Requete pour r�cup�rer la valeur 15 minutes auparavent
	$SQL="SELECT date_time, consigne
	FROM bypass_pc_log
	WHERE date_time >= SUBTIME( NOW( ) ,  '00:15:00' ) 
	LIMIT 0,1						
	"; 
	// //Envoie de la requete
	$RESULT = @mysqli_query($link,$SQL);
	// lecture du resultat de la requete
	$myrow=@mysqli_fetch_array($RESULT); 
	//on r�cup�re la derni�re temp�rature relev�e
	if( $myrow["consigne"] != '' )
		$consigne_pc = $myrow["consigne"];
	else
		$consigne_pc = 0.0;

	// ------------------- R�cup�re l'�tat de l'oduleur (ups) -------------------------------------
	//Requete pour r�cup�rer la derni�re valeur
	$SQL="SELECT `date_time`,`ups_status`,`battery_charge`,`battery_runtime` 
	FROM `ups` 
	ORDER BY `date_time` DESC 
	LIMIT 0,1					
	"; 
	// //Envoie de la requete
	$RESULT = @mysqli_query($link,$SQL);
	// lecture du resultat de la requete
	$myrow=@mysqli_fetch_array($RESULT); 
	//on r�cup�re la derni�re temp�rature relev�e
	$ups_datetime = $myrow["date_time"];
	$ups_battery_charge = $myrow["battery_charge"];
	$ups_battery_runtime = $myrow["battery_runtime"];
	if( $myrow["ups_status"] == "OB DISCHRG")
		$ups_status = "DESCHRG";
	elseif($myrow["ups_status"] == "OL CHRG")
		$ups_status = "OK";
	elseif($myrow["ups_status"] == "OL")
		$ups_status = "OK";
	else	
		$ups_status = "???";
	

	// ------------------- On �crit toutes les donn�es dans une seule table -------------------------------------
	//on met �  la table donnees_instant
	$SQL="UPDATE `domotique`.`donnees_instant` SET 
		`date_time` = NOW(),
		`date_int` = '$data_dateint[0]',
		`T_int` = '$data_tempint[0]',
		`H_int` = '$data_humint[0]',
		`pente_T_int` = '$data_pentetemp',
		`date_ext` = '$data_dateext[0]',
		`T_ext` = '$data_tempext[0]',
		`H_ext` = '$data_humext[0]',
		`pente_T_ext` = '$data_pentetempext',
		`date_pc` = '$data_datePC[0]',
		`T_pc` = '$data_tempPC[0]',
		`H_pc` = '$data_humPC[0]',
		`vmc_temp_air_neuf` = '$vmc_temp_air_neuf',
		`vmc_temp_air_ext` = '$vmc_temp_air_ext',
		`vmc_temp_air_repris` = '$vmc_temp_air_repris',
		`vmc_temp_air_extrait` = '$vmc_temp_air_extrait',
		`vmc_temp_air_bouche` = '$vmc_temp_air_bouche',
		`temp_comble` = '$temp_comble',
		`temp_garage` = '$temp_garage',
		`pente_T_pc` = '1',
		`puissance_pc` = '$puissance_pc',
		`consigne_pc` = '$consigne_pc',
		`puissance_poele` = '$puissance_poele',
		`flux_solaire` = '$flux_solaire',
		`date_teleinfo` = '$data_teleinfo_date[0]',
		`tarif_watt` = '$data_tarif[0]',
		`conso_watt` = '$data_consojour[0]',
		`icon_Ti` = '$str_iconTi',
		`icon_Hi` = '$str_iconHi',
		`icon_Te` = '$str_iconTe',
		`icon_He` = '$str_iconHe',
		`icon_led_edf` = '$str_lededf',
		`icon_led_Ti` = '$str_ledTi',
		`icon_led_Te` = '$str_ledTe',
		`icon_led_pc` = '$str_ledPC',
		`icon_led_garage` = '$str_ledGarage',
		`icon_led_combles` = '$str_ledCombles',
		`icon_led_airneuf` = '$str_ledAirneuf',
		`ups_datetime` = '$ups_datetime',
		`ups_status` = '$ups_status',
		`ups_battery_charge` = '$ups_battery_charge',
		`ups_battery_runtime` = '$ups_battery_runtime'
		WHERE `donnees_instant`.`id` =1;";
	echo"</br>requete SQL = $SQL";
	//Envoie de la requete
    if (!mysqli_query($link,$SQL)) {
        printf("</br>Erreur : %s\n", mysqli_error($link));
    }else{
		echo"</br>Donn�es sauv�es dans la BDD";
	}
	//on quitte la BDD
	mysqli_free_result($RESULT);
	mysqli_close($link);
?>