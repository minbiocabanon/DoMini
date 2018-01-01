<?php // content="text/plain; charset=utf-8"
	include("infos/config.inc.php"); // on inclu le fichier de config
	@include("php/restore_donnees_instant.php");
?>

<!DOCTYPE html>
<html lang="fr">
<head>
	<meta charset="UTF-8">
	<title>DoMini - Consommation annuelle de pellets</title>
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
			$.get('./csv/pellet_bilan.csv', function(csv, state, xhr) {
				
				// inconsistency
				if (typeof csv != 'string') {
					csv = xhr.responseText;
				} 
				
				// parse the CSV data
				var conso = [], header, comment = /^#/, x;
				
				$.each(csv.split('\n'), function(i, line){
				    if (!comment.test(line)) {
						//alert('line:'+line);
				        if (!header) {
				            header = line;
				        
				        } else if (line.length) {
				            var point = line.split(';');
				            
				            x = Date.UTC(point[0], 1, 1, 0, 0);
				            
				            conso.push([x, parseInt(point[1])]); 
				        }
				    }
				});
				
				// create the chart
				chart = new Highcharts.StockChart({
				    chart: {
				        renderTo: 'container_graph',
						//zoomType: 'x',
						type: 'column',
				    },
					legend:
					{
						verticalAlign: 'top',
						floating : false,
						//y: -100,
						enabled: true
					},
					plotOptions: {
						column: {
							borderWidth: 0,
							shadow: false,
							//stacking: "normal"
							dataGrouping: {
								enabled: false
								},
						},
					},
					navigator: {
						enabled : false
					},				    
				    series: [{
						shadow: true,
				        type: 'column',
				        name: 'Nombre de sacs (15kg) consommés',
						color : '#FF0000',
				        data: conso
				    }],
					rangeSelector:
						{
							buttons: [
							{type: 'day',count: 1,text: '1j'},
							{type: 'day',count: 3,text: '3j'},
							{type: 'week',count: 1,text: '7j'},
							{type: 'month',count: 1,text: '1m'},
							{type: 'year',count: 1,text: '1a'},
							{type: 'all',text: 'Tout'}],
							selected: 5
						},
				    
					//title: { text: 'Temperature' },
					
				    xAxis: {
							//type: 'datetime',
							//maxZoom: 3600000 // one hour
							},
				    yAxis: [{
						title: {text: 'Sacs'},
						opposite : false,
						min: 0,
						max: 90,
				        lineWidth: 1
				    }],
					load: function() {
								document.getElementById('container_graph').style.background = 'none';
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
				<a class="brand" href="#">Pellets : </a>
				<ul class="nav">
				  <li><a href="pellet_conso.php">Consommation</a></li>
				  <li class="active"><a href="pellet_bilan.php">Bilan</a></li>
				  <li><a href="pellet.php">Stock</a></li>
				  <li><a href="pellet_rsv.php">Niv. reservoir</a></li
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