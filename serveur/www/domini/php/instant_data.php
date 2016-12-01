<?php 

	//--------------------------------------------------
	//! \file    instant_data.php
	//! \brief	cette page PHP est appelée régulièrement par un CRON afin de récupérer quelques infos essentielles affichée sur la page principale de la domotique
	//! \brief 	Cela permet de réduire le temps d'affichage de la page principale  (une seule requete dans une petite table plutot que plusieurs requetes dans des tables parfois volumineuses)
	//! \date     2012-03
	//! \author   minbiocabanon
	//--------------------------------------------------


	include("../infos/config.inc.php"); // on inclu le fichier de config 
	include("../infos/pyrano.inc.php"); // on inclu le fichier pour avoir les variables/constantes du pyranomètre

	// ------------------- Données TELEINFO -------------------------------------
	// requete MySQL pour obtenir les données de la BDD
	//echo" $host, $login, $passe, $bdd \n";
	$link = mysqli_connect$host,$login,$passe,$bdd);
	if (!$link) {
		die('Erreur de connexion (' . mysqli_connect_errno() . ') '
				. mysqli_connect_error());
	}
	
	//requete pour récupérer la dernière consommation instantanée
	$SQL="SELECT date_time AS Heure, puissance, tarif 
	FROM teleinfo 
	ORDER BY date_time DESC
	LIMIT 0,1"; 
	//Envoie de la requete
	$RESULT = @mysqli_query($link,$SQL);
	// //lecture du resultat de la requete
	$myrow=@mysqli_fetch_array($RESULT); 
	//on récupère l'heure
	$data_teleinfo_date[] = $myrow["Heure"];
	echo"<br/>Heure téléinfo = 	$data_teleinfo_date[0] <br>";
	//on récupère la puissance consommé
	$data_consojour[] = $myrow["puissance"];
	$data_tarif[] = $myrow["tarif"];
	
	//requete pour récupérer déterminer si les données sont récentes
	$SQL="SELECT date_time 
	FROM teleinfo 
	WHERE date_time >= SUBTIME( NOW( ) ,  '1:00:00' )"; 
	//Envoie de la requete
	$RESULT = @mysqli_query($link,$SQL);
	// on récupère le nombre de résultat
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


	// ------------------- Données Température intérieure  -------------------------------------

	//requete pour récupérer la dernière consommation instantanée
	$SQL="SELECT date_time, ana1, ana2 
	FROM analog2 
	ORDER BY date_time DESC
	LIMIT 0,1"; 
	//Envoie de la requete
	$RESULT = @mysqli_query($link,$SQL);
	// lecture du resultat de la requete
	$myrow=@mysqli_fetch_array($RESULT); 
	//on récupère la dernière température relevée
	$data_dateint[0] = $myrow["date_time"];
	$data_tempint[0] = round($myrow["ana1"],2);
	$data_humint[0] = round($myrow["ana2"],2);
	
	//requete pour récupérer déterminer si les données sont récentes
	$SQL="SELECT date_time 
	FROM analog2 
	WHERE date_time >= SUBTIME( NOW( ) ,  '1:00:00' )"; 
	//Envoie de la requete
	$RESULT = @mysqli_query($link,$SQL);
	// on récupère le nombre de résultat
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

	//Requete pour déterminer si la température monte ou descend
	$SQL="SELECT  ana1  AS TempMoy, AVG( ana2 ) AS HumMoy
	FROM  `analog2` 
	WHERE date_time <= NOW( ) 
	AND date_time >= SUBTIME( NOW( ) ,  '01:00:00' )"; 
	//Envoie de la requete
	$RESULT = @mysqli_query($link, $link,$SQL);
		
	// lecture du resultat de la requete
	$myrow=@mysqli_fetch_array($RESULT);
	//on récupère la température précédente
	$data_tempmoy[0] = $myrow["TempMoy"];
	$data_hummoy[0] = $myrow["HumMoy"];
	//on calcul la différence pour savoir si la T° monte ou descend
	$data_deltatemp = $data_tempint[0] - $data_tempmoy[0];
	$data_deltahum = $data_humint[0] - $data_hummoy[0];

	//si le delta T est positif
	if($data_deltatemp > 0){
		//la température monte
		$str_iconTi="flecheup.png";
	}else{
		//la température descend 
		$str_iconTi="flechedown.png";
	}

	//si le delta H est positif
	if($data_deltahum > 0){
		//la température monte
		$str_iconHi="flecheup.png";
	}else{
		//la température descend 
		$str_iconHi="flechedown.png";
	}

	//Requete pour déterminer la pente sur la dernière heure
	$SQL="SELECT ana1 
	FROM analog2
	WHERE date_time >= SUBTIME( NOW( ) ,  '01:00:00' )						
	ORDER BY date_time ASC
	LIMIT 0,1"; 
	// //Envoie de la requete
	$RESULT = @mysqli_query($link,$SQL);
	// lecture du resultat de la requete
	$myrow=@mysqli_fetch_array($RESULT); 
	//on récupère la température relevée il y a 1h
	$data_temp1h = $myrow["ana1"];
	//on calcul la pente sur la dernière heure
	$data_pentetemp = round($data_tempint[0] - $data_temp1h , 2);
	echo "data_temp1h : $data_temp1h  - data_tempint[0] : $data_tempint[0] -  pente : $data_pentetemp";

	// ------------------- Données Température extérieure  -------------------------------------

	//requete pour récupérer la dernière consommation instantanée
	$SQL="SELECT date_time, ana1, ana2 
	FROM analog1 
	ORDER BY date_time DESC
	LIMIT 0,1"; 
	//Envoie de la requete
	$RESULT = @mysqli_query($link, $link,$SQL);
	// lecture du resultat de la requete
	$myrow=@mysqli_fetch_array($RESULT); 
	//on récupère la dernière température relevée
	$data_dateext[0] = $myrow["date_time"];
	$data_tempext[0] = round($myrow["ana1"],1);
	$data_humext[0] = round($myrow["ana2"],1);

	//requete pour récupérer déterminer si les données sont récentes
	$SQL="SELECT date_time 
	FROM analog1 
	WHERE date_time >= SUBTIME( NOW( ) ,  '1:00:00' )";
	//Envoie de la requete
	$RESULT = mysqli_query($link,$SQL);	
	// on récupère le nombre de résultat
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
	
	

	//Requete pour déterminer si la température monte ou descend
	$SQL="SELECT  ana1 AS TempMoy, AVG( ana2 ) AS HumMoy
	FROM  `analog1` 
	WHERE date_time <= NOW( ) 
	AND date_time >= SUBTIME( NOW( ) ,  '01:00:00' )"; 
	//Envoie de la requete
	$RESULT = @mysqli_query($link,$SQL);
	
	// lecture du resultat de la requete
	$myrow=@mysqli_fetch_array($RESULT);
	//on récupère la température précédente
	$data_tempmoy[0] = $myrow["TempMoy"];
	$data_hummoy[0] = $myrow["HumMoy"];
	//on calcul la différence pour savoir si la T° monte ou descend
	$data_deltatemp = $data_tempext[0] - $data_tempmoy[0];
	$data_deltahum = $data_humext[0] - $data_hummoy[0];

	//si le delta T est positif
	if($data_deltatemp > 0){
		//la température monte
		$str_iconTe="flecheup.png";
	}else{
		//la température descend 
		$str_iconTe="flechedown.png";
	}

	//si le delta H est positif
	if($data_deltahum > 0){
		//la température monte
		$str_iconHe="flecheup.png";
	}else{
		//la température descend 
		$str_iconHe="flechedown.png";
	}

	//Requete pour déterminer la pente sur la dernière heure
	$SQL="SELECT ana1 
	FROM analog1
	WHERE date_time >= SUBTIME( NOW( ) ,  '01:00:00' )						
	ORDER BY date_time ASC
	LIMIT 0,1"; 
	// //Envoie de la requete
	$RESULT = @mysqli_query($link,$SQL);
	// lecture du resultat de la requete
	$myrow=@mysqli_fetch_array($RESULT); 
	//on récupère la température relevée il y a 1h
	$data_tempext1h = $myrow["ana1"];
	//on calcul la pente sur la dernière heure
	$data_pentetempext = round($data_tempext[0] - $data_tempext1h , 2);
	echo "data_tempext1h : $data_tempext1h  - data_tempext[0] : $data_tempext[0] -  pente : $data_pentetempext";


	// ------------------- Données Température du Puits Canadien  -------------------------------------
	// requete MySQL pour obtenir les données de la BDD
	//echo" $host, $login, $passe, $bdd \n";
	@mysqli_connect($host,$login,$passe,$bdd);
	//requete pour récupérer la dernière consommation instantanée
	$SQL="SELECT date_time, ana1, ana2 
	FROM analog3 
	WHERE date_time
	ORDER BY date_time DESC
	LIMIT 0,1"; 
	//Envoie de la requete
	$RESULT = @mysqli_query($link,$SQL);
	// lecture du resultat de la requete
	$myrow=@mysqli_fetch_array($RESULT); 
	//on récupère la dernière température relevée
	$data_datePC[0] = $myrow["date_time"];
	$data_tempPC[0] = round($myrow["ana1"],1);
	$data_humPC[0] = round($myrow["ana2"],1);
	
	
	//requete pour récupérer déterminer si les données sont récentes
	$SQL="SELECT date_time 
	FROM analog3 
	WHERE date_time >= SUBTIME( NOW( ) ,  '1:00:00' )"; 
	//Envoie de la requete
	$RESULT = @mysqli_query($link,$SQL);
	//on récupère le nombre de résultat
	//on récupère le nombre de résultat
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

	// ------------------- Récupère puissance pc actuelle -------------------------------------
	//Requete pour récupérer la valeur 15 minutes auparavent
	$SQL="SELECT date_time, puissance
	FROM puissance_pc
	WHERE date_time >= SUBTIME( NOW( ) ,  '00:15:00' ) 
	LIMIT 0,1						
	"; 
	// //Envoie de la requete
	$RESULT = @mysqli_query($link,$SQL);
	// lecture du resultat de la requete
	$myrow=@mysqli_fetch_array($RESULT); 
	//on récupère la dernière température relevée
	$puissance_pc = $myrow["puissance"];
	
	
	// ------------------- Récupère puissance pc actuelle -------------------------------------
	//Requete pour déterminer la valeur moyenne sur les dernières 20 minutes
	$SQL="SELECT puissance
	FROM chauffage_log
	ORDER BY date_time DESC 
	LIMIT 0,1						
	"; 
	// //Envoie de la requete
	$RESULT = @mysqli_query($link,$SQL);
	// lecture du resultat de la requete
	$myrow=@mysqli_fetch_array($RESULT); 
	//on récupère la dernière température relevée
	$puissance_poele = $myrow["puissance"];

	
	// ------------------- Récupère les infos des températures de la VMCDF -------------------------------------
	//Requete pour récuère les dernières valeurs
	$SQL="SELECT ana1 AS temp_air_neuf, ana2 AS temp_air_extrait, ana3 AS temp_air_repris, ana4 AS temp_air_ext
	FROM analog6
	ORDER BY date_time DESC 
	LIMIT 0,1						
	"; 
	// //Envoie de la requete
	$RESULT = @mysqli_query($link,$SQL);
	// lecture du resultat de la requete
	$myrow=@mysqli_fetch_array($RESULT); 
	//on récupère la dernière température relevée
	$vmc_temp_air_neuf = $myrow["temp_air_neuf"];	
	$vmc_temp_air_ext = $myrow["temp_air_ext"];
	$vmc_temp_air_repris = $myrow["temp_air_repris"];
	$vmc_temp_air_extrait = $myrow["temp_air_extrait"];
	
	//echo "<br> vmc_temp_air_neuf = $vmc_temp_air_neuf ; vmc_temp_air_ext=$vmc_temp_air_ext ; vmc_temp_air_repris=$vmc_temp_air_repris ; vmc_temp_air_extrait=$vmc_temp_air_extrait.<br>";
	
	// ------------------- Récupère la température de sortie de bouche ventilation-------------------------------------
	//Requete pour récuère les dernières valeurs
	$SQL="SELECT ana1 AS temp_air_bouche
	FROM analog4
	ORDER BY date_time DESC 
	LIMIT 0,1						
	"; 
	// //Envoie de la requete
	$RESULT = @mysqli_query($link,$SQL);
	// lecture du resultat de la requete
	$myrow=@mysqli_fetch_array($RESULT); 
	//on récupère la dernière température relevée
	$vmc_temp_air_bouche = $myrow["temp_air_bouche"];

	//requete pour récupérer déterminer si les données sont récentes
	$SQL="SELECT date_time 
	FROM analog4 
	WHERE date_time >= SUBTIME( NOW( ) ,  '1:00:00' )"; 
	//Envoie de la requete
	$RESULT = @mysqli_query($link,$SQL);
	//on récupère le nombre de résultat
	//on récupère le nombre de résultat
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

	// ------------------- Récupère la température de sortie du garage (capteur dans bypass PC) -------------------------------------
	//Requete pour récuère les dernières valeurs
	$SQL="SELECT ana1 AS temp_garage
	FROM VMC
	ORDER BY date_time DESC 
	LIMIT 0,1						
	"; 
	// //Envoie de la requete
	$RESULT = @mysqli_query($link,$SQL);
	// lecture du resultat de la requete
	$myrow=@mysqli_fetch_array($RESULT); 
	//on récupère la dernière température relevée
	$temp_garage = $myrow["temp_garage"];
	
	//requete pour récupérer déterminer si les données sont récentes
	$SQL="SELECT date_time 
	FROM VMC 
	WHERE date_time >= SUBTIME( NOW( ) ,  '1:00:00' )"; 
	//Envoie de la requete
	$RESULT = @mysqli_query($link,$SQL);
	//on récupère le nombre de résultat
	//on récupère le nombre de résultat
	$numrows = mysqli_num_rows($RESULT);
	echo"<br/>nb de ligne VMC = $numrows <br>";
	//si le nombre de mesures est >= 10 
	if($numrows >= 10){
		//on affiche une led verte
		$str_ledVMC="label-success";
	//si le nombre de mesures est compris entre 5 et 10
	}elseif($numrows >= 1){
		//on affiche une led orange
		$str_ledVMC="label-warning";
	}else{
		//on affiche une led rouge
		$str_ledVMC="label-important";
	}	
	
	// ------------------- Récupère la température des combles-------------------------------------
	//Requete pour récuère les dernières valeurs
	$SQL="SELECT ana1 AS temp_comble
	FROM analog5
	ORDER BY date_time DESC 
	LIMIT 0,1						
	"; 
	// //Envoie de la requete
	$RESULT = @mysqli_query($link,$SQL);
	// lecture du resultat de la requete
	$myrow=@mysqli_fetch_array($RESULT); 
	//on récupère la dernière température relevée
	$temp_comble = $myrow["temp_comble"];
	
	//requete pour récupérer déterminer si les données sont récentes
	$SQL="SELECT date_time 
	FROM analog5 
	WHERE date_time >= SUBTIME( NOW( ) ,  '1:00:00' )"; 
	//Envoie de la requete
	$RESULT = @mysqli_query($link,$SQL);
	//on récupère le nombre de résultat
	//on récupère le nombre de résultat
	$numrows = mysqli_num_rows($RESULT);
	echo"<br/>nb de ligne analog5 = $numrows <br>";
	//si le nombre de mesures est >= 10 
	if($numrows >= 2){
		//on affiche une led verte
		$str_ledCombles="label-success";
	//si le nombre de mesures est compris entre 5 et 10
	}elseif($numrows >= 1){
		//on affiche une led orange
		$str_ledCombles="label-warning";
	}else{
		//on affiche une led rouge
		$str_ledCombles="label-important";
	}		
	
	// ------------------- Récupère le flux solaire-------------------------------------
	//Requete pour récuère les dernières valeurs
	$SQL="SELECT ana1 as flux_solaire
	FROM pyranometre
	ORDER BY date_time DESC 
	LIMIT 0,1						
	"; 
	// //Envoie de la requete
	$RESULT = @mysqli_query($link,$SQL);
	// lecture du reisultat de la requete
	$myrow=@mysqli_fetch_array($RESULT); 
	//on récupère la dernière température relevée
	$flux_solaire = $myrow["flux_solaire"] * $Coef_V_to_W;
	echo"<br/>flux solaire = $flux_solaire W/m², coef=$Coef_V_to_W";
	
	// ------------------- Récupère de l'état du bypass puits canadien -------------------------------------
	//Requete pour récupérer la valeur 15 minutes auparavent
	$SQL="SELECT date_time, consigne
	FROM bypass_pc_log
	WHERE date_time >= SUBTIME( NOW( ) ,  '00:15:00' ) 
	LIMIT 0,1						
	"; 
	// //Envoie de la requete
	$RESULT = @mysqli_query($link,$SQL);
	// lecture du resultat de la requete
	$myrow=@mysqli_fetch_array($RESULT); 
	//on récupère la dernière température relevée
	$consigne_pc = $myrow["consigne"];
	

	// ------------------- On écrit toutes les données dans une seule table -------------------------------------
	//on met à  la table donnees_instant
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
		`icon_led_vmc` = '$str_ledVMC',
		`icon_led_combles` = '$str_ledCombles',
		`icon_led_airneuf` = '$str_ledAirneuf'
		WHERE `donnees_instant`.`id` =1;";
	//Envoie de la requete
	$RESULT = @mysqli_query($link,$SQL);
	// lecture du resultat de la requete
	if($myrow=@mysqli_fetch_array($RESULT)){
		echo"</br>Erreur lors de la sauvergarde en BDD";
	}else{
		echo"</br>Données sauvées dans la BDD";
	}
		

	//on quitte la BDD
	//mysqli_free_result($RESULT);
	mysqli_close($link);
?>
