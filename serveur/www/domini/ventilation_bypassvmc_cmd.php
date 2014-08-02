<?php
	include("infos/config.inc.php"); // on inclu le fichier de config
	@include("php/restore_donnees_instant.php");
	
	$msg_sent = false;
	
	// on récupère les variables du formulaire (retournée suite a l'appui sur le bouton OK)
	$form_cmdbypassvmc = isset($_POST['bypass_VMC']) ? $_POST['bypass_VMC'] : '';
	$form_envoi = isset($_POST['envoi']) ? $_POST['envoi'] : '';
	
		//DEBUG
		// echo"form_envoi = $form_envoi<br>";
		// echo"form_cmdbypassvmc = $form_cmdbypassvmc<br>";	
	
	//si le formulaire a été rempli
	if($form_envoi == "OK"){
		//DEBUG
		// echo"form_envoi = $form_envoi<br>";
		// echo"form_cmdbypassvmc = $form_cmdbypassvmc<br>";
				
		//on vérifie que la valeur récupérée est cohérente
		if($form_cmdbypassvmc == 0 || $form_cmdbypassvmc == 1 ){

			// echo"Requete SQL<br>";
		
			// Connexion à la BDD
			@mysql_connect($host,$login,$passe) or die("Impossible de se connecter à la base de données");
			@mysql_select_db("$bdd") or die("Impossible de se connecter à la base de données");

			// requete MySQL pour stocker les nouvelles valeurs
			$SQL="UPDATE `domotique`.`donnees_instant` SET `bypass_vmcdf` = '$form_cmdbypassvmc' LIMIT 1 ;"; 

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
	<title>DoMini - Etat bypass VMCDF</title>
	<meta charset="UTF-8">
	<meta name="viewport" content="width=device-width,initial-scale=1">
	<link rel="shortcut icon" type="image/x-icon" href="img/favicon.ico" />
	<!-- Les feuilles de styles -->
	<link href="css/bootstrap.css" rel="stylesheet">
	<link href="css/bootstrap-responsive.css" rel="stylesheet">
	<link href="css/bootstrap-slider.css" rel="stylesheet">
	<link href="css/bootstrapSwitch.css" rel="stylesheet">
		
	<!-- JS files for Jquery -->
	<script type="text/javascript" src="js/jquery-latest.js"></script>
	<!-- JS files for bootstrap -->
	<script type="text/javascript" src="js/bootstrap.js"></script>
	<script type="text/javascript" src="js/bootstrap-slider.js"></script>
	<!-- JS files for bootstrap switches-->
	<script type="text/javascript" src="js/bootstrapSwitch.js"></script>	
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
						<li class="active"><a href="ventilation_bypassvmc_cmd.php">Bypass VMCDF</a></li>
						<li><a href="ventilation_bypass_cmd.php">Bypass PC</a></li>
					</ul>
				  </div>
				</div>	
				<div class="alert alert-info">Bypass = Echangeur VMC bypassé, Croisé = Echangeur VMCDF actif</div>
				<div  class="row-fluid" ID="formul" >
					<?PHP
						if($msg_sent == true){
							//Incrustation d'un boite de confirmation, possibilité de la ferme avec une croix 
							echo'				
								<div class="alert alert-success alert-dismissable">
								  <button type="button" class="close" data-dismiss="alert" aria-hidden="true">&times;</button>
								  <strong>OK!</strong> Consigne mise à jour dans la BDD.
								</div>';
							$msg_sent = false;
						}
					?>
					<div class="span4 offset3">
						<FORM method="POST" name="formulaire">
							Sélectionner la valeur à transmettre au  bypass :</br></br>
									<select name="bypass_VMC" class="selectpicker span6 show-menu-arrow" id="sel_etat_vr">
									  <option value="1" <?PHP if($form_cmdbypassvmc == '1'){echo 'selected';} ?>>Bypass</option>
									  <option value="0" <?PHP if($form_cmdbypassvmc == '0'){echo 'selected';} ?>>Croise</option>
									</select>
							</br>
							<input class="btn btn-primary" type="submit" name="btenvoyer" value="Envoyer configuration"/>
							<input type="hidden" name="envoi" value="OK">
						</form>
					</div>
				
				</div>		
			</div><!-- /container -->
		</div><!-- /wrap -->
	</body>
</html>