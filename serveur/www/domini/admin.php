<?PHP
	include("infos/config.inc.php"); // on inclu le fichier de config
	@include("php/restore_donnees_instant.php");
?>
<!DOCTYPE html>
<html lang="fr">
	<head>
		<title>DoMini- Admin</title>
		<meta charset="UTF-8">
		<meta name="description" content="" />
		<meta http-equiv="Refresh" content="600">
		<meta name="viewport" content="width=device-width,initial-scale=1">
		<link rel="shortcut icon" type="image/x-icon" href="../img/favicon.ico" />
		<!-- Les feuilles de styles -->
		<link href="css/bootstrap.css" rel="stylesheet">
		<link href="css/bootstrap-responsive.css" rel="stylesheet">
		<link href="css/log.css" rel="stylesheet">

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
				<div class="row-fluid">
					<div class="span6">
						<?php
						$search = array(
							'receiver:',
							'[INFO]',
						);
						$replace = array(
							'<span class="log-warning">receiver:</span>',
							'<span class="log-info">[INFO]</span>',
						);
						 
						$file='./log/user.log';
						$contenu=file_get_contents($file);
						$contenu=htmlentities($contenu);
						$contenu=str_replace($search, $replace, $contenu);
						echo '<br><br>Log Serveur : <br><div class="log-content">'.$contenu.'</div>';// le <pre> ne marchera plus ici.
						?>
					
					
					</div>		<!-- /span6 > -->
				</div>		<!-- /fluid > -->				
			</div><!-- /container -->
		</div><!-- /wrap -->
  </body>
</html>