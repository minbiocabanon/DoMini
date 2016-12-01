﻿<?php // content="text/plain; charset=utf-8"
	include("infos/config.inc.php"); // on inclu le fichier de config
	@include("php/restore_donnees_instant.php");
?>

<!DOCTYPE html>
<html lang="fr">
<head>
	<meta charset="UTF-8">
	<title>DoMini - Pellets</title>
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
			$.get('./csv/pellet.csv', function(csv, state, xhr) {
				
				// inconsistency
				if (typeof csv != 'string') {
					csv = xhr.responseText;
				} 
				
				// parse the CSV data
				var credit = [], debit = [], capital = [], header, comment = /^#/, x;
				
				$.each(csv.split('\n'), function(i, line){
				    if (!comment.test(line)) {
				        if (!header) {
				            header = line;
				        
				        } else if (line.length) {
				            var point = line.split(';'), 
				            	date = point[0].split('-'),
				            	time = point[1].split(':');
				            
				            x = Date.UTC(date[2], date[1] - 1, date[0], time[0], time[1]);
				            
				            credit.push([x, parseInt(point[2])]); 
							debit.push([x, parseInt(point[3])]);
							capital.push([x, parseInt(point[4])]);
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
						enabled : true,
						//je reforce le format du navigateur car ça ne prend pas le style de la courbe par défaut...
						series: {
							type: 'column',
							color: '#4572A7',
							fillOpacity: 0.4
						}
					},				    
				    series: [{
						shadow: true,
				        type: 'column',
				        name: 'Credit',
						color : '#FF0000',
				        data: credit
				    },{
						shadow: true,
						type: 'column',
						name: 'Debit',
						color : '#00FF00',
				        data: debit
				    },{
						shadow: true,
						type: 'column',
						name: 'Capital',
						color : '#0000FF',
				        data: capital
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
							selected: 3
						},
				    
					//title: { text: 'Temperature' },
					
				    xAxis: {
							type: 'datetime',
							ordinal: false,
							//maxZoom: 3600000 // one hour
							},
				    yAxis: [{
						title: {text: 'Sacs'},
						opposite : false,
						min: 0,
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
				  <li class="active"><a href="pellet_conso.php">Consommation</a></li>
				  <li><a href="pellet_bilan.php">Bilan</a></li>
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