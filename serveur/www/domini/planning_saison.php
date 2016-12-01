<?PHP
	include("infos/config.inc.php"); // on inclu le fichier de config
	@include("php/restore_donnees_instant.php");
?>
<!DOCTYPE html>	
<html>
 <head>
		<title>DoMini - Planning saison</title>
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
				// alert('type jour: ' + typejour +',  id_debut : '+ id_debut + ', id_fin :' + id_fin +', idjour :' + idjour);
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
						<li><a href="planning.php">Semaine</a></li>
						<li class="active"><a href="planning_saison.php" onClick="return false;">Saison</a></li>
						<li><a href="#" onClick="return false;">Type jour</a></li>
						<li><a href="#" onClick="return false;">Calendrier</a></li>
					</ul>
				  </div>
				</div>
				
				<div class="row-fluid">
					
					<div class="span9">
						<form method="POST" action="pellet.php" name="formulaire">
						<table class="table table-bordered table-striped ">
							<thead>
							  <tr>
								<th>Type</th>
								<th>T° consigne</th>
								<th>Flux solaire</th>
								<th>Commentaire</th>
							  </tr>
							</thead>
							<tbody>
							
							<?PHP
								// requete MySQL pour obtenir les données de la BDD
								//echo" $host, $login, $passe, $bdd \n";
									$link = mysqli_connect($host,$login,$passe,$bdd);
	if (!$link) {
		die('Erreur de connexion (' . mysqli_connect_errno() . ') '
				. mysqli_connect_error());
	}
								
								// requete pour récupérer chaque tranche horaire de 30 minutes pour la semaine en cours
								$SQL="SELECT  `type` ,  `consigne_temperature` ,  `flux solaire` ,  `commentaire` 
										FROM  `calendrier_saison`";
								//on lance la requete
								$RESULT = @mysqli_query($link, $SQL);
								// pour chaque type de saison renvoyée dans la table (une par ligne)
								while($myrow = @mysqli_fetch_array($RESULT)) {
									//on commence une nouvelle ligne du tableau html
									echo '<tr>';
									//on affiche le type de saison dans la 1ere colonne
									echo '<td>'.$myrow["type"].'</td>';
									//on affiche la consigne de T° dans la 2eme colonne
									echo'<td><div class="input-append input-prepend">
											<input class="span3" id="temp_manuel" type="text" maxlength="4" value="'.$myrow["consigne_temperature"].'"> 
											<span class="add-on">°C</span>
										</div></td>';
									//on affiche la consigne de flux solaire dans la 3eme colonne
									echo'<td><div class="input-append input-prepend">
											<input class="span3" id="fluxsolaire" type="text" maxlength="4" value="'.$myrow["flux solaire"].'"> 
											<span class="add-on">w/m²</span>
										</div></td>';
									//on affiche le commentaire
									echo '<td>'.$myrow["commentaire"].'</td>';
									echo '</tr>';
								}

								// on libère la mémoire
								mysqli_free_result($RESULT) ;
								// on ferme la session mysql
								mysqli_close($link);
							?>
							<tr>
								<td colspan="4"><input type="submit" name="btenvoyer" value="       Appliquer       "/></td>
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