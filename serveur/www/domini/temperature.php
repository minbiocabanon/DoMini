<?PHP
	include("infos/config.inc.php"); // on inclu le fichier de config
	@include("php/restore_donnees_instant.php");
	
	//on récupère ce qui est passé en paramètre dans la barre d'adresse
	$nom_courbe = $_GET['nom_courbe'];
	// si pas de nom passé en paramètre
	if($nom_courbe == ""){
		// on choisi d'afficher le graphe avec toutes les températures (cas par défaut)
		$nom_courbe = "temp_int";
	}
?>
<!DOCTYPE html>
<html lang="fr">
<head>
	<title>DoMini - Temperatures</title>
	<meta charset="UTF-8" >
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
			$(function() {
				$.get('./csv/' + '<?php echo $nom_courbe ?>' + '.csv', function(csv, state, xhr) {
					
					// inconsistency
					if (typeof csv != 'string') {
						csv = xhr.responseText;
					} 
					
					// parse the CSV data
					var ptrose = [], humid = [], temp = [], header, comment = /^#/, x;
					
					$.each(csv.split('\n'), function(i, line){
						if (!comment.test(line)) {
							if (!header) {
								header = line;
							
							} else if (line.length) {
								var point = line.split(';'), 
									date = point[0].split('-'),
									time = point[1].split(':');
								
								x = Date.UTC(date[2], date[1] - 1, date[0], time[0], time[1]);
								
								temp.push([x, parseFloat(point[2])]); // température
								humid.push([x, parseFloat(point[3])]); // humidité
								ptrose.push([	x, parseFloat(point[4])]); // point de rosée

							}
						}
					});
					
					// create the chart
					chart = new Highcharts.StockChart({
						chart: {
							renderTo: 'container_graph',
							zoomType: 'x'
						},
						legend:
						{
							verticalAlign: 'top',
							floating : false,
							//y: -100,
							enabled: true
						},				    
						series: [{
							shadow: true,
							type: 'spline',
							name: 'Temperature',
							data: temp
							//yAxis: 1
						},{
							visible: true,
							dashStyle: 'ShortDash',
							shadow: true,
							type: 'spline',
							name: 'Point de rosée',
							data: ptrose
						}, {
							visible: true,
							dashStyle: 'Dash',
							shadow: true,
							type: 'spline',
							name: 'Humidité',
							data: humid,
							yAxis: 1
						}],
						rangeSelector:
							{
								buttons: [
								{type: 'day',count: 1,text: '1j'},
								{type: 'day',count: 5,text: '3j'},
								{type: 'week',count: 1,text: '7j'},
								{type: 'month',count: 1,text: '1m'},
								{type: 'year',count: 1,text: '1a'},
								{type: 'all',text: 'Tout'}],
								selected: 1
							},
						
						//title: { text: 'Temperature' },
						tooltip: { enabled: true,
						            valueDecimals: 1},
						xAxis: {
								type: 'datetime',
								maxZoom: 3600000 // one hour
								},
						yAxis: [{
							ordinal: false,
							opposite : false,
							title: {text: 'Degré Celsius'},
							// height: 350,
							//width: 500,
							lineWidth: 1,
							labels: { formatter: function () { return this.value + ' °C'; }}
						},
						{
							min: 0,
							max: 100,
							startOnTick: false,
							gridLineWidth: 1,
							lineWidth: 2,
							title: { text: 'Humidité', style: { color: '#55BF3B'}},
							labels: { formatter: function () { return this.value + ' %'; }},
							opposite: true
						}
						],
						load: function() {
									document.getElementById('container').style.background = 'none';
								}
					});
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
					  <li <?PHP if($nom_courbe == "temp_int"){echo 'class="active"';} ?>><a href="temperature.php?nom_courbe=temp_int">Intérieure</a></li>
					  <li <?PHP if($nom_courbe == "temp_ext"){echo 'class="active"';} ?>><a href="temperature.php?nom_courbe=temp_ext">Extérieure</a></li>
					  <li <?PHP if($nom_courbe == "temp_pc"){echo 'class="active"';} ?>><a href="temperature.php?nom_courbe=temp_pc">Puits Canadien</a></li>
					  <li><a href="temperature_stat.php">Stat. mensuelles</a></li>
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