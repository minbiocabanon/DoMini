<?PHP
	include("infos/config.inc.php"); // on inclu le fichier de config
	@include("php/restore_donnees_instant.php");
	
	$rendement = round(100 * ($vmc_temp_air_neuf - $vmc_temp_air_ext)/($vmc_temp_air_repris - $vmc_temp_air_ext),0);	
?>
<!DOCTYPE html>
<html lang="fr">
<head>
	<title>DoMini - Flux de la ventilation</title>
		<meta charset="UTF-8">
		<meta http-equiv="Refresh" content="600">
		<meta name="viewport" content="width=device-width,initial-scale=1">
		<link rel="shortcut icon" type="image/x-icon" href="img/favicon.ico" />
		<!-- Les feuilles de styles -->
		<link href="css/bootstrap.css" rel="stylesheet">
		<link href="css/bootstrap-responsive.css" rel="stylesheet">
		<link href="css/bootstrapSwitch.css" rel="stylesheet">
		<link href="css/ventilation.css" rel="stylesheet">
		
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
					<a class="brand" href="#">Ventilation</a>
					<ul class="nav">
						<li class="active"><a href="ventilation_flux.php">Flux</a></li>
						<li><a href="ventilation.php">Puissance</a></li>
						<li><a href="ventilation_bypasspc.php">Bypass</a></li>
						<li><a href="ventilation_air_neuf.php">Air neuf</a></li>
					</ul>
				  </div>
				</div>
			
				<div id="frame_ventilation">
					<div id="image_ventilation">
						<img src="img/plan_ventilation.jpg" alt="plan ventilation" ></img>
					</div>
					<div id="image_bypass">
						<img src="img/<?php if($consigne_pc == 100){echo "bypass_pc_ext.jpg";}else{echo "bypass_pc_pc.jpg";} ?>" alt="bypass_pc"></img>	
					</div>
					<div id="image_bypass_vmc">
						<?php if($bypass_VMC == 1){echo "<img src=\"img/vmcdf_bypass.jpg\" alt=\"bypass_VMC_DF\"></img> ";} ?>
					</div>					
					<div id="temp_ext"><?php echo $data_tempext[0];?>°C</div>
					<div id="text_temp_garage">garage</div>
					<div id="temp_garage"><?php echo $temp_garage;?>°C</div>
					<div id="text_temp_int">Temp. int.</div>
					<div id="temp_int"><?php echo $data_tempint[0];?>°C</div>
					<div id="temp_pc"><?php echo $data_tempPC[0];?>°C</div>
					<div <?php if($bypass_VMC == 1){echo "id=\"temp_air_neuf_bypass\"";} else {echo "id=\"temp_air_neuf\"";} ?> ><?php echo $vmc_temp_air_neuf;?>°C</div>
					<div id="temp_air_ext"><?php echo $vmc_temp_air_ext;?>°C</div>
					<div <?php if($bypass_VMC == 1){echo "id=\"temp_air_repris_bypass\"";} else {echo "id=\"temp_air_repris\"";} ?> ><?php echo $vmc_temp_air_repris;?>°C</div>
					<div id="temp_air_extrait" ><?php echo $vmc_temp_air_extrait;?>°C</div>
					<div <?php if($bypass_VMC == 1){echo "id=\"text_temp_air_bouche_bypass\"";} else {echo "id=\"text_temp_air_bouche\"";} ?> >bouche</div>
					<div <?php if($bypass_VMC == 1){echo "id=\"temp_air_bouche_bypass\"";} else {echo "id=\"temp_air_bouche\"";} ?> ><?php echo $vmc_temp_air_bouche;?>°C</div>
					<div id="temp_comble"><?php echo $temp_comble;?>°C</div>
					<div id="text_temp_comble">combles</div>
					<?PHP 
						if($bypass_VMC == 0){
							echo '	<div id="rendement_vmcdf"> '.$rendement.' >%</div> ';
						}
					?>
				</div>
			
         			
			</div><!-- /container -->
		</div><!-- /wrap -->
	</body>
</html>