<?php 
	include("infos/config.inc.php"); // on inclu le fichier de config
	@include("php/restore_donnees_instant.php");	
?>
<!DOCTYPE html>
<html lang="fr">
<head>

	<title>DoMini - Températures extérieures, stats mensuelles </title>
	<meta charset="UTF-8">
		<meta http-equiv="Refresh" content="600">
		<meta name="viewport" content="width=device-width,initial-scale=1">
		<link rel="shortcut icon" type="image/x-icon" href="img/favicon.ico" />
		<!-- Les feuilles de styles -->
		<link href="css/bootstrap.css" rel="stylesheet">
		<link href="css/bootstrap-responsive.css" rel="stylesheet">
		<link href="css/bootstrapSwitch.css" rel="stylesheet">
		
		<!-- JS files for Jquery -->
		<script type="text/javascript" src="js/jquery-latest.js"></script>
		<!-- JS files for bootstrap -->
		<script type="text/javascript" src="js/bootstrap.js"></script>
		
		<!-- JS files for highstock (graphs) -->
		<script type="text/javascript" src="highstock/js/highstock.js"></script>
		<script type="text/javascript" src="highstock/js/highcharts-more.js"></script>
		<!-- 1a) Optional: add a theme file -->
		<script type="text/javascript" src="highstock/js/themes/gray.js"></script>
		<!-- Traduction française -->
		<script type="text/javascript" src="highstock/js/options.js"></script>

		<script type="text/javascript">
			$(function () {
				$('#container_graph').highcharts({
						chart: {
							renderTo: 'container_graph',
						},
						title: { text: null }
						,
						// colors: [
						   // '#2f7ed8', // 2011
						   // '#c42525', // 2012 
						   // '#8bbc21', // 2013 
						   // '#9FF000', // 2014
						   // '#2f7ed8', // 2011 moyenne 
						   // '#c42525', // 2012 moyenne 
						   // '#8bbc21', // 2013 moyenne 
						   // '#9FF000'  // 2014 moyenne
						// ],
						legend:
						{
							verticalAlign: 'top',
							floating : false,
							enabled: true
						},
						plotOptions: {
							arearange:{
								states: {
									hover: {
										enabled: false
									}
								}
							},
						},
						navigator: {
							enabled : false
						},
						tooltip: {
							shared: true,
							crosshairs: true
						},
						series: [
							<?php
								//on récupére l'année en cours
								$annee = date('Y');
									$link = mysqli_connect($host,$login,$passe,$bdd);
	if (!$link) {
		die('Erreur de connexion (' . mysqli_connect_errno() . ') '
				. mysqli_connect_error());
	}
								
								
								//boucle pour récupérer MIN et MAX
								for($i = 2011; $i <= $annee ; $i++){
									//requete pour récupérer la température min/max par mois et pour chaque année
									$SQL="SELECT min( `ana1` ), max( `ana1` )
											FROM `analog1` 
											WHERE year( `date_time` ) = $i
											GROUP BY month( `date_time` ) 
											ORDER BY month( `date_time` ) ASC
											LIMIT 0 , 30"; 
									
									// on execute la requete
									$RESULT = @mysqli_query($link, $SQL);	
									$comma = '';
									//on écrit les champs pour déclarer les série
									echo "{shadow: true, type: 'areasplinerange', fillOpacity : 0.25, ";
									echo 'name: '. $i .', data: [';
									// pour chaque ligne (une ligne par mois)
									while($row = mysqli_fetch_assoc($RESULT)) {
										// on fabrique une chaine au format [xx , yy], 
										echo $comma .'[';
										$commav = ',';
										foreach($row as $value) {
											echo $value.$commav;
											$commav = '';
										}
										$comma = ',';
										echo "]";
									}
									$comma = '';
									echo "]},\n";
								}
								// on libère la mémoire
								mysqli_free_result($RESULT) ;
											
								//boucle pour récupérer la valeur MOY
								for($i = 2011; $i <= $annee ; $i++){
									//requete pour récupérer la température moy par mois et pour chaque année
									$SQL="SELECT avg( `ana1` )
											FROM `analog1` 
											WHERE year( `date_time` ) = $i
											GROUP BY month( `date_time` ) 
											ORDER BY month( `date_time` ) ASC
											LIMIT 0 , 30"; 
									
									// on execute la requete
									$RESULT = @mysqli_query($link, $SQL);	
									$comma = '';
									//on écrit les champs pour déclarer les série
									echo "{shadow: true, type: 'spline', ";
									echo 'name: \''. $i .'-moy\', data: [';
									while($row = mysqli_fetch_assoc($RESULT)) {
										// on fabrique une chaine au format [zz], 
										echo $comma;
										
										foreach($row as $value) {
											echo $value;
										}
										$comma = ', ';
									}
									$comma = '';
									echo "]},\n";
								}
								// on libère la mémoire
								mysqli_free_result($RESULT) ;								
								// on ferme la connexion à mysql
								mysqli_close($link);
							?>
						
						],
						xAxis: {
								categories: [
									'Jan',
									'Feb',
									'Mar',
									'Apr',
									'May',
									'Jun',
									'Jul',
									'Aug',
									'Sep',
									'Oct',
									'Nov',
									'Dec'
								]
						},
						yAxis: [{
							title: {text: 'Températures Extérieures °C'},
							opposite : false,
							lineWidth: 1,
						}],
						
						load: function() {
									document.getElementById('container_graph').style.background = 'none';
								}
					});
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
					<a class="brand" href="#">Températures</a>
					<ul class="nav">
					  <li><a href="temperature_all.php">Tout</a></li>
					  <li><a href="temperature.php?nom_courbe=temp_int">Intérieure</a></li>
					  <li><a href="temperature.php?nom_courbe=temp_ext">Extérieure</a></li>
					  <li><a href="temperature.php?nom_courbe=temp_pc">Puits Canadien</a></li>
					  <li class="active"><a href="temperature_stat.php">Stat. mensuelles</a></li>
					</ul>
				  </div>
				</div>
			
				<div align="center">
					<div id="container_graph" style=" height: 360px; 	margin: 0 auto; 	background:url(img/spinner.gif); 	background-repeat: no-repeat;	background-position:center;	background-attachment:inherit"></div>
				</div>
			
         			
			</div><!-- /container -->
	</div><!-- /wrap -->
</body>
</html>
