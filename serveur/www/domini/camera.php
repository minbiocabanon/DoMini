<?PHP
	include("infos/config.inc.php"); // on inclu le fichier de config
	@include("php/restore_donnees_instant.php");
?>
<!DOCTYPE html>
<html lang="fr">
	<head>
		<title>DoMini- Camera</title>
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
					  <li><a href="camera_gal.php">Galerie</a></li>
					</ul>
				  </div>
				</div>
				
				<div class="row-fluid">
					<div class="span6">
						<div id="myCarousel" class="carousel slide">
							  <!-- Carousel items -->
							  <div class="carousel-inner">
								<div class="item active">
									<img src="./webcam/ext.jpg" alt="" >
									<div class="carousel-caption">
									  <h4>Jardin</h4>
									 <p><?PHP
										$filename_cam1 = "./webcam/ext.jpg";
										if(file_exists($filename_cam1)){
											echo date("Y-m-d H:i:s",  filemtime($filename_cam1));
										}
									  ?>
									  </p>
									</div>
								</div>
								<div class="item">
									<img src="./webcam/porche.jpg" alt="">
									<div class="carousel-caption">
									  <h4>Allee</h4>
									 <p><?PHP
										$filename_cam2 = './webcam/porche.jpg';
										if(file_exists($filename_cam2)){
											echo date("Y-m-d H:i:s",  filemtime($filename_cam2));
										}
									  ?>
									</p>
									</div>								
								</div>
								<div class="item">
									<img src="./webcam/sejour.jpg" alt="">
									<div class="carousel-caption">
									  <h4>Salon</h4>
									  <p>
									 <?PHP
										$filename_cam3 = './webcam/sejour.jpg';
										if(file_exists($filename_cam3)){
											echo date("Y-m-d H:i:s",  filemtime($filename_cam3));
										}
									  ?>
									  </p>
									</div>								
								</div>
								<div class="item">
									<img src="./webcam/test.jpg" alt="">
									<div class="carousel-caption">
									  <h4>Test</h4>
									  <p>
									 <?PHP
										$filename_cam4 = './webcam/test.jpg';
										if(file_exists($filename_cam4)){
											echo date("Y-m-d H:i:s",  filemtime($filename_cam4));
										}
									  ?>
									  </p>
									</div>								
								</div>								
							  </div>
							  
							  <!-- Carousel nav -->
							  <a class="carousel-control left" href="#myCarousel" data-slide="prev">&lsaquo;</a>
							  <a class="carousel-control right" href="#myCarousel" data-slide="next">&rsaquo;</a>
						</div>
					</div>		<!-- /span6 > -->
				</div>		<!-- /fluid > -->				
			</div><!-- /container -->
		</div><!-- /wrap -->
  </body>
</html>
