<?PHP
	include("infos/config.inc.php"); // on inclu le fichier de config
	@include("php/restore_donnees_instant.php");
	
	$galtoerase = isset($_POST['gal']) ? $_POST['gal'] : '';
	
	// echo 'galtoerase='.$galtoerase;
	
	if($galtoerase == "garage" || $galtoerase == "jardin" || $galtoerase == "porche"){
	
		switch($galtoerase){
			case "garage":
				// echo "<br>garage exec";
				exec("rm /var/www/domini/webcam/garage/*.jpg");
				break;
			case "jardin":
				// echo "<br>jardin exec";
				exec("rm /var/www/domini/webcam/ext/*.jpg");
				break;
			case "porche":
				// echo "<br>porche exec";
				exec("rm /var/www/domini/webcam/porche/*.jpg");
				break;
			default:
				break;		
		}
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
					  <li><a href="camera.php">Live</a></li>
					  <li class="active"><a href="camera_gal.php">Galerie</a></li>
					</ul>
				  </div>
				</div>	
				<div class="row-fluid">
					<h2>GARAGE</h2>
						<form method="POST" action="camera_gal.php" name="formulaire">
							<input type="hidden" name="gal" value="garage">
							<input type="submit" name="bteffacer" value="Effacer"/>
						</form>
						<?php include_once('webcam/resources/UberGallery.php'); $gallery = UberGallery::init()->createGallery('webcam/garage'); ?>
				</div>				
				<div class="row-fluid">			
					<h2>JARDIN</h2>
					<form method="POST" action="camera_gal.php" name="formulaire">
						<input type="hidden" name="gal" value="jardin">
						<input type="submit" name="bteffacer" value="Effacer"/>
					</form>
					<?php include_once('webcam/resources/UberGallery.php'); $gallery = UberGallery::init()->createGallery('webcam/ext'); ?>
				</div>
				<div class="row-fluid">
					<h2>PORCHE</h2>				
					<form method="POST" action="camera_gal.php" name="formulaire">
						<input type="hidden" name="gal" value="porche">
						<input type="submit" name="bteffacer" value="Effacer"/>
					</form>					
					<?php include_once('webcam/resources/UberGallery.php'); $gallery = UberGallery::init()->createGallery('webcam/porche'); ?>			
				</div>
			</div><!-- /container -->
		</div><!-- /wrap -->
  </body>
</html>
