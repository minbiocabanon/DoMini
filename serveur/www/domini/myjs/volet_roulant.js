/*
 * javascript pour gérer l'état des VR depuis la page index
 */	

/* Fonction qui permet de griser les listes en fonction de l'état des VR sélectionné*/ 

$(document).ready(function() {
	
});

// function request(id_div) {
	// // on récupère les valeurs et checkbox du formulaire
	// var mode_vr = document.forms["formulaire_VR"].elements["mode_vr"].value;
	// var vr_bureau = document.getElementById('vr_bureau').checked;
	// var vr_sejour = document.getElementById('vr_sejour').checked;
	// var vr_chm = document.getElementById('vr_chm').checked;
	// var vr_chjf = document.getElementById('vr_chjf').checked;
	// alert('id: '+ id_div +', mode_vr :'+ mode_vr +', etat_vr_bureau :'+ vr_bureau+', etat_vr_sejour :'+ vr_sejour+', etat_vr_chm :'+ vr_chm+', etat_vr_chjf :'+ vr_chjf);
	
	// // TEST ------------ !!
	// if (id_div==""){
		// document.getElementById(id_div).innerHTML="";
		// return;
	// }
	// if (window.XMLHttpRequest){
		// // code for IE7+, Firefox, Chrome, Opera, Safari
		// xmlhttp=new XMLHttpRequest();
	// }
	// else{
		// // code for IE6, IE5
		// xmlhttp=new ActiveXObject("Microsoft.XMLHTTP");
	// }
	// xmlhttp.onreadystatechange=function(){
		// if (xmlhttp.readyState==4 && xmlhttp.status==200){
			// document.getElementById(id_div).innerHTML = xmlhttp.responseText;
			// $('#voletroulant_tab .switch').bootstrapSwitch();
		// }
	// }
	// // on appelle la page php avec les valeurs du formulaire en paramètre
	// //xmlhttp.open("GET","volet_roulant_update.php?mode_vr"+mode_vr+"&vr_bureau="+vr_bureau+"&vr_sejour="+vr_sejour+"&vr_chm="+vr_chm+"&vr_chjf="+vr_chjf,true);
	// xmlhttp.open("GET","./myjs/volet_roulant_update.php",true);
	// // on envoie ce que nous a retourné la page php dans la div
	// xmlhttp.send();
// }