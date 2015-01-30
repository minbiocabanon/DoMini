<?PHP
	include("infos/config.inc.php"); // on inclu le fichier de config
	@include("php/restore_donnees_instant.php");
	include("php/voletroulant.inc.php"); // fonction de conversion des Ã©tat des VR
	//include("php/voletroulant_etat.inc.php"); // récupére l'état des VR
	include("./php/voletroulant_tab.php");
?>
<!DOCTYPE html>
<html lang="fr">
	<head>
		<title>DoMini- Accueil</title>
		<meta charset="UTF-8">
		<meta http-equiv="Refresh" content="600">
		<meta name="viewport" content="width=device-width,initial-scale=1">
		<link rel="shortcut icon" type="image/x-icon" href="./img/favicon.ico" />
		<!-- Les feuilles de styles -->
		<link href="css/bootstrap.css" rel="stylesheet">
		<link href="css/bootstrap-responsive.css" rel="stylesheet">
		<link href="css/bootstrapSwitch.css" rel="stylesheet">
		<link href="css/bootstrap-select.css" rel="stylesheet">

		<!-- JS files for Jquery -->
		<script type="text/javascript" src="js/jquery-latest.js"></script>
		<!-- JS files for bootstrap -->
		<script type="text/javascript" src="js/bootstrap.js"></script>
		<!-- JS files for bootstrap switches-->
		<script type="text/javascript" src="js/bootstrapSwitch.js"></script>
		<!-- JS files for bootstrap select custom-->
		<script type="text/javascript" src="js/bootstrap-select.js"></script>
		
		<!-- AJAX pour volets roulant -->
		<script type="text/javascript" src="myjs/volet_roulant.js"></script>
	</head>

	<body>
		<!-- Part 1: Wrap all page content here -->
		<div id="wrap">
			<!-- Menu -->
			<?PHP include("menu.html"); ?>
			<div class="container">
				<div class="row-fluid">
					<div class="span6">
						<table class="table table-bordered table-striped table-condensed">
							<thead>
							  <tr>
								<th width=50%>Int.</th>
								<th width=50%>Ext.</th>
							  </tr>
							</thead>
							<tbody>
							  <tr>
								<td>
									<?php
										echo'<h2 rel="tooltip" title="'.$data_dateint[0].'">'.$data_tempint[0].'°C</h2>';
										//choix de la couleur du label pour indique si la TÂ° monte ou descend
										if($data_pentetemp[0] > 0)
											echo'<span class="label label-warning">';
										else if($data_pentetemp[0] < 0)
											echo'<span class="label label-info">';
										else //(températureature stable)
											echo'<span class="label label-default">';
										printf("%+1.1f °C/h</span>", $data_pentetemp[0]);
									?>
								</td>
								<td>
									<?php
										echo'<h2 rel="tooltip" title="'.$data_dateext[0].'">'.$data_tempext[0].'°C</h2>';
										if($data_pentetempext[0] > 0)
											echo'<span class="label label-warning">';
										else if($data_pentetempext[0] < 0)
											echo'<span class="label label-info">';
										else //(température stable)
											echo'<span class="label label-default">';
										printf("%+1.1f °C/h</span>", $data_pentetempext[0]);
									?>
								</td>
							  </tr>
							  <tr>
								<td class="tdhead" colspan="2">
								  Humidité
								</td>
							  </tr>
							  <tr>
								<td>
									<?PHP
									echo"$data_humint[0] %HR";
									?>
								</td>
								<td>
									<?PHP
									echo"$data_humext[0] %HR";
									?>
								</td>
							  </tr>
							  <tr>
								<td class="tdhead">
								  Puits canadien
								</td>
								<td class="tdhead">
								 Soleil
								</td>
							  </tr>
							  <tr>
								<td>
									<?PHP 
									require('./php/puissance_pc.php'); 
									
									echo'<span rel="tooltip" title="'.$data_datepc[0].'">'.$data_tempPC[0].'°C, </span>';
									//si le PC n'est pas en bypass
									if($consigne_pc != 0){
										//alors on affiche la puissance récupérée
										echo"$puissance_pc W";
									}
									else{
										//sinon on affiche un petit message
										echo"(bypass)";
									}

								?>		
								</td>
								<td>
								<?PHP
										echo''.$flux_solaire.' W/m²';
									?>	
								</td>
							  </tr>
							  </tbody>
						 </table>
					</div>		<!-- /span > -->

					<div class="span6">
						<table class="table table-bordered table-striped ">
							<thead>
							  <tr>
								<th colspan="5">Cameras</th>
							  </tr>
							</thead>
							<tbody>
							  <tr>
								<td>
								<a href="camera.php"><img src="webcam/ext_tn.jpg" alt="ext"  width="110"></a>
								<a href="camera.php"><img src="webcam/porche_tn.jpg" alt="porche" width="110"></a>
								<a href="camera.php"><img src="webcam/sejour_tn.jpg" alt="sejour" width="110"></a>
								<a href="camera.php"><img src="webcam/garage_tn.jpg" alt="garage" width="110"></a>
								</td>
							  </tr>
							  </tbody>
						 </table>
					</div>		<!-- /span6 > -->

					<div class="span6">
						 <div class="center" >
							<!--<object type="application/x-shockwave-flash" data="http://swf.yowindow.com/yowidget3.swf" width="400" >
								<param name="movie" value="http://swf.yowindow.com/yowidget3.swf"/>
								<param name="allowfullscreen" value="true"/>
								<param name="wmode" value="opaque"/>
								<param name="bgcolor" value="#FFFFFF"/>
								<param name="flashvars" value="location_id=gn:2972315&amp;location_name=cadours&amp;time_format=24&amp;unit_system=metric&amp;lang=fr&amp;background=#FFFFFF&amp;mini_temperature=true&amp;mini_time=true&amp;mini_locationBar=false&amp;mini_momentBar=true&amp;copyright_bar=false"/>
							</object>
							-->
						</div>

						<table class="table table-bordered table-striped ">
							<thead>
							  <tr>
								<th width=50%>Conso Elec.</th>
								<th width=50%>Etats capteurs</th>
							  </tr>
							</thead>
							<tbody>
							  <tr>
								<td>
									<?PHP
										echo'<span rel="tooltip" title="'.$data_teleinfo_date[0].'">'.$data_consojour[0].' W  </span>';
										// si on est en heure pleine
										if($data_tarif[0] == "HP..")
											// on affiche HP en rouge
											echo"<span class=\"label label-important\">HP</span>";
										else
											// sinon on affiche HC en vert
											echo"<span class=\"label label-success\">HC</span>";
									?>
								</td>
								<td>
									<span class="label <?PHP echo"$str_ledTe"; ?>">Ext</span>
									<span class="label <?PHP echo"$str_ledTi"; ?>">Int</span>
									<span class="label <?PHP echo"$str_ledPC"; ?>">PC</span>
									<span class="label <?PHP echo"$str_icon_lededf"; ?>">Téléinfo</span>
								</td>
							  </tr>
							  </tbody>
						 </table>
					</div>		<!-- /span6 > -->
				</div>		<!-- /fluid > -->

				<div class="row-fluid">
					<div class="span6" id="voletroulant_tab">
						<table class="table table-bordered table-striped table-condensed">
							<thead>
							  <tr>
								<th colspan="4">Etat volets roulants</th>
							  </tr>
							</thead>
						<tbody>
							<tr>
								<th colspan="3">
									<select name="mode_vr" class="selectpicker span6 show-menu-arrow" id="sel_etat_vr">
									  <option value="manuel" <?PHP if($mode_vr == 'Manuel'){echo 'selected';} ?>>Manuel</option>
									  <option value="ouverts" <?PHP if($mode_vr == 'Ouverts'){echo 'selected';} ?>>Tous ouverts</option>
									  <option value="fermes" <?PHP if($mode_vr == 'Fermés'){echo 'selected';} ?>>Tous Fermés</option>
									  <option value="automatique" <?PHP if($mode_vr == 'Automatique'){echo 'selected';} ?>>Auto</option>
									</select>
								</th>
								<th colspan="1">
									<div id="formulaire_VR" class="btn btn-primary btn-small">Appliquer</div>
								</th>
							</tr>	
						  <tr>
							<td class="tdhead">Bureau</td>
							<td class="tdhead">Séjour</td>
							<td class="tdhead">Ch. M</td>
							<td class="tdhead">Ch. JF</td>
						  </tr>
						  <tr>
							<td> 
								<div id="btn_vr_bureau">
									<?PHP
										if($mode_vr == 'Manuel') echo '<div id="create-button" name="btn_vr_bureau" class="btn btn-primary btn-small" type="button">Changer</div>';
										else {echo'<div  class="switch switch-small" id="vr_bureau" data-on="success" data-off="danger" data-on-label="Ouvert" data-off-label="Fermé"><input type="checkbox"  name="chk"'; echo affiche_etat($etat_vr_bureau, $mode_vr); echo'/></div>';}
									?>
								</div>
							</td>
							<td>
								<div id="btn_vr_sejour">
									<?PHP
										if($mode_vr == 'Manuel') echo '<div id="create-button" name="btn_vr_sejour" class="btn btn-primary btn-small" type="button">Changer</div>';
										else {echo'<div  class="switch switch-small" id="vr_sejour" data-on="success" data-off="danger" data-on-label="Ouvert" data-off-label="Fermé"><input type="checkbox"  name="chk"'; echo affiche_etat($etat_vr_salon, $mode_vr); echo'/></div>';}
									?>
								</div>
							</td>
							<td>
								<div id="btn_vr_chm">
									<?PHP
										if($mode_vr == 'Manuel') echo '<div id="create-button" name="btn_vr_chm" class="btn btn-primary btn-small" type="button">Changer</div>';
										else {echo'<div  class="switch switch-small" id="vr_chm" data-on="success" data-off="danger" data-on-label="Ouvert" data-off-label="Fermé"><input type="checkbox"  name="chk"'; echo affiche_etat($etat_vr_chm, $mode_vr); echo'/></div>';}
									?>
								</div>
							</td>
							<td>	
								<div id="btn_vr_chjf">
									<?PHP
										if($mode_vr == 'Manuel') echo '<div id="create-button" name="btn_vr_chjf" class="btn btn-primary btn-small" type="button">Changer</div>';
										else {echo'<div  class="switch switch-small" id="vr_chjf" data-on="success" data-off="danger" data-on-label="Ouvert" data-off-label="Fermé"><input type="checkbox"  name="chk"'; echo affiche_etat($etat_vr_chjf, $mode_vr); echo'/></div>';}
									?>
								</div>
							</td>
						  </tr>
						</tbody>
					  </table>
					</div> <!-- /span6 > -->
					<div class="span6">
						<div id="chauffage_tab"></div>
					  <table class="table table-bordered table-striped table-condensed">
						<thead>
						  <tr>
							<th colspan="5">Chauffage</th>
						  </tr>
						</thead>
						<tbody>
							<tr>
								<th width=20%>
									<div  id="btn_chauffage_automan" class="switch switch-small" data-on="success" data-off="danger" data-on-label="Auto" data-off-label="Manuel">
										<input type="checkbox" id="etat_chauffage_automan" name="chk" <?PHP echo $etat_chauffage_status; ?>/>
									</div>
								</th>
								<th width=20%>
									<div  id="btn_chauffage_onoff"class="switch switch-small" data-on="success" data-off="danger" data-on-label="Marche" data-off-label="Arrêt">
										<input type="checkbox" id="etat_chauffage_onoff" name="chk" <?PHP echo $mode_chauffage_status; ?>/>
									</div>
								</th>
								<th width=40%>
									<div class="input-append input-prepend">
										<span class="add-on">puissance :</span>
										<input class="span2" id="appendedPrependedInput" type="text" placeholder="00">
										<span class="add-on">%</span>
										<!--<button id="formulaire_chauffe" class="btn" type="button">Appliquer</button> -->
									</div>
								</th>
								<th colspan="2" width=20%>
									<input class="btn btn-primary btn-small" id="formulaire_chauffe" type="button" value="Appliquer"/>
								</th>
							</tr>	
						  <tr>
							<td class="tdhead">Poele</td>
							<td class="tdhead">Consigne</td>
							<td class="tdhead">Granulés</td>
							<td class="tdhead">démarrage</td>
							<td class="tdhead">fin</td>
						  </tr>
						  <tr>
							<td>
								<?PHP echo $puissance_poele; ?>
							</td>
							<td>
								<?PHP echo $temp_consigne;	?>									
							</td>
							<td>
								<?PHP echo $pellets_nvg.' %' ; ?>
							</td>
							<td>
								<?PHP //echo $h_demarr;	?>
							</td>
							<td>
								<?PHP //echo $h_fin;	?>
							</td>
						  </tr>
						</tbody>
					  </table>
					  
					</div> <!-- /span6 > -->
					
				</div>		<!-- /fluid > -->
			</div><!-- /container -->
		</div><!-- /wrap -->
		<script>
				$('.selectpicker').selectpicker();

				$(document).on('click', "#create-button", function(){ 
					//alert($(this).attr("name"));
					switch ($(this).attr("name")){
						case 'btn_vr_bureau':
							$('#btn_vr_bureau').html('<div  class="switch switch-small" id="vr_bureau" data-on="success" data-off="danger" data-on-label="Ouvert" data-off-label="Fermé"><input type="checkbox"  name="chk"/></div>');
							$('#vr_bureau').bootstrapSwitch();$('#vr_bureau').bootstrapSwitch('setActive', true);
							break;
						case 'btn_vr_sejour':
							$('#btn_vr_sejour').html('<div  class="switch switch-small" id="vr_sejour" data-on="success" data-off="danger" data-on-label="Ouvert" data-off-label="Fermé"><input type="checkbox"  name="chk"/></div>');
							$('#vr_sejour').bootstrapSwitch();$('#vr_sejour').bootstrapSwitch('setActive', true);
							break;
						case 'btn_vr_chm':
							$('#btn_vr_chm').html('<div  class="switch switch-small" id="vr_chm" data-on="success" data-off="danger" data-on-label="Ouvert" data-off-label="Fermé"><input type="checkbox"  name="chk"/></div>');
							$('#vr_chm').bootstrapSwitch();$('#vr_chm').bootstrapSwitch('setActive', true);
							break;
						case 'btn_vr_chjf':
							$('#btn_vr_chjf').html('<div  class="switch switch-small" id="vr_chjf" data-on="success" data-off="danger" data-on-label="Ouvert" data-off-label="Fermé"><input type="checkbox"  name="chk"/></div>');
							$('#vr_chjf').bootstrapSwitch();$('#vr_chjf').bootstrapSwitch('setActive', true);
							break;
					}
				});
				
				$(document).on('change', "#sel_etat_vr", function(){
				//$('#sel_etat_vr').change(function() {
					//alert('The option with value ' + $(this).val() + ' and text ' + $(this).text() + ' was selected.');
					switch ($(this).val()) { 
						case 'manuel': 
							//document.getElementById('test_btn').innerHTML = '<div id="create-button" class="btn btn-primary btn-small" type="button">Create</div>';
							$('#vr_bureau').bootstrapSwitch('setActive', false);$('#btn_vr_bureau').html('<div id="create-button" name="btn_vr_bureau" class="btn btn-primary btn-small" type="button">Changer</div>');
							$('#vr_sejour').bootstrapSwitch('setActive', false);$('#btn_vr_sejour').html('<div id="create-button" name="btn_vr_sejour" class="btn btn-primary btn-small" type="button">Changer</div>');
							$('#vr_chm').bootstrapSwitch('setActive', false);$('#btn_vr_chm').html('<div id="create-button" name="btn_vr_chm" class="btn btn-primary btn-small" type="button">Changer</div>');
							$('#vr_chjf').bootstrapSwitch('setActive', false);$('#btn_vr_chjf').html('<div id="create-button" name="btn_vr_chjf" class="btn btn-primary btn-small" type="button">Changer</div>');
							break;
						
						case 'fermes':
							$('#btn_vr_bureau').html('<div  class="switch switch-small" id="vr_bureau" data-on="success" data-off="danger" data-on-label="Ouvert" data-off-label="Fermé"><input type="checkbox"  name="chk"/></div>');$('#vr_bureau').bootstrapSwitch('setActive', false); $('#vr_bureau').bootstrapSwitch('setState', false); 
							$('#btn_vr_sejour').html('<div  class="switch switch-small" id="vr_sejour" data-on="success" data-off="danger" data-on-label="Ouvert" data-off-label="Fermé"><input type="checkbox"  name="chk"/></div>');$('#vr_sejour').bootstrapSwitch('setActive', false); $('#vr_sejour').bootstrapSwitch('setState', false); 
							$('#btn_vr_chm').html('<div  class="switch switch-small" id="vr_chm" data-on="success" data-off="danger" data-on-label="Ouvert" data-off-label="Fermé"><input type="checkbox"  name="chk"/></div>');$('#vr_chm').bootstrapSwitch('setActive', false); $('#vr_chm').bootstrapSwitch('setState', false); 
							$('#btn_vr_chjf').html('<div  class="switch switch-small" id="vr_chjf" data-on="success" data-off="danger" data-on-label="Ouvert" data-off-label="Fermé"><input type="checkbox"  name="chk"/></div>');$('#vr_chjf').bootstrapSwitch('setActive', false); $('#vr_chjf').bootstrapSwitch('setState', false);
							$('#vr_bureau').bootstrapSwitch();$('#vr_sejour').bootstrapSwitch();$('#vr_chm').bootstrapSwitch();$('#vr_chjf').bootstrapSwitch();
							break;
						
						case 'ouverts':
							$('#btn_vr_bureau').html('<div  class="switch switch-small" id="vr_bureau" data-on="success" data-off="danger" data-on-label="Ouvert" data-off-label="Fermé"><input type="checkbox"  name="chk"/></div>');$('#vr_bureau').bootstrapSwitch('setActive', false); $('#vr_bureau').bootstrapSwitch('setState', true); 
							$('#btn_vr_sejour').html('<div  class="switch switch-small" id="vr_sejour" data-on="success" data-off="danger" data-on-label="Ouvert" data-off-label="Fermé"><input type="checkbox"  name="chk"/></div>');$('#vr_sejour').bootstrapSwitch('setActive', false); $('#vr_sejour').bootstrapSwitch('setState', true); 
							$('#btn_vr_chm').html('<div  class="switch switch-small" id="vr_chm" data-on="success" data-off="danger" data-on-label="Ouvert" data-off-label="Fermé"><input type="checkbox"  name="chk"/></div>');$('#vr_chm').bootstrapSwitch('setActive', false); $('#vr_chm').bootstrapSwitch('setState', true); 
							$('#btn_vr_chjf').html('<div  class="switch switch-small" id="vr_chjf" data-on="success" data-off="danger" data-on-label="Ouvert" data-off-label="Fermé"><input type="checkbox"  name="chk"/></div>');$('#vr_chjf').bootstrapSwitch('setActive', false); $('#vr_chjf').bootstrapSwitch('setState', true);					
							$('#vr_bureau').bootstrapSwitch();$('#vr_sejour').bootstrapSwitch();$('#vr_chm').bootstrapSwitch();$('#vr_chjf').bootstrapSwitch();
							break;
						
						case 'automatique':
						default:
							$('#btn_vr_bureau').html('<div  class="switch switch-small" id="vr_bureau" data-on="success" data-off="danger" data-on-label="Ouvert" data-off-label="Fermé"><input type="checkbox"  name="chk"/></div>');$('#vr_bureau').bootstrapSwitch('setActive', false); 
							$('#btn_vr_sejour').html('<div  class="switch switch-small" id="vr_sejour" data-on="success" data-off="danger" data-on-label="Ouvert" data-off-label="Fermé"><input type="checkbox"  name="chk"/></div>');$('#vr_sejour').bootstrapSwitch('setActive', false); 
							$('#btn_vr_chm').html('<div  class="switch switch-small" id="vr_chm" data-on="success" data-off="danger" data-on-label="Ouvert" data-off-label="Fermé"><input type="checkbox"  name="chk"/></div>');$('#vr_chm').bootstrapSwitch('setActive', false); 
							$('#btn_vr_chjf').html('<div  class="switch switch-small" id="vr_chjf" data-on="success" data-off="danger" data-on-label="Ouvert" data-off-label="Fermé"><input type="checkbox"  name="chk"/></div>');$('#vr_chjf').bootstrapSwitch('setActive', false);
							$('#vr_bureau').bootstrapSwitch();$('#vr_sejour').bootstrapSwitch();$('#vr_chm').bootstrapSwitch();$('#vr_chjf').bootstrapSwitch();
							break;
					}
				});				
			
				$(document).on('click', "#formulaire_VR", function(){
					var mode_vr = $('#sel_etat_vr').val();
					//alert(' taille #vr_bureau:'+$('#vr_bureau').length + '\n taille #vr_sejour:' + $('#vr_sejour').length + '\n taille #vr_chm:' + $('#vr_chm').length + '\n taille #vr_chjf:'+$('#vr_chjf').length);
					//si on est mode manuel
					if( mode_vr == 'manuel' ) {
						// on teste si le bouton existe ou non , si il n'existe pas c'est que l'utilisateur a choisi de modifier l'état et donc on récupére l'état...
						if($('#vr_bureau').length != 0){var vr_bureau = $('#vr_bureau').bootstrapSwitch('status');}
						if($('#vr_sejour').length != 0){var vr_sejour = $('#vr_sejour').bootstrapSwitch('status');}
						if($('#vr_chm').length != 0){var vr_chm = $('#vr_chm').bootstrapSwitch('status');}
						if($('#vr_chjf').length != 0){var vr_chjf = $('#vr_chjf').bootstrapSwitch('status');} 
					}
					else{
						//dans les autres modes, on récupère l'état des vr
						var vr_bureau = $('#vr_bureau').bootstrapSwitch('status');
						var vr_sejour = $('#vr_sejour').bootstrapSwitch('status');
						var vr_chm = $('#vr_chm').bootstrapSwitch('status');
						var vr_chjf = $('#vr_chjf').bootstrapSwitch('status');
					}
					// alert('mode_vr :'+ mode_vr +'\n\n etat_vr_bureau :'+ vr_bureau+'\n etat_vr_sejour :'+ vr_sejour+'\n etat_vr_chm :'+ vr_chm+'\n etat_vr_chjf :'+ vr_chjf);
					
					// TEST ------------ !!
					// if (id_div==""){
						// document.getElementById(id_div).innerHTML="";
						// return;
					// }
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
							document.getElementById('voletroulant_tab').innerHTML = xmlhttp.responseText;
							$('#voletroulant_tab .switch').bootstrapSwitch();
							$('.selectpicker').selectpicker();
						}
					}
					// on appelle la page php avec les valeurs du formulaire en paramètre
					xmlhttp.open("GET","./myjs/volet_roulant_update.php?mode_vr="+mode_vr+"&vr_bureau="+vr_bureau+"&vr_sejour="+vr_sejour+"&vr_chm="+vr_chm+"&vr_chjf="+vr_chjf,true);
					// xmlhttp.open("GET","./myjs/volet_roulant_update.php",true);
					// on envoie ce que nous a retourné la page php dans la div
					xmlhttp.send();
					return false;
				});

				$('#etat_chauffage_automan').change(function () {
				// $(document).on('click', "#etat_chauffage_automan", function(){
					if($(this).is(":checked")) {
						$('#btn_chauffage_onoff').bootstrapSwitch('setActive', false);
					}else{
						$('#btn_chauffage_onoff').bootstrapSwitch('setActive', true);
					}
				});

				$(document).on('click', "#formulaire_chauffe", function(){
					var etat_chauffage_automan = $('#btn_chauffage_automan').bootstrapSwitch('status');
					var etat_chauffage_onoff = $('#btn_chauffage_onoff').bootstrapSwitch('status');
					var puissance = $('#appendedPrependedInput').val();
					
					
					// alert(' etat_chauffage_automan:'+ etat_chauffage_automan + '\n etat_chauffage_onoff:' + etat_chauffage_onoff + '\n puissance:' + puissance);
					//si on est mode manuel
					if( etat_chauffage_automan == true ) {
						// on est en mode AUTO
						etat_chauffage_status = 3;
					}
					else{ //etat_chauffage_automan == false
						if( etat_chauffage_onoff == true ) {
							// on est en mode MANUEL et ON FORCé
							etat_chauffage_status = 2;
						}
						else{// on est en mode MANUEL et OFF FORCé
							etat_chauffage_status = 1;
						}
					}
					
					//alert('etat_chauffage_status :'+ etat_chauffage_status);
					
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
							document.getElementById('chauffage_tab').innerHTML = xmlhttp.responseText;
							// $('#voletroulant_tab .switch').bootstrapSwitch();
							// $('.selectpicker').selectpicker();
						}
					}
					// on appelle la page php avec les valeurs du formulaire en paramètre
					xmlhttp.open("GET","./php/chauffage_update.php?etat_chauffage_status="+etat_chauffage_status+"&puissance="+puissance,true);
					// xmlhttp.open("GET","./myjs/volet_roulant_update.php",true);
					// on envoie ce que nous a retourné la page php dans la div
					xmlhttp.send();
					return false;
				});
			
		</script>
  </body>
</html>