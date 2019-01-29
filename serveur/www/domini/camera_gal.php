<?PHP
	include("infos/config.inc.php"); // on inclu le fichier de config
	@include("php/restore_donnees_instant.php");
	
	//on récupère ce qui est passé en paramètre dans la barre d'adresse
	$galtoerase = isset($_POST['gal']) ? $_POST['gal'] : '';
	$nom_cam = $_GET['cam'];
	if($nom_cam == ""){
		// on choisi d'afficher le graphe avec toutes les températures (cas par défaut)
		$nom_cam = "garage";
	}	
	//echo 'nom_cam='.$nom_cam;
	// echo 'galtoerase='.$galtoerase;
	
	if($galtoerase == "garage" || $galtoerase == "jardin" || $galtoerase == "porche" || $galtoerase == "kiwi" || $galtoerase == "sejour"){
		$command = "rm /var/www/domini/webcam/".$galtoerase."/*.jpg";
		exec($command);
	}
?>
<!DOCTYPE html>
<html lang="fr">
	<head>
		<title>DoMini- Camera galerie</title>
		<meta charset="UTF-8">
		<meta name="description" content="" />
		<meta http-equiv="Refresh" content="600">
		<meta name="viewport" content="width=device-width,initial-scale=1">
		<link rel="shortcut icon" type="image/x-icon" href="./img/favicon.ico" />
		<!-- Les feuilles de styles -->
		<link href="css/bootstrap.css" rel="stylesheet">
		<link href="css/bootstrap-responsive.css" rel="stylesheet">
		<link href="css/bootstrapSwitch.css" rel="stylesheet">
		<link rel="stylesheet" type="text/css" href="webcam/resources/UberGallery.css" />
		<link rel="stylesheet" type="text/css" href="webcam/resources/colorbox/1/colorbox.css" />

		<!-- JS files for Jquery -->
		<script type="text/javascript" src="js/jquery-latest.js"></script>
		<!-- JS files for bootstrap -->
		<script type="text/javascript" src="js/bootstrap.js"></script>
		<!-- JS files for bootstrap switches-->
		<script type="text/javascript" src="js/bootstrapSwitch.js"></script>
		<!-- JS files for ubergalery-->
		<script type="text/javascript" src="webcam/resources/colorbox/jquery.colorbox.js"></script>
		<script type="text/javascript">
			$(document).ready(function(){
				$("a[rel='colorbox']").colorbox({maxWidth: "90%", maxHeight: "90%", opacity: ".5"});
			});
		</script>
	</head>

	<body>
		<!-- Part 1: Wrap all page content here -->
		<div id="wrap">
			<?PHP include("menu.html"); ?>

			<div class="container">
				<div class="navbar">
				  <div class="navbar-inner">
					<a class="brand" href="#">Cameras</a>
					<ul class="nav">
					  <li <?PHP if($nom_cam == "garage"){echo 'class="active"';} ?>><a href="camera_gal.php?cam=garage">garage</a></li>
					  <li <?PHP if($nom_cam == "jardin"){echo 'class="active"';} ?>><a href="camera_gal.php?cam=jardin">jardin</a></li>
					  <li <?PHP if($nom_cam == "porche"){echo 'class="active"';} ?>><a href="camera_gal.php?cam=porche">porche</a></li>
					  <li <?PHP if($nom_cam == "sejour"){echo 'class="sejour"';} ?>><a href="camera_gal.php?cam=sejour">sejour</a></li>
					</ul>
				  </div>
				</div>	
				<div class="row-fluid">
						<form method="POST" action="camera_gal.php<?PHP echo '?cam='.$nom_cam; ?>" name="formulaire">
							<input type="hidden" name="gal" value="<?PHP echo $nom_cam;?>">
							<input type="submit" name="bteffacer" value="Effacer"/>
						</form>
						<?php include_once('webcam/resources/UberGallery.php'); $rep = 'webcam/'.$nom_cam.'' ; $gallery = UberGallery::init()->createGallery($rep); ?>
				</div>
				
			</div><!-- /container -->
		</div><!-- /wrap -->
  </body>
</html>
