<?PHP
	include("infos/config.inc.php"); // on inclu le fichier de config
	@include("php/restore_donnees_instant.php");
	
		// on force le refresh des cameras
		exec("script/webcam_garage.sh");
?>
<!DOCTYPE html>
<html lang="fr">
	<head>
		<title>DoMini- Camera pepette</title>
		<meta charset="UTF-8">
		<meta name="description" content="" />
		<meta http-equiv="Refresh" content="600">
		<meta name="viewport" content="width=device-width,initial-scale=1">
		<link rel="shortcut icon" type="image/x-icon" href="./img/favicon.ico" />
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
		<!-- JS files for bootstrap carousel-->
		<script type="text/javascript" src="js/bootstrap-carousel.js"></script>
		
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
					  <li class="active"><a href="camera.php">Live</a></li>
					</ul>
				  </div>
				</div>
				
				<div class="row-fluid">
					<div class="span6">
						<img src="webcam/pepette.jpg?<?PHP echo time()?>" alt="garage" width="800"><br>
					</div>		<!-- /span6 > -->
				</div>		<!-- /fluid > -->				
			</div><!-- /container -->
		</div><!-- /wrap -->
  </body>
</html>
