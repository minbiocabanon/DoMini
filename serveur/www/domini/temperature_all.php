<?PHP
	include("infos/config.inc.php"); // on inclu le fichier de config
	@include("php/restore_donnees_instant.php");
?>
<!DOCTYPE html>
<html lang="fr">
<head>
	<meta charset="UTF-8">
	<title>DoMini - Temperatures</title>	
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
		    $(function ()
		    {
					var seriesOptions = [],
					seriesCounter = 0,
					names = ['ext', 'int', 'pc','air_neuf','garage','vmc_ext'];

		        $.each(names, function (i, name)
		        {
							$.get('./csv/temp_' + name + '.csv', function (csv, state, xhr)
							{
								// inconsistency
								if (typeof csv != 'string')
								{
									csv = xhr.responseText;
								}
								// parse the CSV data
								var temp = [], header, comment = /^#/, axe, seriename, seriecolor;
								$.each(csv.split('\n'), function(i, line){
									if (!comment.test(line)) {
										if (!header) {
											header = line;
										
										} else if (line.length) {
											var point = line.split(';'), 
												date = point[0].split('-'),
												time = point[1].split(':');
											
											x = Date.UTC(date[2], date[1] - 1, date[0], time[0], time[1]);
											
											temp.push([x, parseFloat(point[2])]); 
											//humid.push([x, parseFloat(point[3])]); // humidité
											//ptrose.push([	x, parseFloat(point[4])]); // point de rosée		            
										}
									}
								});

								// Affectation des noms et axes
								switch (name)
								{
									//'', '', ''         
									//'Temperature Observatoire': '°C', '': '°C', : '°C', '':       
									case 'ext':
										axe = 0;
										seriename = 'Exterieure';
										//seriecolor = '#AA4643';
										break;
									case 'int':
										axe = 0;
										seriename = 'Interieure';
										//seriecolor = '#89A54E';
										break;
									case 'pc':
										axe = 0;
										seriename = 'Puits Canadien';
										//seriecolor = '#4572A7';
										break;
									case 'air_neuf':
										axe = 0;
										seriename = 'Air neuf';
										break;
									case 'garage':
										axe = 0;
										seriename = 'Garage';										
										break;	
									case 'vmc_ext':
										axe = 0;
										seriename = 'VMC ext.';										
										break;											
								}
								seriesOptions[i] = 
								{
									name: seriename,
									shadow: true,
									type: 'spline',
									color: seriecolor,
									data: temp,
									yAxis: axe
								};

								// As we're loading the data asynchronously, we don't know what order it will arrive. So
								// we keep a counter and create the chart when all the data is loaded. 
								seriesCounter++;
								if (seriesCounter == names.length)
								{
										createChart();
								}
						});
				});

				// create the chart when all data is loaded
				function createChart()
				{
					chart = new Highcharts.StockChart(
					{
						chart:{
							renderTo: 'container_graph',
							zoomType: 'x',
							//alignTicks: false
						},
						legend:{
							verticalAlign: 'top',
							floating : false,
							//y: -100,
							enabled: true
						},
						rangeSelector: { 
							selected: 1 
						},
						//title: { text: 'Temperature Observatoire vs Luminosité' },
						//tooltip: { yDecimals: 1, enabled: true },
						tooltip: { 	enabled: true,
						            valueDecimals: 1,
									valueSuffix: ' °C'},
						xAxis:{
							ordinal: false,
							type: 'datetime',
							maxZoom: 8 * 3600000, // 8 heures
							title: { text: null }
						},
						plotOptions: { 
							series: {
								dataGrouping: { enabled: true,
												smoothed : false},
								marker: { 
									enabled: false, 
									states: { 
										hover: { enabled: true}
									}
								}
							} 
						},
						yAxis: [
						// Premier axe temperatures
						{
							//gridLineWidth: 0,
							opposite : false,
							lineWidth: 1,
							title: { text: 'Temperatures'},
							labels: { formatter: function () { return this.value + ' °C'; }}
						}],
						load: 
							function() {
								document.getElementById('container_graph').style.background = 'none';
							},
						series: seriesOptions,
						rangeSelector:
						{
							buttons: [
							{type: 'day',count: 1,text: '1j'},
							{type: 'day',count: 3,text: '3j'},
							{type: 'week',count: 1,text: '7j'},
							{type: 'month',count: 1,text: '1m'},],
							selected: 1
						}
					});
				}
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
					  <li class="active"><a href="temperature_all.php">Tout</a></li>
					  <li><a href="temperature.php?nom_courbe=temp_int">Intérieure</a></li>
					  <li><a href="temperature.php?nom_courbe=temp_ext">Extérieure</a></li>
					  <li><a href="temperature.php?nom_courbe=temp_pc">Puits Canadien</a></li>
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
