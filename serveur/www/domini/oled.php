<?PHP
	include("infos/config.inc.php"); // on inclu le fichier de config
	@include("php/restore_donnees_instant.php");
	
	$form_envoi = "NOK";
	$donnees_OK = false;
	
	$colorRGB = isset($_POST['colorRGB']) ? $_POST['colorRGB'] : '';
	$textoled = isset($_POST['textoled']) ? $_POST['textoled'] : '';  
	$form_envoi = isset($_POST['envoi']) ? $_POST['envoi'] : '';
	
	// echo" colorRGB : $colorRGB<br>";
	// echo" textoled : $textoled<br>";
	// echo "bouton envoyer = $form_envoi<br>";
	
	$colorRGB = @ereg_replace("rgb\(","", $colorRGB);
	$colorRGB = @ereg_replace("\)","", $colorRGB);
	$RGB = explode("," , $colorRGB);
	// echo" R : $RGB[0]<br>";
	// echo" G : $RGB[1]<br>";
	// echo" B : $RGB[2]<br>";
	
	
	//si on vient de saisir des infos passées en paramètre de l'adresse
	if($form_envoi == "OK"){
	
		//on formate le message à transmettre au jeenode oled
		$message = '!LCD,'.$RGB[0].','.$RGB[1].','.$RGB[2].','. $textoled.'';
		
		// Connexion à la BDD
			$link = mysqli_connect($host,$login,$passe,$bdd);
	if (!$link) {
		die('Erreur de connexion (' . mysqli_connect_errno() . ') '
				. mysqli_connect_error());
	}
		
		// requete MySQL pour stocker les nouvelles valeurs
		$SQL="INSERT INTO `domotique`.`tx_msg_radio` (`id`, `date_time`, `message`) VALUES (NULL, NOW() , \"$message\");"; 

		//Execution de la requete
		mysqli_query($link,$SQL);
		mysqli_close($link);
		
		//message de confirmation
		//mis à 1 du flag pour afficher le message de confirmatio
		$donnees_OK = true;
		$form_envoi = "NOK";
		
		//reset des valeurs pour éviter de resaisir les infos en cas de refresh de la page
		$_POST['colorRGB'] = "";
		$_POST['textoled'] = "";
		$_POST['envoi'] = "";
		$message = 0;
		$RGB[] = "";
		
		// on lance le soft qui regénère le fichier CSV
		// exec("/var/www/domini/bin/emitter /dev/ttyUSB0"); // inutile, le logiciel pyReceiver.py gère l'envoi des messages spontanés

	}
	
?>
<!DOCTYPE html>
<html lang="fr">
	<head>
		<title>DoMini- Oled</title>
		<meta charset="UTF-8">
		<meta name="description" content="" />
		<meta http-equiv="Refresh" content="1024">
		<meta name="viewport" content="width=device-width,initial-scale=1">
		<link rel="shortcut icon" type="image/x-icon" href="./img/favicon.ico" />
		<!-- Les feuilles de styles -->
		<link href="css/bootstrap.css" rel="stylesheet">
		<link href="css/bootstrap-responsive.css" rel="stylesheet">
		<link href="css/bootstrap.colorpickersliders.css" rel="stylesheet" type="text/css" media="all">
		<link href="css/prettify.css" rel="stylesheet" type="text/css" media="all">

		<!-- JS files for Jquery -->
		<script type="text/javascript" src="js/jquery-latest.js"></script>
		<!-- JS files for bootstrap -->
		<script type="text/javascript" src="js/bootstrap.js"></script>
		<!-- JS files for bootstrap color picker -->
		<script type="text/javascript" src="js/prettify.js"></script>
		<script type="text/javascript" src="js/tinycolor.js"></script>		
		<script type="text/javascript" src="js/bootstrap.colorpickersliders.js"></script>		
	</head>

	<body>
		<!-- Part 1: Wrap all page content here -->
		<div id="wrap">
			<?PHP include("menu.html"); ?>

			<div class="container">				  
					<div class="span6">
						<form method="POST" action="oled.php" name="formulaire">
							<table class="table table-bordered table-striped ">
								<thead>
								  <tr>
									<th>Oled + Jeenode + webcam </th>
								  </tr>
								</thead>
								<tbody>
									<tr>
										<td>
											<div>
												<input type="text" class="form-control input-lg" id="focusedInput" placeholder="Taper votre texte ici" rows="4" name="textoled">
												<span class="help-block">Max 40 caractères</span>
											</div>								
										</td>
									</tr>	
									<tr>
										<td><input type="text" class="form-control" id="large" value="#ffffff" maxlength="40" name="colorRGB">
										
											<script>
												$("input#large").ColorPickerSliders({
													size: 'lg',
													placement: 'right',
													swatches: false,
													order: {
														hsl: 1
													}
												});
											</script>
										</td>
									</tr>	

									<tr>
										<td><input type="submit" name="btenvoyer" value="       OK       "/></td>
									</tr>							
								</tbody>
							 </table>
							 <input type="hidden" name="envoi" value="OK">
						 </form>
						<?PHP
							// $img = file_get_contents("http://92.245.144.185:50180/webcam/sejour.jpg"); 
							// on lui donne un nom pour éviter les conflits avec les autres images 
							// file_put_contents('./webcam/webcam'.$webcam.'.jpg', $img);
							//on affiche l'image !!!
							$filename_cam = "http://192.168.0.115:8080/shot.jpg";
							$img = file_get_contents($filename_cam);

								// echo ('<img src="'.$filename_cam.'?' .time(). '">');
								echo ('<img src="'. $filename_cam .'?' .time(). '">');
								echo date("Y-m-d H:i:s",  filemtime($img));
							
							// echo ('<img src="http://192.168.0.115:8080/shot.jpg?' .time(). '">');
						?>
					</div>		<!-- /span6 > -->				
			</div><!-- /container -->
		</div><!-- /wrap -->
  </body>
</html>