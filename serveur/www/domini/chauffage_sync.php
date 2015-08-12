<?php
	include("infos/config.inc.php"); // on inclu le fichier de config
	@include("php/restore_donnees_instant.php");
	
	function Convert_ssmenu($var){
		switch($var){
			case "SB":
				return 0;
			break;
			case "ON":
				return 1;
			break;
			case "TM":
				return 2;
			break;				
		}	
	}
	
	$msg_sent = false;
	
	// on récupère les variables du formulaire (retournée suite a l'appui sur le bouton OK)
	$form_ssmenu = isset($_POST['ssmenu']) ? $_POST['ssmenu'] : '';
	$form_envoi = isset($_POST['envoi']) ? $_POST['envoi'] : '';
	
	//si le formulaire a été rempli
	if($form_envoi == "OK"){
		//DEBUG
		// echo"form_envoi = $form_envoi<br>";
		// echo"form_ssmenu = $form_ssmenu<br>";
		
		// on converti le texte en valeur INTO
		$ssmenu = Convert_ssmenu($form_ssmenu);
		
		// echo"ssmenu = $ssmenu<br>";
		
		//on vérifie que la valeur récupérée est cohérente
		if($form_ssmenu == 0 || $form_ssmenu == 1 || $form_ssmenu == 2 ){

			// echo"Requete SQL<br>";
		
			// Connexion à la BDD
			@mysql_connect($host,$login,$passe) or die("Impossible de se connecter à la base de données");
			@mysql_select_db("$bdd") or die("Impossible de se connecter à la base de données");

			// requete MySQL pour stocker les nouvelles valeurs
			$SQL="INSERT INTO `domotique`.`tx_msg_radio` (`id` ,`date_time` ,`message` )VALUES (NULL , NOW(), '\$POL,2,$ssmenu');"; 

			//Execution de la requete
			mysql_query($SQL) or die('Erreur SQL !'.$SQL.'<br>'.mysql_error());
			mysql_close();
			
			// echo"Envoi msq radio<br>";
			// on force l'envoi des messages radio présent dans la pile
			// exec("/var/www/domini/bin/emitter /dev/ttyUSB0",$result);  // inutile, le logiciel pyReceiver.py gère l'envoi des messages spontanés
			//variable pour afficher un message de confirmation
			$msg_sent = true;
		}
	}	
?>
<!DOCTYPE html>
<html lang="fr">
<head>
	<title>DoMini - Synchronisation menu du poele</title>
	<meta charset="UTF-8">
	<meta http-equiv="Refresh" content="600">
	<meta name="viewport" content="width=device-width,initial-scale=1">
	<link rel="shortcut icon" type="image/x-icon" href="img/favicon.ico" />
	<!-- Les feuilles de styles -->
	<link href="css/bootstrap.css" rel="stylesheet">
	<link href="css/bootstrap-responsive.css" rel="stylesheet">
	<link href="css/bootstrapSwitch.css" rel="stylesheet">
		
	<!-- JS files for Jquery -->
	<script type="text/javascript" src="js/jquery-latest.js"></script>
	<!-- JS files for bootstrap -->
	<script type="text/javascript" src="js/bootstrap.js"></script>
</head>
	<body>
		<!-- Part 1: Wrap all page content here -->
		<div id="wrap">
			<?PHP include("menu.html"); ?>
		
			<div class="container">
				<div class="navbar">
				  <div class="navbar-inner">
					<a class="brand" href="#">Synchronisation état du poêle</a>
					<ul class="nav">
					</ul>
				  </div>
				</div>	
				<div  class="row-fluid" ID="formul" >
				<?PHP
					if($msg_sent == true){
						//Incrustation d'un boite de confirmation, possibilité de la ferme avec une croix 
						echo'				
							<div class="alert alert-success alert-dismissable">
							  <button type="button" class="close" data-dismiss="alert" aria-hidden="true">&times;</button>
							  <strong>OK!</strong> Consigne envoyée au poële.
							</div>';
						$msg_sent = false;
					}
				?>
				</div>
					<div class="span12 offset3">
						<FORM method="POST" name="formulaire">
							<ul style="list-style-type:none">
								<li>	Sélectionner l'état actuel du poele :<br>
										<select name="ssmenu">
											<option <?php if($form_ssmenu == "SB") { echo "selected=\"selected\""; } ?>>SB</option>
											<option <?php if($form_ssmenu == "ON") { echo "selected=\"selected\""; } ?>>ON</option>
											<option <?php if($form_ssmenu == "TM") { echo "selected=\"selected\""; } ?>>TM</option>
										</select>	
								</li>
							</ul>
									
							<input type="submit" name="btenvoyer" value="       Envoyer configuration       "/>
							<input type="hidden" name="envoi" value="OK">
						</form>
					</div>
				
				</div>		
			</div><!-- /container -->
		</div><!-- /wrap -->
	</body>
</html>