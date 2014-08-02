<?PHP
	include("infos/config.inc.php"); // on inclu le fichier de config
	@include("php/restore_donnees_instant.php");
?>
<!DOCTYPE html>
<html lang="fr">
<head>
	<title>DoMini - Log consigne chauffage (PID))</title>
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
		$(function() {
			$.get('./csv/chauffage_log.csv', function(csv, state, xhr) {
				
				// inconsistency
				if (typeof csv != 'string') {
					csv = xhr.responseText;
				} 
				
				// parse the CSV data
				var temp_int = [], temp_consigne = [], puissance = [], mode = [], header, comment = /^#/, x;
				
				$.each(csv.split('\n'), function(i, line){
				    if (!comment.test(line)) {
				        if (!header) {
				            header = line;
				        
				        } else if (line.length) {
				            var point = line.split(';'), 
				            	date = point[0].split('-'),
				            	time = point[1].split(':');
				            
				            x = Date.UTC(date[2], date[1] - 1, date[0], time[0], time[1]);
				            
				            temp_int.push([x, parseFloat(point[2])]); // température intérieure
							temp_consigne.push([x, parseFloat(point[3])]); // température de consigne
							puissance.push([x, parseInt(point[4])]); // puissance calculée par le PID
							mode.push([x, parseInt(point[5])]); // mode

				        }
				    }
				});
				
				// create the chart
				chart = new Highcharts.StockChart({
				    chart: {
				        renderTo: 'container_graph',
						alignTicks: false,
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
				        name: 'T° int.',
				        data: temp_int
				    },
					{
						shadow: true,
				        type: 'spline',
				        name: 'T° cons.',
				        data: temp_consigne
				    },
					{
						shadow: true,
				        type: 'column',
				        name: 'consigne (%)',
				        data: puissance,
				        yAxis: 1
				    },
					{
						shadow: true,
				        type: 'column',
				        name: 'mode',
				        data: mode,
						yAxis: 2
				    }],
					rangeSelector:
						{
							buttons: [
							{type: 'day',count: 1,text: '1j'},
							{type: 'day',count: 3,text: '3j'},
							{type: 'week',count: 1,text: '7j'},
							{type: 'month',count: 1,text: '1m'}],
							selected: 0
						},
				    
					//title: { text: 'Temperature' },
					tooltip: { yDecimals: 1, enabled: true },
					plotOptions: { 
						series: { dataGrouping: { enabled: false, }} 
						},
				    xAxis: {
							ordinal: false,
							type: 'datetime',
							maxZoom: 3600000 // one hour
							},
				    yAxis: [{
						title: {text: 'Température'},
						opposite : false,
						height: 150,
						//width: 500,
						min: 0,
				        lineWidth: 1,
						labels: { formatter: function () { return this.value + ' °C'; }}
				    },
					{
						title: { text: 'Consigne', style: { color: '#55BF3B'}},
						height: 150,
						min: 0,
						max: 100,
						lineWidth: 2,
						labels: { formatter: function () { return this.value + ' %'; }},
						startOnTick: false,
						gridLineWidth: 1,
						opposite: true
					},
					{
						title: { text: 'mode', style: { color: '#DD1100'}},
						top: 225,
						height: 35,
						min: 0,
						max: 3,
						lineWidth: 2,
						tickInterval: 1,
						startOnTick: false,
						gridLineWidth: 1
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
					<a class="brand" href="#">Chauffage log</a>
					<ul class="nav">
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