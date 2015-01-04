<?PHP
	include("infos/config.inc.php"); // on inclu le fichier de config
	@include("php/restore_donnees_instant.php");
?>
<!DOCTYPE html>
<html lang="fr">
<head>
	<?php 
			// On se connecte à la BDD
			@mysql_connect($host,$login,$passe) or die("Impossible de se connecter à la base de données");
			@mysql_select_db("$bdd") or die("Impossible de se connecter à la base de données");
			
			$SQL="SET lc_time_names = 'fr_FR'" ; // Pour afficher date en français dans MySql. 
			mysql_query($SQL) ;
			
			
			// requete MySQL pour obtenir les données du compteur
			$SQL="SELECT DATE_FORMAT(date_time, '%e %M %Y - %T') as DATE,HC,HP
				FROM teleinfo
				ORDER BY date_time DESC
				LIMIT 0 , 1"; 
			// on execute la requete
			$RESULT = @mysql_query($SQL);
			// on recupére le resultat
			$myrow = @mysql_fetch_array($RESULT);
			// on stocke la quantité de HP et HC consommés
			$data_HC = floor($myrow["HC"]/1000);
			$data_HP = floor($myrow["HP"]/1000);
			$date = $myrow["DATE"];
			//echo"NB de pellets en stock : $data_nbpellets[0]";
			// on clean et on ferme la BDD
			mysql_free_result($RESULT) ;
			mysql_close();
	?>
	<title>DoMini - Teleinfo compteur EDF</title>
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
				  <li><a href="teleinfo.php?nom_courbe=jour">Jour</a></li>
				  <li><a href="teleinfo.php?nom_courbe=mois">Mois</a></li>
				  <li><a href="teleinfo.php?nom_courbe=an">Annuel</a></li>
				  <li class="active"><a href="teleinfo_edf.php">Compteur</a></li>
				</ul>
			  </div>
			</div>
			
			<div class="row-fluid">
				
				<div class="span12">
					<table class="table table-bordered table-striped ">
						<thead>
						  <tr>
							<th>HP</th>
							<th>HC</th>
						  </tr>
						</thead>
						<tbody>
							<tr>
								<td><?php echo str_pad($data_HP, 5, "0", STR_PAD_LEFT); ?> kWh</td>
								<td><?php echo str_pad($data_HC, 5, "0", STR_PAD_LEFT); ?> kWh</td>
							</tr>
							<tr>
								<td colspan="2">Relevé compteur au : <?php echo"$date"; ?></td>
							</tr>						  
						</tbody>
					 </table>
				</div>		<!-- /span6 > -->			
			</div>		<!-- /fluid > -->			
		</div><!-- /container -->
	</div><!-- /wrap -->
</body>
</html>