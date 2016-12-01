<?php	
	include("../infos/config.inc.php"); // on inclu le fichier de config
	
	// requete MySQL pour obtenir les données de la BDD
		$link = mysqli_connect($host,$login,$passe,$bdd);
	if (!$link) {
		die('Erreur de connexion (' . mysqli_connect_errno() . ') '
				. mysqli_connect_error());
	}
	

	//requete pour récupérer la conso en W pour les 2 derniers jours
	$SQL="SELECT UNIX_TIMESTAMP(date_time) AS DATE, puissance, tarif 
	FROM teleinfo 
	WHERE date_time <= NOW( ) AND date_time >= SUBDATE(NOW(),2) 
	ORDER BY date_time"; 
	
	$RESULT = @mysqli_query($link, $SQL);
	
	// fetch a row and write the column names out to the file
	$row = mysqli_fetch_assoc($RESULT);
	
	// remove the result pointer back to the start
	mysqli_data_seek($RESULT, 0);
	
	// pour chaque ligne
	while($myrow = @mysqli_fetch_array($RESULT)) {
		 // on récupère la date au format unixtime
		 $data_timestamp = $myrow["DATE"];
		//selon HP ou HC on récupère la puissance et on met à 0 l'autre donnée
		if($myrow["tarif"] == "HP.."){
			$data_consojour_HP = $myrow["puissance"];
			$data_consojour_HC = 0;
		}else{
			$data_consojour_HP = 0;
			$data_consojour_HC = $myrow["puissance"];
		}
	
		// convert from Unix timestamp to JavaScript time
		$data_timestamp *= 1000; 
		// on créé les deux séries de données -> pas trop optimisé comme méthode, on doit déclarer 2 fois la date :-(
		$dataHC[] = "[$data_timestamp, $data_consojour_HC]";
		$dataHP[] = "[$data_timestamp, $data_consojour_HP]";
	}
			 
	//on quite la session mysql
	mysqli_free_result($RESULT) ;
	mysqli_close($link);

?>
<script type="text/javascript">
$(function() {
	chart = new Highcharts.StockChart({
			chart: {
				renderTo: 'container_teleinfo',
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
				type: 'area',
				name: 'Heure Pleine',
				color : '#FF0000',
				data: [<?php echo join($dataHP, ',') ?>]
			},{
				shadow: true,
				type: 'area',
				name: 'Heure Creuse',
				color : '#00FF00',
				data: [<?php echo join($dataHC, ',') ?>]
			}],
			rangeSelector:	{	enabled: false	},
			scrollbar:{	enabled: false	},
			navigator:{	enabled: false	},
			title: { text: 'Conso. Elec.', align: 'left' },
			tooltip: { yDecimals: 0, enabled: true },
			xAxis: {
					ordinal: false,
					type: 'datetime',
					maxZoom: 3600000 // one hour
					},
			yAxis: [{
				title: {text: 'Watts'},
				min: 0,
				lineWidth: 1,
				labels: { formatter: function () { return this.value + ' W'; }}
			}],
			load: function() {
						document.getElementById('container_teleinfo').style.background = 'none';
					}			
	});
});
</script>

	
