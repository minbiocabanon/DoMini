<?PHP
	include("infos/config.inc.php"); // on inclu le fichier de config
	@include("php/restore_donnees_instant.php");
	include('./planning/Planning.php');
	
	// on récupère les variables du formulaire
	$num_semaine = isset($_GET['num_semaine']) ? $_GET['num_semaine'] : '';
	if($num_semaine == ""){
		$num_semaine = date('W');
		//echo"Num semaine = $num_semaine";	
	}
		
	//echo date('l jS \of F Y h:i:s A');
	// requete MySQL pour obtenir les données de la BDD
	//echo" $host, $login, $passe, $bdd \n";
		$link = mysqli_connect($host,$login,$passe,$bdd);
	if (!$link) {
		die('Erreur de connexion (' . mysqli_connect_errno() . ') '
				. mysqli_connect_error());
	}
	
	// requete pour récupérer chaque tranche horaire de 30 minutes pour la semaine en cours
	$SQL="SELECT `id` , `heure_debut` , `heure_fin` , `temperature` , WEEKDAY(`date`) as dayweek 
	FROM `calendrier_30min` 
	WHERE WEEK( `date` , 3 ) = $num_semaine;";
	//on lance la requete
	$RESULT = @mysqli_query($link, $SQL);
	// lecture du resultat de la requete
	$myrow=@mysqli_fetch_array($RESULT);
	// pour chaque 30 minutes
	while($myrow = @mysqli_fetch_array($RESULT)) {
		// on fait un traitement particulier de la température (pour ne pas afficher des 0 partout)
		$temp = $myrow["temperature"];
		// on fiche la couleur rouge par défaut
		$cellcolor = "#CC0000";
		// si la température de consigne est 0
		if($temp == "0.0"){
			// on n'affichera rien
			$temp = "";
			// pas de couleur de fond
			$cellcolor = "";
		}
		// on génére une cellule pour le créneau lu et avec l'ID de cellule (pour traitement AJAX qd on cliquera sur la cellule) et la température de consigne
		$contenusCellules[] = new PlanningCellule($myrow["dayweek"],$myrow["id"],$myrow["heure_debut"],$myrow["heure_fin"],$cellcolor, $temp);
	}
	// on libère la mémoire
	mysqli_free_result($RESULT) ;
	
	//on récupére la température
	//requete pour récupérer la température de consigne de saison
	$SQL="	SELECT `consigne_temperature`
			FROM `calendrier_saison` 
			WHERE `type` = ( 
				SELECT `saison` 
				FROM `calendrier` 
				WHERE `date` =  DATE_FORMAT(NOW(), '%Y-%m-%d')) 
			LIMIT 0 , 1"; 
	
	//on envoie la requete
	$RESULT = @mysqli_query($link, $SQL);
	//on récupère le résultat
	$myrow=@mysqli_fetch_array($RESULT); 
	// on extrait le résultat et on le stocke dans une variable
	$temp_consigne = $myrow["consigne_temperature"];
	//libération de la variable
	mysqli_free_result($RESULT) ;
	
	
	
	// on ferme la session mysql
	mysqli_close($link);		
