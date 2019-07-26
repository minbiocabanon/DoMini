<?PHP
	include("./infos/config.inc.php"); // on inclu le fichier de config
	@include("php/restore_donnees_instant.php");
	
	// ------------------- recuperation des donnees statut de l'alarme
	// requete MySQL pour obtenir les données de la BDD
	// echo" $host, $login, $passe, $bdd \n";
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
	
	$form_envoi = isset($_POST['envoi']) ? $_POST['envoi'] : 'NOK';
	$form_etat_alarme = isset($_POST['form_etat_alarme']) ? $_POST['form_etat_alarme'] : '';
	$form_alarm_mode = isset($_POST['alarm_mode']) ? $_POST['alarm_mode'] : '';

	// echo "form_envoi = $form_envoi<br>";
	// echo "form_etat_alarme = $form_etat_alarme<br>";
	// echo "form_alarm_mode = $form_alarm_mode<br>";
	
	if($form_envoi == "OK"){
			// conversion variable
		if( $form_etat_alarme == "on")
				$form_etat_alarme = "ON";
			else
				$form_etat_alarme = "OFF";
		//requete pour récupérer la dernière consommation instantanée
		$SQL="UPDATE `alarme_statut` SET `mode` = '".$form_etat_alarme."' WHERE `alarme_statut`.`id` = 1;"; 
		//Envoie de la requete
		$RESULT = @mysqli_query($link,$SQL);

		// pensez à ajouter dans la meme requete le mode  TOTAL ou PERIMETRIE
	}	
	
	
	//requete pour récupérer la dernière consommation instantanée
	$SQL="SELECT `id`,`mode` FROM `alarme_statut`"; 
	//Envoie de la requete
	$RESULT = @mysqli_query($link,$SQL);
	// //lecture du resultat de la requete
	$myrow = mysqli_fetch_array($RESULT); 
	//on récupère l'heure
	$etat_alarme = $myrow["mode"];

	// echo "<br>";
	// echo "etat_alarme = $etat_alarme<br>";
	
	// conversion variable
	if( $etat_alarme == "ON" || $etat_alarme == "on")
			$etat_alarme = "checked";
		else
			$etat_alarme = "";
				
	$alarm_mode = 'TOTAL';
	$etat_entrée = "glyphicon-ok";
	
	// echo "<br>";
	// echo "alarm_mode = $alarm_mode<br>";
	// echo "etat_alarme = $etat_alarme<br>";
	// echo "etat_entrée = $etat_entrée<br>";

	
