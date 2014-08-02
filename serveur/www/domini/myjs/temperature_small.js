/*!
 * javascript pour générer graph téléinfo live
 */
$(function ()
{
		var seriesOptions = [],
		seriesCounter = 0,
		names = ['ext', 'int', 'pc'];

	$.each(names, function (i, name)
	{
				$.get('../../csv/temp_' + name + '.csv', function (csv, state, xhr)
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
				renderTo: 'container_temperature',
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
			title: { text: 'Temperatures', align: 'left'},
			tooltip: { yDecimals: 1, enabled: true },
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
				lineWidth: 1,
				title: { text: 'Temperatures'},
				labels: { formatter: function () { return this.value + ' C'; }}
			}],
			load: 
				function() {
					document.getElementById('container_temperature').style.background = 'none';
				},
			series: seriesOptions,
			rangeSelector:
						{
							buttons: [
							{type: 'day',count: 1,text: '1j'},
							{type: 'day',count: 3,text: '3j'},
							{type: 'week',count: 1,text: '7j'},
							{type: 'month',count: 1,text: '1m'},],
							selected: 1,
							enabled: true,
							inputEnabled: false,
						},
			//rangeSelector:	{	enabled: false	},
			scrollbar:{	enabled: false	},
			navigator:{	enabled: false	},				

		});
	}
});	