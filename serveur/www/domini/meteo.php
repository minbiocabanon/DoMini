<?PHP
	include("./infos/config.inc.php"); // on inclu le fichier de config
	@include("php/restore_donnees_instant.php");
?>
<!DOCTYPE html>
<html lang="fr">
	<head>
		<title>DoMini- Meteo</title>
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
							<object type="application/x-shockwave-flash" data="http://swf.yowindow.com/yowidget3.swf" width="600" height="400">
								<param name="movie" value="http://swf.yowindow.com/yowidget3.swf"/>
								<param name="allowfullscreen" value="true"/>
								<param name="wmode" value="opaque"/>
								<param name="bgcolor" value="#FFFFFF"/>
								<param name="flashvars" value="location_id=gn:2972315&amp;location_name=cadours&amp;time_format=24&amp;unit_system=metric&amp;lang=fr&amp;background=#FFFFFF&amp;mini_temperature=true&amp;mini_time=true&amp;mini_locationBar=false&amp;mini_momentBar=true&amp;copyright_bar=false"/>							
							</object>
						</div>		
					</div>		<!-- /span6 > -->
				</div>		<!-- /fluid > -->
			</div><!-- /container -->
		</div><!-- /wrap -->
  </body>
</html>