?>
<!DOCTYPE html>
<html lang="fr">
	<head>
		<title>DoMini- Alarme</title>
		<meta charset="UTF-8">
		<meta name="description" content="" />
		<meta http-equiv="Refresh" content="600">
		<meta name="viewport" content="width=device-width,initial-scale=1">		
		<link rel="shortcut icon" type="image/x-icon" href="../img/favicon.ico" />
		<!-- Les feuilles de styles -->
		<link href="css/bootstrap.css" rel="stylesheet">
		<link href="css/bootstrap-responsive.css" rel="stylesheet">
		<link href="css/bootstrapSwitch.css" rel="stylesheet">

		<!-- JS files for Jquery -->
		<script type="text/javascript" src="js/jquery-latest.js"></script>
		<!-- JS files for bootstrap -->
		<script type="text/javascript" src="js/bootstrap.js"></script>
		<!-- JS files for bootstrap switches-->
		<script type="text/javascript" src="js/bootstrapSwitch.js"></script>
	</head>

	<body>
		<!-- Part 1: Wrap all page content here -->
		<div id="wrap">
			<!-- Menu -->
			<?PHP include("menu.html"); ?>

			<div class="container">				
				<div class="row-fluid">

					<div class="span12">
						<form method="POST" action="alarme.php" name="formulaire">
						<table class="table table-bordered table-striped table-condensed">
							
							<thead>
							  <tr>
								<th colspan="12">Alarme</th>
							  </tr>
							</thead>
							<tbody>
								<tr>
									<td colspan="6">
										<div  id="btn_etat_alarme" class="switch switch-medium" data-on="success" data-off="danger" data-on-label="ARMé" data-off-label="Ouvert">
											<input type="checkbox" name="form_etat_alarme" value="on" <?PHP echo $etat_alarme; ?> />
										</div>
									</td>
									<td colspan="6">
										<div >
											Statut : 
											<?PHP
												//$alarm_status = file_get_contents('http://192.168.0.122/jeedom/core/api/jeeApi.php?apikey=i790jec51h53tkdhq3uw&type=cmd&id=4');
												$alarm_status = 0;
												if($alarm_status == 0){
													echo '<span class="glyphicon glyphicon-ok" aria-hidden="true"></span>';
												}
												elseif ($alarm_status == 1) {
													echo '<span class="glyphicon glyphicon-remove" aria-hidden="true"></span>';
												}
												else{
													echo '<span class="glyphicon glyphicon-question-sign" aria-hidden="true"></span>';
												}
											?>	
										</div>
									</td>
								</tr>
								<tr>
									<td colspan="12">
										<select name="alarm_mode" class="selectpicker span6 show-menu-arrow" id="sel_etat_vr">
										  <option value="Total" <?PHP if($alarm_mode == 'TOTAL'){echo 'selected';} ?>>Total</option>
										  <option value="Périmétrie" <?PHP if($alarm_mode == 'PERIMETRIE'){echo 'selected';} ?>>Périmétrie</option>
										</select>
									</td>							
								</tr>
								<tr>
				
									</td>					
									<td colspan="12">
										<input class="btn btn-primary btn-small" id="formulaire_alarme" type="submit" value="Appliquer"/>
										<input type="hidden" name="envoi" value="OK">
								
								</tr>
							</tbody>
							
						</table>
						</form>	
					</div>	
				</div>		<!-- /fluid > -->
				
				<div class="row-fluid">
					<div class="span12">
						<table class="table table-bordered table-striped table-condensed">
							<thead>
							  <tr>
								<th colspan="12">Etat des capteurs</th>
							  </tr>
							</thead>
							<tbody>							
							<tr>
								<td class="tdhead">Entrée</td>
								<td class="tdhead">Cuisine</td>
								<td class="tdhead">Salon</td>
								<td class="tdhead">Bureau</td>
								<td class="tdhead">Chambres</td>
								<td class="tdhead">SdB</td>
								<td class="tdhead">WC</td>
								<td class="tdhead">Garage</td>
							</tr>
							 <tr>
								<td> <!-- Entrée -->
									<span class="glyphicon <?PHP echo $etat_entrée; ?>" aria-hidden="true"></span>
								</td>
								<td> <!-- Cuisine -->
									<span class="glyphicon glyphicon-ok" aria-hidden="true"></span>									
								</td>
								<td> <!-- Salon -->
									<span class="glyphicon glyphicon-ok" aria-hidden="true"></span>
								</td>
								<td> <!-- Bureau -->
									<span class="glyphicon glyphicon-ok" aria-hidden="true"></span>
								</td>
								<td> <!-- Chambres -->
									<span class="glyphicon glyphicon-ok" aria-hidden="true"></span>
								</td>
								<td> <!-- SdB -->
									<span class="glyphicon glyphicon-ok" aria-hidden="true"></span>
								</td>
								<td> <!-- WC-->
									<span class="glyphicon glyphicon-ok" aria-hidden="true"></span>
								</td>
								<td> <!-- Garage-->
									<span class="glyphicon glyphicon-ok" aria-hidden="true"></span>
								</td>
							</tr>
						</tbody>
					  </table>
					</div> <!-- /span12 > -->				
				</div>		<!-- /fluid > -->
			</div><!-- /container -->
		</div><!-- /wrap -->
  </body>
</html>