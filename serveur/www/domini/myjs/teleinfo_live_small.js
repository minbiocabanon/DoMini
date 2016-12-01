/*!
 * javascript pour générer graph téléinfo live
 */
$(function() {
	$.get('../../csv/teleinfo_live.csv', function(csv, state, xhr) {
		
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
				data: HP
			},{
				shadow: true,
				type: 'area',
				name: 'Heure Creuse',
				color : '#00FF00',
				data: HC
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
});
