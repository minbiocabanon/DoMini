<?php
	include("infos/config.inc.php"); // on inclu le fichier de config
	@include("php/restore_donnees_instant.php");
	
	$form_envoi = "NOK";
	$donnees_OK = false;
	
	$quantite_in = isset($_POST['quantite_in']) ? $_POST['quantite_in'] : '';
	$quantite_out = isset($_POST['quantite_out']) ? $_POST['quantite_out'] : '';  
	$data_nbpellets = isset($_POST['nbpellets']) ? $_POST['nbpellets'] : ''; 
	$form_envoi = isset($_POST['envoi']) ? $_POST['envoi'] : '';
	$date = isset($_POST['date']) ? $_POST['date'] : '';
	
	// Connexion à la BDD
	@mysql_connect($host,$login,$passe) or die("Impossible de se connecter à la base de données");
	@mysql_select_db("$bdd") or die("Impossible de se connecter à la base de données");

	// requete MySQL pour obtenir les données de pellets pour les 12 derniers mois
	// pour les 12 derniers mois
	$SQL="SELECT capital
		FROM pellets
		ORDER BY id DESC 
		LIMIT 0 , 1"; 
	// on execute la requete
	$RESULT = @mysql_query($SQL);
	// on recupére le resultat
	$myrow = @mysql_fetch_array($RESULT);
	// on stocke la quantité de sacs de pellets en stock
	$data_nbpellets = $myrow["capital"];
	//echo"NB de pellets en stock : $data_nbpellets[0]";
	
	mysql_free_result($RESULT) ;
	mysql_close();
	
		
	// echo" quantité saisie IN : $quantite_in<br>";
	// echo" quantité saisie OUT: $quantite_out<br>";
	// echo" capital: $data_nbpellets<br>";
	// echo "bouton envoyer = $form_envoi<br>";
	// echo "date = $date<br>";
	
	if($quantite_in == "")
		$quantite_in = 0;
	if($quantite_out == "")
		$quantite_out = 0;	

	//echo "form_envoi = $form_envoi <br>";

	
	
		//si on vient de saisir des infos passées en paramètre de l'adresse
	if($form_envoi == "OK"){
		// Connexion à la BDD
		@mysql_connect($host,$login,$passe) or die("Impossible de se connecter à la base de données");
		@mysql_select_db("$bdd") or die("Impossible de se connecter à la base de données");
		// requete MySQL pour stocker les nouvelles valeurs
		$SQL="INSERT INTO pellets VALUES('','$date', $quantite_in, $quantite_out, ($data_nbpellets+$quantite_in-$quantite_out))"; 

		//Execution de la requete
		mysql_query($SQL) or die('Erreur SQL !'.$SQL.'<br>'.mysql_error());
		mysql_close();
		
		//message de confirmation
		//mis à 1 du flag pour afficher le message de confirmatio
		$donnees_OK = true;
		$form_envoi = "NOK";
		
		//reset des valeurs pour éviter de resaisir les infos en cas de refresh de la page
		$_POST['quantite_in'] = "";
		$_POST['quantite_out'] = "";
		$_POST['envoi'] = "";
		$quantite_in = 0;
		$quantite_out = 0;
		$data_nbpellets = 0;
		
		// on lance le soft qui regénère le fichier CSV
		exec("cd /var/www/domini/php/highstock/");
		exec("wget 192.168.0.102:80/php/highstock/csv_pellet.php",$result);
		exec("rm /var/www/domini/php/highstock/csv_pellet.php.*");
		exec("rm /var/www/domini/csv_pellet.php.*");
	}

	// Connexion à la BDD
	@mysql_connect($host,$login,$passe) or die("Impossible de se connecter à la base de données");
	@mysql_select_db("$bdd") or die("Impossible de se connecter à la base de données");

	
	// requete MySQL pour obtenir les données de pellets pour les 12 derniers mois
	// pour les 12 derniers mois
	$SQL="SELECT capital
		FROM pellets
		ORDER BY id DESC 
		LIMIT 0 , 1"; 
	// on execute la requete
	$RESULT = @mysql_query($SQL);
	// on recupére le resultat
	$myrow = @mysql_fetch_array($RESULT);
	// on stocke la quantité de sacs de pellets en stock
	$data_nbpellets = $myrow["capital"];
	//echo"NB de pellets en stock : $data_nbpellets[0]";
	
	// on clean et on ferme la BDD
	mysql_free_result($RESULT) ;
	mysql_close();
	
	
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
		<link href="css/datepicker.css" rel="stylesheet">
		
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
		<!-- Bootstrap date picker  lien : http://www.eyecon.ro/bootstrap-datepicker/ -->
		<script src="js/bootstrap-datepicker.js"></script>
		<script>
			$(function(){
				window.prettyPrint && prettyPrint();
				$('#dp1').datepicker({
					format: 'yyyy-mm-dd'
				});
				$('#dp3').datepicker();
			});
		</script>
		<script type="text/javascript">
		$(function() {
			$.get('csv/pellet.csv', function(csv, state, xhr) {
				
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
				  <li><a href="pellet_conso.php">Consommation</a></li>
				  <li><a href="pellet_bilan.php">Bilan</a></li>
				  <li class="active"><a href="pellet.php">Stock</a></li>
				</ul>
			  </div>
			</div>
			
			<div class="row-fluid">
				
				<div class="span9">
					<form method="POST" action="pellet.php" name="formulaire">
					<table class="table table-bordered table-striped ">
						<thead>
						  <tr>
							<th>Quantité de sacs en stock </th>
							<th><?PHP echo "$data_nbpellets"; ?></th>
						  </tr>
						</thead>
						<tbody>
							<tr>
								<td colspan="2">Gestion du stock </td>
							</tr>
							<tr>
								<td>Date</td>
								<td>
									<div class="input-append date" id="dp3" data-date="<? echo date("Y-m-d"); ?>" data-date-format="yyyy-mm-dd">
										<input class="span4" size="10" type="text" name="date" value="<? echo date("Y-m-d"); ?>" readonly>
										<span class="add-on"><i class="icon-calendar"></i></span>
									</div>								
								</td>
							</tr>	
							<tr>
								<td>Quantité de granulés pris dans le stock</td>
								<td><input class="span5" type="text" name="quantite_out" maxlength="2"></td>
							</tr>	
							<tr>
								<td>Quantité de granulés remis dans le stock</td>
								<td><input class="span5" type="text" name="quantite_in" maxlength="2"></td>
							</tr>
							<tr>
								<td colspan="2"><input type="submit" name="btenvoyer" value="       OK       "/></td>
							</tr>							
						</tbody>
					 </table>
					<input type="hidden" name="envoi" value="OK">
					<input type="hidden" name="nbpellets" value="<?PHP echo"$data_nbpellets"; ?>">
					 </form>
				</div>		<!-- /span6 > -->			
			</div>		<!-- /fluid > -->			
		</div><!-- /container -->
	</div><!-- /wrap -->
</body>
</html>