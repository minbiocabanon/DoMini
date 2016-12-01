<?php
	@include("../infos/config.inc.php"); // on inclu le fichier de config
	@include("../php/voletroulant.inc.php"); // fonction de conversion des état des VR

	$mode_vr = isset($_GET['mode_vr']) ? $_GET['mode_vr'] : '';
	$vr_bureau = isset($_GET['vr_bureau']) ? $_GET['vr_bureau'] : '';
	$vr_sejour = isset($_GET['vr_sejour']) ? $_GET['vr_sejour'] : '';
	$vr_chm = isset($_GET['vr_chm']) ? $_GET['vr_chm'] : '';
	$vr_chjf = isset($_GET['vr_chjf']) ? $_GET['vr_chjf'] : '';
	
	// echo"mode_vr = $mode_vr </br>";
	// echo"vr_bureau = $vr_bureau </br>";
	// echo"vr_sejour = $vr_sejour </br>";
	// echo"vr_chm = $vr_chm </br>";
	// echo"vr_chjf = $vr_chjf </br>";
	
	
	$mode = Convert_mode($mode_vr);
	if($vr_bureau == 'true') $etat_vr_bureau_bdd = 1; else if ($vr_bureau == 'false') $etat_vr_bureau_bdd = 0; else $etat_vr_bureau_bdd = 3;
	if($vr_sejour == 'true') $etat_vr_salon_bdd = 1; else if ($vr_sejour == 'false') $etat_vr_salon_bdd = 0; else $etat_vr_salon_bdd = 3;
	if($vr_chm == 'true') $etat_vr_chm_bdd = 1; else if ($vr_chm == 'false') $etat_vr_chm_bdd = 0; else $etat_vr_chm_bdd = 3;
	if($vr_chjf == 'true') $etat_vr_chjf_bdd = 1; else if ($vr_chjf == 'false') $etat_vr_chjf_bdd = 0; else $etat_vr_chjf_bdd = 3;

	
	// echo"mode = $mode </br>";
	// echo"etat_vr_bureau_bdd = $etat_vr_bureau_bdd </br>";
	// echo"etat_vr_salon_bdd = $etat_vr_salon_bdd </br>";
	// echo"etat_vr_chm_bdd = $etat_vr_chm_bdd </br>";
	// echo"etat_vr_chjf_bdd = $etat_vr_chjf_bdd </br>";
	
	// Connexion à la BDD
	$link = mysqli_connect($host,$login,$passe,$bdd);
	if (!$link) {
		die('Erreur de connexion (' . mysqli_connect_errno() . ') '
				. mysqli_connect_error());
	}
	// requete MySQL pour stocker les nouvelles valeurs
	$SQL="UPDATE `domotique`.`voletroulant_statut` SET `date_time` = NOW(),
			`mode` = '$mode',
			`bureau` = '$etat_vr_bureau_bdd',
			`salon` = '$etat_vr_salon_bdd',
			`chambreM` = '$etat_vr_chm_bdd',
			`chambreJF` = '$etat_vr_chjf_bdd'
			WHERE `voletroulant_statut`.`id` =1;"; 

	//Execution de la requete
	mysqli_query($link,$SQL) or die('Erreur SQL !'.$SQL.'<br>'.mysqli_error());
	mysqli_close($link);

	//si le mode est Manuel ou Tous ouverts ou Tous fermés
	//if($mode != 0){
		//on force le lancement de deux logiciels pour que l'action sur les VR soit immédiate
		// on lance le soft qui rempli la table d'état des VR
		exec("/var/www/domini/bin/gestion_vr",$result);
		//var_dump($result);
		// on force l'envoi des messages
		// exec("/var/www/domini/bin/emitter /dev/ttyUSB0",$result); // inutile, le logiciel pyReceiver.py gère l'envoi des messages spontanés
		//var_dump($result);
	//}
	
	//si le mode est Manuel
	if($mode == 3){
		//on met à jour la BDD	- on force l'état à IMMOBILE pour ne pas que les volets bougent alors qu'on est manuel		
		// Connexion à la BDD
		$link = mysqli_connect($host,$login,$passe,$bdd);
		if (!$link) {
			die('Erreur de connexion (' . mysqli_connect_errno() . ') '
					. mysqli_connect_error());
		}
		// requete MySQL pour stocker les nouvelles valeurs
		$SQL="UPDATE `domotique`.`voletroulant_statut` SET `date_time` = NOW(),
				`mode` = '$mode',
				`bureau` = '3',
				`salon` = '3',
				`chambreM` = '3',
				`chambreJF` = '3'
				WHERE `voletroulant_statut`.`id` =1;"; 

		//Execution de la requete
		mysqli_query($link,$SQL) or die('Erreur SQL !'.$SQL.'<br>'.mysqli_error());
		mysqli_close($link);	
	}

	include('../php/voletroulant_tab.php');
