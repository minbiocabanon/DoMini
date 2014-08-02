<?php 
	include("infos/config.inc.php"); // on inclu le fichier de config
	@include("php/restore_donnees_instant.php");
	
	//on récupère ce qui est passé en paramètre dans la barre d'adresse
	$nom_courbe = $_GET['nom_courbe'];
	// si pas de nom passé en paramètre
	if($nom_courbe == ""){
		// on choisi d'afficher le graphe avec toutes les températures (cas par défaut)
		$nom_courbe = "jour";
	}
	
	switch($nom_courbe){
		case "jour":
			$rangeselected = 2;
		break;
		case "mois":
			$rangeselected = 5;
		break;
		case "an":
			$rangeselected = 5;
		break;					
	}	
?>
<!DOCTYPE html>
<html lang="fr">
<head>

	<title>DoMini - Teleinfo</title>
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
			$.get('./csv/teleinfo_' + '<?php echo $nom_courbe ?>' + '.csv', function(csv, state, xhr) {
				
				// inconsistency
				if (typeof csv != 'string') {
					csv = xhr.responseText;
				} 
				
				// parse the CSV data
				var HC = [], HP = [], header, comment = /^#/, x;
				
				$.each(csv.split('\n'), function(i, line){
				    if (!comment.test(line)) {
				        if (!header) {
				            header = line;
				        
				        } else if (line.length) {
				            var point = line.split(';'), 
				            	date = point[0].split('-'),
				            	time = point[1].split(':');
				            
				            x = Date.UTC(date[2], date[1] - 1, date[0], time[0], time[1]);
				            
				            HP.push([x, parseFloat(point[2])]); // conso en heure pleine
							HC.push([x, parseFloat(point[3])]); // conso en heure creuse
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
							stacking: "normal"
						},
					},
					navigator: {
						enabled : false,
						// je reforce le format du navigateur car ça ne prend pas le style de la courbe par défaut...
						// series: {
							// type: 'column',
							// color: '#4572A7',
							// fillOpacity: 0.4
						// }
					},				    
				    series: [{
						shadow: true,
				        type: 'column',
				        name: 'Heure Pleine',
						color : '#FF0000',
				        data: HP
				    },{
						shadow: true,
						type: 'column',
						name: 'Heure Creuse',
						color : '#00FF00',
				        data: HC
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
							selected: <?php echo"$rangeselected"; ?>
						},
				    
					//title: { text: 'Temperature' },
					
				    xAxis: {
							ordinal: false,
							type: 'datetime',
							//maxZoom: 3600000 // one hour
							},
				    yAxis: [{
						title: {text: '<?php if($nom_courbe == "an") {echo " kWh";} else{ echo " €";} ?>'},
						opposite : false,
						min: 0,
				        lineWidth: 1,
						labels: { formatter: function () { return this.value + '<?php if($nom_courbe == "an") {echo " kWh";} else{ echo " €";} ?>'; }}
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
				<a class="brand" href="#">Durée</a>
				<ul class="nav">
				  <li><a href="teleinfo_live.php">Live</a></li>
				  <li <?PHP if($nom_courbe == "jour"){echo 'class="active"';} ?>><a href="teleinfo.php?nom_courbe=jour">Jour</a></li>
				  <li <?PHP if($nom_courbe == "mois"){echo 'class="active"';} ?>><a href="teleinfo.php?nom_courbe=mois">Mois</a></li>
				  <li <?PHP if($nom_courbe == "an"){echo 'class="active"';} ?>><a href="teleinfo.php?nom_courbe=an">Annuel</a></li>
				  <li><a href="teleinfo_edf.php">Compteur</a></li>
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