?>
<!DOCTYPE html>	
<html>
 <head>
		<title>DoMini - Planning</title>
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
		<script>
			$(document).ready(function(){
				$('table td').click(function(){
					var cell = $(this).attr('id');
					var temp_man = $('#temp_manuel').val();
					//alert('temp_man: '+temp_man);

					// TEST ------------ !!
					if (cell==""){
						document.getElementById(cell).innerHTML="";
						return;
					}
					if (window.XMLHttpRequest){
					// code for IE7+, Firefox, Chrome, Opera, Safari
						xmlhttp=new XMLHttpRequest();
					}
					else{
					// code for IE6, IE5
						xmlhttp=new ActiveXObject("Microsoft.XMLHTTP");
					}
					xmlhttp.onreadystatechange=function(){
						if (xmlhttp.readyState==4 && xmlhttp.status==200){
							document.getElementById(cell).innerHTML=xmlhttp.responseText;
						}
					}
					xmlhttp.open("GET","./planning/change_cellule.php?IdCell="+cell+"&temp_man="+temp_man,true);
					xmlhttp.send();
					
					// référence à mon élément (la cellule dont je veux changer la couleur de fond)
					oCible = document.getElementById(cell);
					if(oCible.bgColor != '#CC0000')
						// Affecter la couleur du fond attention à la casse
						oCible.bgColor = '#CC0000';
					else
						// Affecter la couleur du fond attention à la casse
						oCible.bgColor = 'transparent';
				});				
			});
			
			function ChangeTypeJour(typejour, idjour, id_debut, id_fin){
				//alert('type jour: ' + typejour +',  id_debut : '+ id_debut + ', id_fin :' + id_fin +', idjour :' + idjour);
			   var xmlhttp=null;
			   if (window.XMLHttpRequest) {
				  xmlhttp = new XMLHttpRequest();
			   }
			   else if (window.ActiveXObject)
			   {
				  xmlhttp = new ActiveXObject("Microsoft.XMLHTTP");
			   }
				
			   	xmlhttp.open("GET", "./planning/change_jour.php?idjour="+idjour+"&typejour="+typejour, false);
				xmlhttp.send();
			   
			   var cell = id_debut;
			   while(cell <= id_fin){
					// alert('cell : ' + cell );
					xmlhttp.open("GET","./planning/update_cellule.php?IdCell="+cell, false);
					xmlhttp.send();
					document.getElementById(cell).innerHTML = xmlhttp.responseText;
				   
				   	// référence à mon élément (la cellule dont je veux changer la couleur de fond)
					oCible = document.getElementById(cell);
					if(document.getElementById(cell).innerHTML != "0.0"){
						// Affecter la couleur du fond attention à la casse
						oCible.bgColor = '#CC0000';
					}
					else{
						document.getElementById(cell).innerHTML = "";
						// Affecter la couleur du fond attention à la casse
						oCible.bgColor = 'transparent';
					}
					cell++;
				}
			};			
		</script>
		
	<body>
		<!-- Part 1: Wrap all page content here -->
		<div id="wrap">
			<?PHP include("menu.html"); ?>
		
			<div class="container">
				<div class="navbar">
				  <div class="navbar-inner">
					<a class="brand" href="#">Planning</a>
					<ul class="nav">
						<li class="active"><a href="planning.php">Semaine</a></li>
						<li><a href="#" onClick="return false;">Mois</a></li>
						<li><a href="#" onClick="return false;">Année</a></li>
						<li><a href="#" onClick="return false;">Configuration</a></li>
					</ul>
				  </div>
				</div>
				
				<div class="row-fluid">
					<div class="span6">
						<a class="btn" href="planning.php?num_semaine=<?php echo $num_semaine-1; ?>"><i class="icon-chevron-left"></i></a>
						S<?php echo $num_semaine; ?>
						<a class="btn" href="planning.php?num_semaine=<?php echo $num_semaine+1; ?>"><i class="icon-chevron-right"></i></a>
					</div>
					<div class="span6">
						<div class="input-append input-prepend">
							<span class="add-on">Température :</span>
							<input class="span2" id="temp_manuel" type="text" maxlength="4" value="<?php echo $temp_consigne; ?>"> 
							<span class="add-on">°C</span>
							<!--<button id="formulaire_chauffe" class="btn" type="button">Appliquer</button> -->
						</div>
						<!--<input class="span2" type="text" name="temp_man" maxlength="4" placeholder="<?php echo $temp_consigne; ?>"> °C -->
					</div>
				</div>
				<div class="row-fluid">	
					<div class="span12">
						<?php								
							// on génére le tableau de la semaine
							$planning = new Planning(0, 6, 330, 1410, 30, $contenusCellules);
							// on affiche la table
							$planning->afficherHtmlTable();
						?>		
					</div>
				</div><!-- /fluid -->
			</div><!-- /container -->
		</div><!-- /wrap -->				
	</body>		
 </html>