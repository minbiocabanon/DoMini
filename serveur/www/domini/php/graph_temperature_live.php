<?php	
	include("../infos/config.inc.php"); // on inclu le fichier de config
	
	// requete MySQL pour obtenir les données de la BDD
	@mysql_connect($host,$login,$passe) or die("Impossible de se connecter à la base de données");
	@mysql_select_db("$bdd") or die("Impossible de se connecter à la base de données");

	//requete pour récupérer la température EXT pour les 2 derniers jours
	$SQL="SELECT UNIX_TIMESTAMP(date_time) AS DATE, ana1 
	FROM analog1
	WHERE date_time <=  NOW( ) AND date_time >= SUBDATE(NOW(),2) 
	ORDER BY date_time"; 
	$RESULT = @mysql_query($SQL);
	// fetch a row and write the column names out to the file
	$row = mysql_fetch_assoc($RESULT);
	// remove the result pointer back to the start
	mysql_data_seek($RESULT, 0);
	
	// pour chaque ligne
	while($myrow = @mysql_fetch_array($RESULT)) {
		// on récupère la date au format unixtime
		$data_timestamp = $myrow["DATE"];
		// on récupère la temperature en °C
		$datatemp = $myrow["ana1"];
		// convert from Unix timestamp to JavaScript time
		$data_timestamp *= 1000; 
		// on créé la série de données
		$datatempext[] = "[$data_timestamp, $datatemp]";
	}
	mysql_free_result($RESULT) ;
	
	//requete pour récupérer la température INT pour les 2 derniers jours
	$SQL="SELECT UNIX_TIMESTAMP(date_time) AS DATE, ana1 
	FROM analog2
	WHERE date_time <=  NOW( ) AND date_time >= SUBDATE(NOW(),2) 
	ORDER BY date_time"; 
	$RESULT = @mysql_query($SQL);
	// fetch a row and write the column names out to the file
	$row = mysql_fetch_assoc($RESULT);
	// remove the result pointer back to the start
	mysql_data_seek($RESULT, 0);
	
	// pour chaque ligne
	while($myrow = @mysql_fetch_array($RESULT)) {
		// on récupère la date au format unixtime
		$data_timestamp = $myrow["DATE"];
		// on récupère la temperature en °C
		$datatemp = $myrow["ana1"];
		// convert from Unix timestamp to JavaScript time
		$data_timestamp *= 1000; 
		// on créé la série de données
		$datatempint[] = "[$data_timestamp, $datatemp]";
	}
	mysql_free_result($RESULT) ;
	
	//requete pour récupérer la température PC pour les 2 derniers jours
	$SQL="SELECT UNIX_TIMESTAMP(date_time) AS DATE, ana1 
	FROM analog3
	WHERE date_time <=  NOW( ) AND date_time >= SUBDATE(NOW(),2) 
	ORDER BY date_time"; 
	$RESULT = @mysql_query($SQL);
	// fetch a row and write the column names out to the file
	$row = mysql_fetch_assoc($RESULT);
	// remove the result pointer back to the start
	mysql_data_seek($RESULT, 0);
	
	// pour chaque ligne
	while($myrow = @mysql_fetch_array($RESULT)) {
		// on récupère la date au format unixtime
		$data_timestamp = $myrow["DATE"];
		// on récupère la temperature en °C
		$datatemp = $myrow["ana1"];
		// convert from Unix timestamp to JavaScript time
		$data_timestamp *= 1000; 
		// on créé la série de données
		$datatemppc[] = "[$data_timestamp, $datatemp]";
	}	
	//on quite la session mysql
	mysql_free_result($RESULT) ;
	mysql_close();

?>
<script type="text/javascript">
$(function() {
	chart = new Highcharts.StockChart({
			chart: {
				renderTo: 'container_temperature',
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
				        name: 'Int.',
				        data: [<?php echo join($datatempint, ',') ?>]
				        //yAxis: 1
				    },{
						shadow: true,
				        type: 'spline',
				        name: 'Ext.',
				        data: [<?php echo join($datatempext, ',') ?>]
				    }, {
						shadow: true,
				        type: 'spline',
				        name: 'PC',
				        data: [<?php echo join($datatemppc, ',') ?>]
				    }],
			rangeSelector:	{	enabled: false	},
			scrollbar:{	enabled: false	},
			navigator:{	enabled: false	},
			title: { text: 'Temperatures', align: 'left' },
			tooltip: { yDecimals: 1, enabled: true },
			xAxis: {
					ordinal: false,
					type: 'datetime',
					maxZoom: 8 * 3600000, // 8 heures
					title: { text: null }
					},
			yAxis: [{
				title: {text: '°C'},
				min: 0,
				lineWidth: 1,
				labels: { formatter: function () { return this.value + ' °C'; }}
			}],
			load: function() {
						document.getElementById('container_teleinfo').style.background = 'none';
					}			
	});
});
</script>

	
