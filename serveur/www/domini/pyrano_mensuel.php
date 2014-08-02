<?php 
	include("infos/config.inc.php"); // on inclu le fichier de config
	@include("php/restore_donnees_instant.php");

	$filename_cam1 = "./csv/pyrano_mensuel.csv";
	if(file_exists($filename_cam1)){
		$date_pyr =  date("Y-m-d H:i:s",  filemtime($filename_cam1));
	}
	
?>
<!DOCTYPE html>
<html lang="fr">
<head>

	<title>DoMini - Ensoleillement mensuel</title>
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
		<!-- 1a) Optional: add a theme file -->
		<script type="text/javascript" src="highstock/js/themes/gray.js"></script>
		<!-- Traduction française -->
		<script type="text/javascript" src="highstock/js/options.js"></script>

		<script type="text/javascript">
			$(function () {
				$('#container_graph').highcharts({
						chart: {
							renderTo: 'container_graph',
							type: 'column'
						},
						title: { text: null }
						,
						legend:
						{
							verticalAlign: 'top',
							floating : false,
							enabled: true
						},
						plotOptions: {
							column: {
								borderWidth: 0,
								shadow: false,
								// stacking: "normal"
							},
						},
						navigator: {
							enabled : false
						},
						series: [
							<?php
								$row = 1; 
								if (($handle = fopen("./csv/pyrano_mensuel.csv", "r")) !== FALSE) {
									while (($data = fgetcsv($handle, 256, ";")) !== FALSE) {
										$num = count($data);
										$c=0;
										echo "{shadow: true,";
										echo 'name: '. $data[$c] .', data: [';
										for ($c=1; $c < $num; $c++) {
											echo $data[$c].',';
										}
										echo "]},\n";
										$row++;
									}
									fclose($handle);
								}
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
							title: {text: 'Ensoleillement'},
							opposite : false,
							min: 0,
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
				<a class="brand" href="#">Flux solaire</a>
				<ul class="nav">
				  <li><a href="pyrano.php">Live</a></li>
				  <li><a href="pyrano_mensuel.php">Mensuel</a></li>
				</ul>
			  </div>
			</div>
			<div align="center">
				<div id="container_graph" style=" height: 360px; 	margin: 0 auto; 	background:url(img/spinner.gif); 	background-repeat: no-repeat;	background-position:center;	background-attachment:inherit"></div>
			</div>
			<div> Relevés à la date du <?PHP echo $date_pyr; ?></br></div>
				
		</div><!-- /container -->
	</div><!-- /wrap -->
</body>
</html>