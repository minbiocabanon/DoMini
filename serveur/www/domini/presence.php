<?PHP
	include("infos/config.inc.php"); // on inclu le fichier de config
	@include("php/restore_donnees_instant.php");
	// on récupère les variables du formulaire (retournée suite a l'appui sur le bouton OK)
	$statut = isset($_GET['statut']) ? $_GET['statut'] : '';
	
	if($statut == "ALARM_ON"){
		$img_lock="cadenas-red.png";
		$param = "ALARM_OFF";
		$etat = "ALARME ON";
		$msg_class = "alert-success";
		
		//on change l'état de la vidéo surveillance
		// Connexion à la BDD
			$link = mysqli_connect($host,$login,$passe,$bdd);
	if (!$link) {
		die('Erreur de connexion (' . mysqli_connect_errno() . ') '
				. mysqli_connect_error());
	}
		
		$SQL="UPDATE `domotique`.`mode_domotique` SET `videosurveillance` = '1' WHERE `mode_domotique`.`id` =1;"; 
		//Execution de la requete
		mysqli_query($link,$SQL);
		
		$SQL="UPDATE `domotique`.`mode_domotique` SET `mode` = 'AUTO' WHERE `mode_domotique`.`id` =1;"; 	
		//Execution de la requete
		mysqli_query($link,$SQL);		
		mysqli_close($link);	
		
		//ON ACTIVE LA DETECTION DE MOUVEMENT DE LA CAMERA (CGI VAPIX)
		file_get_contents('http://root:melisse!@192.168.0.116/axis-cgi/operator/param.cgi?action=update&Event.E0.Enabled=yes');
		file_get_contents('http://root:melisse!@192.168.0.117/axis-cgi/operator/param.cgi?action=update&Event.E0.Enabled=yes');
	}
	else if ($statut == "ALARM_OFF"){
		$img_lock="cadenas-green.png";
		$param = "ALARM_ON";
		$etat = "ALARME OFF";
		$msg_class = "alert-danger";
		//on change l'état de la vidéo surveillance
		// Connexion à la BDD
			$link = mysqli_connect($host,$login,$passe,$bdd);
	if (!$link) {
		die('Erreur de connexion (' . mysqli_connect_errno() . ') '
				. mysqli_connect_error());
	}
		

		$SQL="UPDATE `domotique`.`mode_domotique` SET `videosurveillance` = '0' WHERE `mode_domotique`.`id` =1;"; 
	
		// on execute la requete
		mysqli_query($link,$SQL);
		
		//Execution de la requete
		mysqli_close($link);
		
		//ON DESACTIVE LA DETECTION DE MOUVEMENT DE LA CAMERA (CGI VAPIX)
		file_get_contents('http://root:melisse!@192.168.0.116/axis-cgi/operator/param.cgi?action=update&Event.E0.Enabled=no');
		file_get_contents('http://root:melisse!@192.168.0.117/axis-cgi/operator/param.cgi?action=update&Event.E0.Enabled=no');
		
	}
	//si aucun paramètre
	else{
		// Connexion à la BDD
			$link = mysqli_connect($host,$login,$passe,$bdd);
	if (!$link) {
		die('Erreur de connexion (' . mysqli_connect_errno() . ') '
				. mysqli_connect_error());
	}
		
		//on récupère l'état de la domotique
		$SQL="	SELECT * FROM `mode_domotique` LIMIT 0 , 1"; 
	
		// on execute la requete
		$RESULT = @mysqli_query($link, $SQL);
		
		// lecture du resultat de la requete
		$myrow=@mysqli_fetch_array($RESULT);
		//on récupère la température précédente
		$mode = $myrow["mode"];
		$alarme = $myrow["alarme"];
		$videosurveillance = $myrow["videosurveillance"];
		
		//Execution de la requete
		mysqli_close($link);


		if($alarme == "0"){
			$param = "ALARM_ON";
			$etat = "ALARME OFF";
			$msg_class = "alert-danger";
			$img_lock="cadenas-green.png";
		}
		else{
			$param = "ALARM_OFF";
			$etat = "ALARME ON";
			$msg_class = "alert-success";
			$img_lock="cadenas-red.png";
		}
	}
?>
<!DOCTYPE html>
<html lang="fr">
	<head>
		<title>DoMini- Presence</title>
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
						 <div class="center" >
							<div class="alert <?php echo $msg_class; ?>"><?php echo $etat; ?></div>
							<a href="presence.php?statut=<?php echo $param; ?>"><img src="img/<?php echo $img_lock; ?>" border="0" width="300"/></a>
						</div>		
					</div>		<!-- /span6 > -->
				</div>		<!-- /fluid > -->
			</div><!-- /container -->
		</div><!-- /wrap -->
  </body>
</html>