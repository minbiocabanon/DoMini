﻿<?php 
	
	//--------------------------------------------------
	//! \file    restore_donnees_instant.php
	//! \brief	cette page PHP est appelée par la page index pour récupérer rapidement les infos essentielles
	//! \date     2016-11
	//! \author   minbiocabanon
	//--------------------------------------------------
	
	// ------------------- recuperation de toutes les donnees instantanees
	// requete MySQL pour obtenir les données de la BDD
	//echo" $host, $login, $passe, $bdd \n";
	$link = mysqli_connect($host,$login,$passe,$bdd);
	if (!$link) {
		die('Erreur de connexion (' . mysqli_connect_errno() . ') '
				. mysqli_connect_error());
	}

	//echo 'Succès... ' . mysqli_get_host_info($link) . "\n";
	
	
	$SQL="SET lc_time_names = 'fr_FR'" ; // Pour afficher date en français dans MySql. 
	mysqli_query($link,$SQL);
	$SQL="SET NAMES UTF8" ; // Pour afficher date en français dans MySql. 
	mysqli_query($link,$SQL);
	
	//requete pour récupérer la dernière consommation instantanée
	$SQL="SELECT *, DATE_FORMAT(date_time, '%W %e %M %X - %T') as DATE FROM `donnees_instant`"; 
	//Envoie de la requete
	$RESULT = @mysqli_query($link,$SQL);
	// //lecture du resultat de la requete
	$myrow = mysqli_fetch_array($RESULT); 
	//on récupère l'heure
	$data_date[] = $myrow["DATE"];
	
	//on récupére les infos sur la T+H int
	$data_dateint[] = $myrow["date_int"];
	$data_tempint[] = round($myrow["T_int"], 1);
	$data_humint[] = round($myrow["H_int"], 1);
	$data_pentetemp[] = round($myrow["pente_T_int"], 1);
	
	//on récupére les infos sur la T+H ext
	$data_dateext[] = $myrow["date_ext"];
	$data_tempext[] = round($myrow["T_ext"], 1);
	$data_humext[] = round($myrow["H_ext"], 1);
	$data_pentetempext[] = round($myrow["pente_T_ext"], 1);
	
	//on récupére les infos sur la T+H PC
	$data_datepc[] = $myrow["date_pc"];
	$data_tempPC[] =  round($myrow["T_pc"], 1);
	$data_humPC[] =  round($myrow["H_pc"], 1);
	//$data_pentetempPC = $myrow["pente_T_pc"];
	$consigne_pc = $myrow["consigne_pc"];
	$puissance_pc = $myrow["puissance_pc"];
	$puissance_poele = $myrow["puissance_poele"];
	$puissance_poele .= "%";
	// flux solaire , dernière valeur enregistrée en watt/m²
	$flux_solaire = $myrow["flux_solaire"];
	
	//on récupère les infos de la vmcdf
	$vmc_temp_air_neuf = round($myrow["vmc_temp_air_neuf"], 1);
	$vmc_temp_air_ext = round($myrow["vmc_temp_air_ext"], 1);
	$vmc_temp_air_repris = round($myrow["vmc_temp_air_repris"], 1);
	$vmc_temp_air_extrait = round($myrow["vmc_temp_air_extrait"], 1);
	$vmc_temp_air_bouche =  round($myrow["vmc_temp_air_bouche"], 1);	
	
	//on récupère la consigne du PC
	$consigne_pc = $myrow["consigne_pc"];
	
	//on récupère la température des combles
	$temp_comble =  round($myrow["temp_comble"], 1);		

	//on récupère la température du garage
	$temp_garage =  round($myrow["temp_garage"], 1);		

	
	//on récupère la puissance consommé
	$data_teleinfo_date[] = $myrow["date_teleinfo"];
	$data_consojour[] = $myrow["conso_watt"];
	$data_tarif[] = $myrow["tarif_watt"];
	
	//recuperation du nom des images
	$str_iconTi = $myrow["icon_Ti"];
	$str_iconHi = $myrow["icon_Hi"];
    $str_iconTe = $myrow["icon_Te"];
    $str_iconHe = $myrow["icon_He"];
	$str_icon_lededf = $myrow["icon_led_edf"];
	$str_ledTi = $myrow["icon_led_Ti"];
	$str_ledTe = $myrow["icon_led_Te"];
	$str_ledPC = $myrow["icon_led_pc"];
	$str_ledVMC = $myrow["icon_led_vmc"];
	$str_ledCombles = $myrow["icon_led_combles"];
	$str_ledAirneuf = $myrow["icon_led_airneuf"];
	
	//on récupère l'état du bypass de la VMCDF
	$bypass_VMC = $myrow["bypass_vmcdf"];
	
	// on recupère les données de l'onduleur
	$ups_datetime = $myrow["ups_datetime"];
	$ups_status = $myrow["ups_status"];
	$ups_battery_charge = $myrow["ups_battery_charge"];
	$ups_battery_runtime = $myrow["ups_battery_runtime"];

	//requete pour récupérer la dernière valeur du niveau dans le réservoir
	$SQL="SELECT `nvg`  FROM `pellets_rsv` ORDER BY `date_time` DESC LIMIT 0,1"; 
	//Envoie de la requete
	$RESULT = @mysqli_query($link,$SQL);
	//lecture du resultat de la requete
	$myrow = @mysqli_fetch_array($RESULT); 
	//on récupère la mesure
	$pellets_nvg = $myrow["nvg"];
	$pellets_nvg = round(100 * ( 45 - $pellets_nvg )/45, 0);
	
	
	// ------------------- Récupération Etat et heure de début de chauffage -------------------------------------
	
	//requete pour récupérer les heures de début et fin de consigne de chauffage
	$SQL="SELECT heure_debut_consigne, heure_fin_consigne, temp_consigne
	FROM `chauffage_log`
	ORDER BY date_time DESC
	LIMIT 0,1"; 
	//Envoie de la requete
	$RESULT = @mysqli_query($link,$SQL);
	// lecture du resultat de la requete
	$myrow = @mysqli_fetch_array($RESULT); 
	//on récupère les heures de début et fin
	$h_demarr = $myrow["heure_debut_consigne"];
	$h_fin = $myrow["heure_fin_consigne"];
	$temp_consigne = $myrow["temp_consigne"];
	mysqli_free_result($RESULT);

	//requete pour récupérer les états et température du chauffage
	$SQL="SELECT etat, mode, consigne_utilisateur
	FROM `chauffage_statut`
	LIMIT 0,1"; 
	//Envoie de la requete
	$RESULT = @mysqli_query($link,$SQL);
	// lecture du resultat de la requete
	$myrow=@mysqli_fetch_array($RESULT); 
	//on récupère la dernière température relevée
	$etat[0] = $myrow["etat"];	
	$mode[0] = $myrow["mode"];	
	$consigne_utilisateur[0] = $myrow["consigne_utilisateur"];
	mysqli_free_result($RESULT);
	
	// ETAT_ERREUR, 	// 0
    // ETAT_OFF, 		// 1
	// ETAT_ON, 		// 2
	// ETAT_AUTO		// 3
	
	// MODE_ERREUR, 	// 0
	// MODE_ATTENTE,	// 1
	// MODE_ANTICIPE,	// 2
	// MODE_CHAUFFE		// 3 

	//on préparer les variables pour position les boutons dans le bon état dans la page principale (index)
	// on lit l'état du poele
	switch($etat[0]){
		//poele en erreur
		default:
		case 0 :
			$etat_chauffage_status = "";
			$mode_chauffage_status = "disabled";
			$puissance_poele = "Err.";
			$temp_consigne = "-";
			$h_demarr = "-";
			$h_fin = "-";
			break;
		//poele OFF
		case 1 :
			$etat_chauffage_status = "";
			$mode_chauffage_status = "";
			$puissance_poele = "OFF";
			$temp_consigne = "-";
			$h_demarr = "-";
			$h_fin = "-";
			break;	
		//poele ON
		case 2 :
			$etat_chauffage_status = "";
			$mode_chauffage_status = "checked";
			$puissance_poele = $consigne_utilisateur[0].'%';
			$temp_consigne = "-";
			$h_demarr = "-";
			$h_fin = "-";
			break;	
		//poele AUTO
		case 3 :
			$etat_chauffage_status = "checked";
			$mode_chauffage_status = "disabled";
			$temp_consigne .= "°C";
			
				switch($mode[0]){
				// MODE_ERREUR
				case 0:
					$etat_chauffage_status .= "";
					$mode_chauffage_status .= "";
					$puissance_poele = "Err.";
					break;
				// MODE_ATTENTE
				case 1:
					$etat_chauffage_status .= "";
					$mode_chauffage_status .= "";
					$puissance_poele = "Veille";
					break;
				// MODE_ANTICIPE
				case 2:
					$etat_chauffage_status .= "";
					$mode_chauffage_status .= " checked";
					break;	
				// MODE_CHAUFFE
				case 3:
					$etat_chauffage_status .= "";
					$mode_chauffage_status .= " checked";
					break;		
				}
			break;							
	}
	
	
		
	//on quitte la BDD
	mysqli_close($link);
?>