?>
						<!-- Incrustation d'un boite de confirmation, possibilité de la ferme avec une croix -->
						<div class="alert alert-success alert-dismissable">
						  <button type="button" class="close" data-dismiss="alert" aria-hidden="true">&times;</button>
						  <strong>OK!</strong> Consigne envoyée aux volets.
						</div>

						<table class="table table-bordered table-striped table-condensed">
							<thead>
							  <tr>
								<th colspan="4">Etat volets roulants</th>
							  </tr>
							</thead>
						<tbody>
							<tr>
								<th colspan="3">
									<select name="mode_vr" class="selectpicker span6" id="sel_etat_vr">
									  <option value="manuel" <?PHP if($mode_vr == 'Manuel'){echo 'selected';} ?>>Manuel</option>
									  <option value="ouverts" <?PHP if($mode_vr == 'Ouverts'){echo 'selected';} ?>>Tous ouverts</option>
									  <option value="fermes" <?PHP if($mode_vr == 'Fermés'){echo 'selected';} ?>>Tous Fermés</option>
									  <option value="automatique" <?PHP if($mode_vr == 'Automatique'){echo 'selected';} ?>>Auto</option>
									</select>
								</th>
								<th colspan="1">
									<div id="formulaire_VR" class="btn btn-primary btn-small">Appliquer</div>
								</th>
							</tr>	
						  <tr>
							<td class="tdhead">Bureau</td>
							<td class="tdhead">Séjour</td>
							<td class="tdhead">Ch. M</td>
							<td class="tdhead">Ch. JF</td>
						  </tr>
						  <tr>
							<td> 
								<div id="btn_vr_bureau">
									<?PHP
										if($mode_vr == 'Manuel') echo '<div id="create-button" name="btn_vr_bureau" class="btn btn-primary btn-small" type="button">Changer</div>';
										else {echo'<div  class="switch switch-small" id="vr_bureau" data-on="success" data-off="danger" data-on-label="Ouvert" data-off-label="Fermé"><input type="checkbox"  name="chk"'; echo affiche_etat($etat_vr_bureau, $mode_vr); echo'/></div>';}
									?>
								</div>
							</td>
							<td>
								<div id="btn_vr_sejour">
									<?PHP
										if($mode_vr == 'Manuel') echo '<div id="create-button" name="btn_vr_sejour" class="btn btn-primary btn-small" type="button">Changer</div>';
										else {echo'<div  class="switch switch-small" id="vr_sejour" data-on="success" data-off="danger" data-on-label="Ouvert" data-off-label="Fermé"><input type="checkbox"  name="chk"'; echo affiche_etat($etat_vr_salon, $mode_vr); echo'/></div>';}
									?>
								</div>
							</td>
							<td>
								<div id="btn_vr_chm">
									<?PHP
										if($mode_vr == 'Manuel') echo '<div id="create-button" name="btn_vr_chm" class="btn btn-primary btn-small" type="button">Changer</div>';
										else {echo'<div  class="switch switch-small" id="vr_chm" data-on="success" data-off="danger" data-on-label="Ouvert" data-off-label="Fermé"><input type="checkbox"  name="chk"'; echo affiche_etat($etat_vr_chm, $mode_vr); echo'/></div>';}
									?>
								</div>
							</td>
							<td>	
								<div id="btn_vr_chjf">
									<?PHP
										if($mode_vr == 'Manuel') echo '<div id="create-button" name="btn_vr_chjf" class="btn btn-primary btn-small" type="button">Changer</div>';
										else {echo'<div  class="switch switch-small" id="vr_chjf" data-on="success" data-off="danger" data-on-label="Ouvert" data-off-label="Fermé"><input type="checkbox"  name="chk"'; echo affiche_etat($etat_vr_chjf, $mode_vr); echo'/></div>';}
									?>
								</div>
							</td>
						  </tr>
						</tbody>
					  </table>