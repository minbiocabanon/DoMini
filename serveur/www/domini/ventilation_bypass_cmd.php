<?php
	include("infos/config.inc.php"); // on inclu le fichier de config
	@include("php/restore_donnees_instant.php");
	
	$msg_sent = false;
	
	// on récupère les variables du formulaire (retournée suite a l'appui sur le bouton OK)
	$form_cmdbypass = isset($_POST['ssmenu']) ? $_POST['ssmenu'] : '';
	//$form_cmdbypass = $_GET['ssmenu'];
	$form_envoi = isset($_POST['envoi']) ? $_POST['envoi'] : '';

	// echo"form_envoi = $form_envoi<br>";
	// echo"form_cmdbypass = $form_cmdbypass<br>";
	
	//si le formulaire a été rempli
	if($form_envoi == "OK"){
	// if(true){
		//DEBUG
		// echo"form_envoi = $form_envoi<br>";
		// echo"form_cmdbypass = $form_cmdbypass<br>";
				
		//on vérifie que la valeur récupérée est cohérente
		if($form_cmdbypass >= 0 && $form_cmdbypass <= 100 ){

			// echo"Requete SQL<br>";
		
			// Connexion à la BDD
			@mysql_connect($host,$login,$passe) or die("Impossible de se connecter à la base de données");
			@mysql_select_db("$bdd") or die("Impossible de se connecter à la base de données");

			// requete MySQL pour stocker les nouvelles valeurs
			$SQL="INSERT INTO `domotique`.`tx_msg_radio` (`id` ,`date_time` ,`message` )VALUES (NULL , NOW(), '!BPC,$form_cmdbypass,0');"; 

			//Execution de la requete
			mysql_query($SQL) or die('Erreur SQL !'.$SQL.'<br>'.mysql_error());
			mysql_close();
			
			//variable pour afficher un message de confirmation
			$msg_sent = true;
		}
	}	
?>
<!DOCTYPE html>
<html lang="fr">
<head>
	<title>DoMini - Commande manuelle du bypass (pour test)</title>
	<meta charset="UTF-8">
	<meta name="viewport" content="width=device-width,initial-scale=1">
	<link rel="shortcut icon" type="image/x-icon" href="img/favicon.ico" />
	<!-- Les feuilles de styles -->
	<link href="css/bootstrap.css" rel="stylesheet">
	<link href="css/bootstrap-responsive.css" rel="stylesheet">
		
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
					<a class="brand" href="#">VMCDF / PC</a>
					<ul class="nav">
						<li ><a href="ventilation_bypassvmc_cmd.php">Bypass VMCDF</a></li>
						<li class="active"><a href="ventilation_bypass_cmd.php">Bypass PC</a></li>
					</ul>
				  </div>
				</div>	
				<div class="alert alert-info">0 = bypass en position PC, 100 = bypass en position air extérieur</div>
				<div  class="row-fluid" ID="formul" >
				<?PHP
					if($msg_sent == true){
						//Incrustation d'un boite de confirmation, possibilité de la ferme avec une croix 
						echo'				
							<div class="alert alert-success alert-dismissable">
							  <button type="button" class="close" data-dismiss="alert" aria-hidden="true">&times;</button>
							  <strong>OK!</strong> Consigne envoyée au bypass.
							</div>';
						$msg_sent = false;
					}
				?>
					<div class="span12 offset3">
						<FORM method="POST" action="ventilation_bypass_cmd.php" name="formulaire">
							Sélectionner la valeur à transmettre au  bypass :</br></br>
							<input name="ssmenu" class="span5" type="text" maxlength="3">
							</br>							
							<input class="btn btn-primary" type="submit" name="btenvoyer" value="Envoyer configuration">
							<input type="hidden" name="envoi" value="OK">
						</form>
					</div>
				</div>		
			</div><!-- /container -->
		</div><!-- /wrap -->
	</body>
</html>