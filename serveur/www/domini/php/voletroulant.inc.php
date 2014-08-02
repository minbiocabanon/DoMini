<?php
	function Convert_etat($var){
		switch($var){
			case "Fermé":
				return 0;
			break;
			case "Ouvert":
				return 1;
			break;
			case "Mi-ombre":
				return 2;
			break;	
			case "Immobile":
				return 3;
			break;				
		}	
	}
	
	function Convert_mode($var){
		switch($var){
			case "automatique":
				return 0;
			break;
			case "fermes":
				return 1;
			break;
			case "ouverts":
				return 2;
			break;	
			case "manuel":
				return 3;
			break;			
		}	
	}
	
	function reverse_convert_mode($var){
		switch($var){
			case 0:
				return ("Automatique");
			break;
			case 1:
				return ("Fermés");
			break;
			case 2:
				return ("Ouverts");
			break;	
			case 3:
				return ("Manuel");
			break;
		}	
	}
	
	function reverse_convert_etat($var){
		switch($var){
			case 0:
				return ("Fermé");
			break;
			case 1:
				return ("Ouvert");
			break;
			case 2:
				return ("Mi-ombre");
			break;	
			case 3:
				return ("Immobile");
			break;
		}	
	}

	function affiche_etat($etat, $mode){
		switch($etat){
			case "Fermé":
				$chaine_etat = "";
			break;
			case "Ouvert":
				$chaine_etat = "checked";
			break;
			case "Mi-ombre":
				$chaine_etat = "checked";
			break;	
			case "Immobile":
				$chaine_etat = "checked";
			break;				
		}
		
		switch($mode){
			case "Automatique":
				$chaine_etat .= " disabled";
			break;
			case "Fermés":
				$chaine_etat .= " disabled";
			break;
			case "Ouverts":
				$chaine_etat .= " disabled";
			break;	
			case "Manuel":
				$chaine_etat .= " ";
			break;
		}
		return $chaine_etat;
	}	
?>
