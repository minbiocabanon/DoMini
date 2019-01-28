<?PHP
	include("infos/config.inc.php"); // on inclu le fichier de config
	@include("php/restore_donnees_instant.php");
?>
<!DOCTYPE html>
<html lang="fr">
	<head>
		<title>DoMini- Reseau</title>
		<meta charset="UTF-8">
		<meta name="description" content="" />
		<meta http-equiv="Refresh" content="1024">
		<meta name="viewport" content="width=device-width,initial-scale=1">
		<link rel="shortcut icon" type="image/x-icon" href="./img/favicon.ico" />
		<!-- Les feuilles de styles -->
		<link href="css/bootstrap.css" rel="stylesheet">
		<link href="css/bootstrap-responsive.css" rel="stylesheet">

		<!-- JS files for Jquery -->
		<script type="text/javascript" src="js/jquery-latest.js"></script>
		<!-- JS files for bootstrap -->
		<script type="text/javascript" src="js/bootstrap.js"></script>
	</head>

	<body>
		<!-- Part 1: Wrap all page content here -->
		<div id="wrap">
			<?PHP include("menu.html"); ?>

			<div class="container">				  
				
					<div class="span12">
						
					<div class="bs-example">
					  <table class="table">
						<thead>
						  <tr>
							<th>Description</th>
							<th>@ LAN</th>
							<th>@ WAN</th>
						  </tr>
						</thead>
						<tbody>
						  <tr>
							<td>Accueil DoMini</td>
							<td><a href="http://192.168.0.102:80">http://192.168.0.102:80</a></td>
							<td><a href="http://domini.dlinkddns.com:50180">http://domini.dlinkddns.com:50180</a></td>
						  </tr>
						  <tr>
							<td>Revotech (jardin)</td>
							<td><a href="http://192.168.0.120:80">192.168.0.120:80</a></td>
							<td><a href="http://domini.dlinkddns.com:50112">http://domini.dlinkddns.com:50112</a></td>
						  </tr>
						  <tr>
							<td>Revotech (porche)</td>
							<td><a href="http://192.168.0.129:80">http://192.168.0.129:80</a></td>
							<td><a href="http://domini.dlinkddns.com:50113">http://domini.dlinkddns.com:50113</a></td>
						  </tr>
						  <tr>
							<td>Thomson DSC-323W (kiwi)</td>
							<td><a href="http://192.168.0.121:88">http://192.168.0.121:88</a></td>
							<td><a href="http://domini.dlinkddns.com:50121">http://domini.dlinkddns.com:50121</a></td>
						  </tr>	
						  <tr>
						  <tr>
							<td>Camera Foscam C2 (sejour)</td>
							<td><a href="http://192.168.0.130:88">http://192.168.0.130:88</a></td>
							<td><a href="http://domini.dlinkddns.com:50130">http://domini.dlinkddns.com:50130</a></td>
						  </tr>	
						  <tr>
						  
							<td>Router WiFi</td>
							<td><a href="http://192.168.0.1:80">http://192.168.0.1:80</a></td>
							<td><a href="http://domini.dlinkddns.com:50100">http://domini.dlinkddns.com:50100</a></td>
						  </tr>							  
						  <tr>
							<td>NAS</td>
							<td><a href="http://192.168.0.100:80">http://192.168.0.100:80</a></td>
							<td><a href="http://domini.dlinkddns.com:50110">http://domini.dlinkddns.com:50110</a></td>
						  </tr>
						  <tr>
							<td>ub1610 SSH </td>
							<td><a href="192.168.0.127:22">192.168.0.127:22</a></td>
							<td><a href="domini.dlinkddns.com:50117">domini.dlinkddns.com:50117</a></td>
						  </tr>						  
						  <tr>
							<td>OTA LinkitOne</td>
							<td><a href="http://192.168.0.102:83">http://192.168.0.102:83</a></td>
							<td><a href="http://domini.dlinkddns.com:50150">http://domini.dlinkddns.com:50150</a></td>
						  </tr>	
						  <tr>
							<td>Sigfox</td>
							<td><a href="192.168.0.102:82">192.168.0.102:82</a></td>
							<td><a href="domini.dlinkddns.com:50122">domini.dlinkddns.com:50122</a></td>
						  </tr>	
						  <tr>
							<td>Rpi WiFi Web</td>
							<td><a href="192.168.0.122:80">192.168.0.122:22</a></td>
							<td><a href="domini.dlinkddns.com:50123">domini.dlinkddns.com:50123</a></td>
						  </tr>
						  <tr>
							<td>my IP</td>
							<?PHP
								$myip = file_get_contents('http://ipinfo.io/ip/');
								//echo $myip;
							?>
							<td><a href="<?PHP echo $myip ?>"><?PHP echo $myip ?></a></td>
							<td></td>
						  </tr>							  
						</tbody>
					  </table>
					</div><!-- /example -->
					
					
					</div>		<!-- /span12 > -->
							
			</div><!-- /container -->
		</div><!-- /wrap -->
  </body>
</html>