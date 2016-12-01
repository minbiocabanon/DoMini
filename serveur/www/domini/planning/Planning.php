<?php

class Planning {
         private $joursFr = Array(6=>'Dimanche', 0=>'Lundi', 1=>'Mardi', 2=>'Mercredi', 3=>'Jeudi', 4=>'Vendredi', 5=>'Samedi');
         // Fonction mYSQL
		 // DAYOFWEEK(date)  Returns the weekday index for date (1 = Sunday, 2 = Monday, …, 7 = Saturday). These index values correspond to the ODBC standard.
         private $jourDebut;             // jour de début du planning (0 à 6)
         private $jourFin;               // jour de fin du planning

         private $heureDebut;    // heure de début de chaque jour (en minutes)
         private $heureFin;              // heure de fin de chaque jour (en minutes)

         private $pas;                   // durée d'une case (en minutes)
         private $minutesKeys;
         
         private $contenu;               // contenu général du planning (tableau de PlanningCellule)

         private $tabSemaine;    // stockage des données (tableau initialisé avec des cellules vides)

         const htmlSpace = '&nbsp;';
         const htmlEmptyCell = '<td>&nbsp;</td>';
         const htmlCellOpen = '<td>';
         const htmlCellClose = '</td>';
         const htmlRowOpen = '<tr>';
         const htmlRowClose = '</tr>';
         const htmlTableOpen = '<table class="table table-bordered table-striped table-condensed"><tbody>';
         const htmlTableClose = '</tbody></table>';
         
         const separateurHeure = 'h';

         public function __construct($jourDebut=0, $jourFin=6, $heureDebut=540, $heureFin=1260, $pas=30, $contenu = Array()){
                 $this->jourDebut = $jourDebut;
                 $this->jourFin = $jourFin;
                 $this->heureDebut = $heureDebut;
                 $this->heureFin = $heureFin;
                 $this->pas = $pas;
                 $this->contenu = $contenu;
                 
                 $this->initTableauSemaine($this->contenu);
                 // $this->debugPHPArrays();
                 $this->insererContenus($contenu);
         }
         
    /**
          * Génère un tableau dont les clés sont les heures de début de chaque case (en minutes)
          * Serviront à identifier facilement chaque case du planning
          * @return unknown_type
          */
         private function genererMinutesKeys() {
                 $keys = Array();
                 for ($key=$this->heureDebut; $key<=$this->heureFin; $key+=$this->pas) {
                         $keys[] = $key;
                 }
                 $this->keys = $keys;
                 return $keys;
         }

    /**
          * Génère un tableau correspondant à un jour
          * @return unknown_type
          */
         private function initTableauJour() {
                 if ($this->pas != 0) {
                         $numCells = ($this->heureDebut - $this->heureFin) / $this->pas;
                 } else {
                         echo 'pas == 0 !!';
                 }
                 $keys = $this->genererMinutesKeys();
                 $tabJour = array_fill_keys($keys, self::htmlEmptyCell);
                 return $tabJour;
         }

         private  function initTableauSemaine() {
                 $this->tabSemaine = Array();
                 $tabJour = $this->initTableauJour();
                 for($i=$this->jourDebut; $i<=$this->jourFin; $i++) {
                         $this->tabSemaine[$i] = $tabJour;
                 }
         }
         
         private function getNumeroCellule($minutesDebut, $minutesFin) {
                 return ($minutesFin - $minutesDebut) / $this->pas;
         }
         
    /**
          * Insère tous les contenus de cellulés envoyés
          * @param $contenuPlanning
          * @return unknown_type
          */
         private function insererContenus($contenuPlanning) {
                 foreach ($contenuPlanning as $contenuCellule) {
                         $this->insererContenu($contenuCellule);
                 }
         }
         
    /**
          * Insère le contenu d'une cellule précise
          * @param $contenuCellule
          * @return unknown_type
          */
         private function insererContenu($contenuCellule) {
                 // ajout de la cellule fusionnée
                 $duree = $this->getNumeroCellule($contenuCellule->heureDebut, $contenuCellule->heureFin);
                 $contenu = $contenuCellule->contenu.'<br />';
                 //$contenu .= $this->convertMinutesEnHeuresMinutes($contenuCellule->heureDebut);
                 //$contenu .= ' - '.$this->convertMinutesEnHeuresMinutes($contenuCellule->heureFin);
                 
                 $this->tabSemaine[$contenuCellule->numJour][$contenuCellule->heureDebut] = $this->genererCelluleHTML($contenu, $duree, '', $contenuCellule->bgColor, $contenuCellule->IdCell);
                 
                 // suppression du contenu suivant
                 $key = $contenuCellule->heureDebut;
                 for ($cpt = $duree-1; $cpt>0; $cpt--) {
                         $key += $this->pas;
                         $this->tabSemaine[$contenuCellule->numJour][$key] = '';                 
                 }
         }
         
         /* Affichage */ 
         public function debugPHPArrays() {
                 echo '<pre>';
                 print_r($this->tabSemaine);
                 echo '</pre>';          
         }
         
         public function genererHtmlTable() {
                 $htmlTable = self::htmlTableOpen;
                 $htmlTable .= $this->genererBandeauJours();
				 $htmlTable .= $this->genererBandeauDateJours();
				 $htmlTable .= $this->genererBandeauTypeJours();
                 
                 $key = $this->heureDebut;
                 $keyEnd = $this->heureFin;
                 for ($key; $key < $keyEnd; $key+=$this->pas) {
                         $htmlTable .= self::htmlRowOpen;
                         $htmlTable .= '<td>'.$this->convertMinutesEnHeuresMinutes($key).'</br> '.$this->convertMinutesEnHeuresMinutes($key+$this->pas).'</td>';
                         foreach ($this->tabSemaine as $tabHeures) {
                                 $htmlTable .= $tabHeures[$key];
                         }
                         $htmlTable .= self::htmlRowClose;
                 }
                 
                 $htmlTable .= self::htmlTableClose;
                 return $htmlTable;
         }
         
         public function afficherHtmlTable() {
                 echo $this->genererHtmlTable();
         }
         
         private function genererBandeauJours() {
                 $daysLine = self::htmlRowOpen;
                 $daysLine .= $this->genererCelluleHTML(self::htmlSpace);
                 $day = $this->jourDebut;
                 while ($day <= $this->jourFin){
                         $daysLine .= $this->genererCelluleHTML($this->jourFr($day), '', '');
                         $day++;
                 }
                 $daysLine .= self::htmlRowClose;
                 return $daysLine;
         }
		 
        private function genererBandeauDateJours() {
				// on déclare ses variables comme globales car déjà définies dans les fichiers includes mais hors de portée.
				global $num_semaine, $host, $login, $passe, $bdd;
				// requete MySQL pour obtenir les données de la BDD
				//echo" $host, $login, $passe, $bdd \n";
					$link = mysqli_connect($host,$login,$passe,$bdd);
	if (!$link) {
		die('Erreur de connexion (' . mysqli_connect_errno() . ') '
				. mysqli_connect_error());
	}
				

                 $daysLine = self::htmlRowOpen;
                 $daysLine .= $this->genererCelluleHTML(self::htmlSpace);
                 $day = $this->jourDebut;
                 while ($day <= $this->jourFin){
					// on récupère la date
					//requete pour sélectionner toutes les tranches de 30 minutes pour la semaine en cours
					$SQL="SELECT DATE_FORMAT(`date`, '%d/%m') as day_month, WEEKDAY(now()) as dayweek, WEEKOFYEAR(now()) as weekyear
							FROM `calendrier_30min` 
							WHERE WEEKDAY(`date`) = $day
							AND week( date( `date` ) , 3 ) = $num_semaine
							LIMIT 0,1"; 
							//AND week( date( date ) , 3 ) = $num_semaine
					//on lance la requete
					$RESULT = @mysqli_query($link, $SQL);
					// lecture du resultat de la requete
					$myrow=@mysqli_fetch_array($RESULT);
					//on récupère la température précédente
					$date_day_month = $myrow["day_month"];	
					$date_day_week = $myrow["dayweek"];
					$date_week_year = $myrow["weekyear"];
					
					//si le jour en cours est traité, on fait un background vert
					if($date_day_week == $day && $date_week_year == $num_semaine)
						$colorcell = "#24CC24";
					else
						$colorcell = "";
					
					 $daysLine .= $this->genererCelluleHTML($date_day_month, '', '', $colorcell);
					 $day++;
					 mysqli_free_result($RESULT) ;
                 }
				 mysqli_close($link);
                 $daysLine .= self::htmlRowClose;
                 return $daysLine;
         }		 
 
         private function genererBandeauTypeJours() {
				// on déclare ses variables comme globales car déjà définies dans les fichiers includes mais hors de portée.
				global $num_semaine, $host, $login, $passe, $bdd;
				// requete MySQL pour obtenir les données de la BDD
				//echo" $host, $login, $passe, $bdd \n";
					$link = mysqli_connect($host,$login,$passe,$bdd);
	if (!$link) {
		die('Erreur de connexion (' . mysqli_connect_errno() . ') '
				. mysqli_connect_error());
	}
				
		 
                 $daysLine = self::htmlRowOpen;
                 $daysLine .= $this->genererCelluleHTML(self::htmlSpace);
                 $day = $this->jourDebut;
                 while ($day <= $this->jourFin) {
					// on récupère le type de jour pour sélectionner le bon jour dans la liste déroulante
					$SQL="	SELECT `type_jour`
							FROM `calendrier` 
							WHERE WEEKDAY(`date`) = $day
							AND week(`date` , 3 ) = $num_semaine 
							LIMIT 0 , 1"; 
					//on lance la requete
					$RESULT = @mysqli_query($link, $SQL);
					// lecture du resultat de la requete
					$myrow=@mysqli_fetch_array($RESULT);
					$typejour = $myrow["type_jour"];
					
					// on récupère les id de début du jour pour AJAX en cas de mis jour par l'utilisateur (click sur cellule)
					//AND `heure_debut` >= '05:30:00'
					// AND `heure_debut` >= SEC_TO_TIME($this->heureDebut * 60)
					$SQL="	SELECT `id`
							FROM `calendrier_30min` 
							WHERE WEEKDAY(`date`) = $day
							AND week( `date` , 3 ) = $num_semaine
							AND `heure_debut` >= SEC_TO_TIME($this->heureDebut * 60)
							LIMIT 1"; 							
							// ORDER BY `id` ASC
					//on lance la requete
					$RESULT = @mysqli_query($link, $SQL);
					// lecture du resultat de la requete
					$myrow=@mysqli_fetch_array($RESULT);
					$id_debut = $myrow["id"];
					
					// on récupère les id de fin du jour pour AJAX en cas de mis jour par l'utilisateur (click sur cellule)
					// AND `heure_fin` <= '23:30:00'
					// AND `heure_fin` <= SEC_TO_TIME($this->heureFin)
					$SQL="	SELECT `id`
							FROM `calendrier_30min` 
							WHERE WEEKDAY(`date`) = $day
							AND week( `date` , 3 ) = $num_semaine
							AND `heure_fin` <= SEC_TO_TIME($this->heureFin * 60)
							ORDER BY `id` DESC
							LIMIT 1"; 
					//on lance la requete
					$RESULT = @mysqli_query($link, $SQL);
					// lecture du resultat de la requete
					$myrow=@mysqli_fetch_array($RESULT);
					$id_fin = $myrow["id"];

					// on récupère l'id du jour en cours
					$SQL="	SELECT `id`  
						FROM `calendrier` 
						WHERE WEEKDAY( `date` ) = $day
						AND week( `date` , 3 ) = $num_semaine"; 
					//on lance la requete
					$RESULT = @mysqli_query($link, $SQL);
					// lecture du resultat de la requete
					$myrow=@mysqli_fetch_array($RESULT);
					$idjour = $myrow["id"];					
					
					// on écrit le code html pour faire une liste déroulante et on ajoute SELECTED sur la bonne option( type de jour défini dans le calendrier)
					$contenucell = "<select class=\"span8\" name=\"type_jour\" onchange=\"ChangeTypeJour(this.value, $idjour, $id_debut, $id_fin)\" ><option value=\"T\" ";
					if($typejour == "T") { $contenucell .= "selected=\"selected\"";}
					$contenucell .=">Travail</option><option value=\"W\"";
					if($typejour == "W") { $contenucell .= "selected=\"selected\"";}
					$contenucell .=">WeekEnd</option><option value=\"S\"";
					if($typejour == "S") { $contenucell .= "selected=\"selected\"";}
					$contenucell .=">Special</option><option value=\"A\"";
					if($typejour == "A") { $contenucell .= "selected=\"selected\"";}
					$contenucell .=">Absent</option></select>";
					// on génére la cellule
					$daysLine .= $this->genererCelluleHTML($contenucell, '', '', '');
					$day++;
					mysqli_free_result($RESULT) ;
                 }
				 mysqli_close($link);
                 $daysLine .= self::htmlRowClose;
                 return $daysLine;
         }
 
		/**
		* Génère une ligne HTML contenant le libellé des jours utilisés dans le planning
		* @param $contenuCellule
		* @param $colspan
		* @param $class
		* @param $bgColor
		* @param $IdCell
		* @return unknown_type
		*/
         private function genererCelluleHTML($contenuCellule, $colspan = '', $class = '', $bgColor = '', $IdCell = '') {
				$celluleHTML = '<td';
				if (!empty($colspan)) 
						 $celluleHTML .= ' rowspan="'.$colspan.'"';                      
				if (!empty($class)) 
						 $celluleHTML .= ' class="'.$class.'"';
				if (!empty($bgColor)) 
						 $celluleHTML .= ' bgcolor="'.$bgColor.'"';
				if (!empty($IdCell)) 
						 $celluleHTML .= ' id="'.$IdCell.'"';						 
                 $celluleHTML .= '/>';
                 $celluleHTML .= $contenuCellule;
                 $celluleHTML .= '</td>';
                 return $celluleHTML;
         }
         
    /**
          * Renvoie le libellé d'un jour en Français
          * @param $dayNum
          * @return unknown_type
          */
         private function jourFr($dayNum) {
                 return $this->joursFr[$dayNum];
         }

         private function convertMinutesEnHeuresMinutes($minutes) {
                 $heure = floor($minutes / 60);
                 $minutes = ($minutes % 60);
                 $minutes = str_pad($minutes, 2, '0', STR_PAD_LEFT);
                 return ($heure. self::separateurHeure .$minutes);
         }
}

class PlanningCellule {
         private $data;
         
         public function __construct($numJour, $IdCell, $heureDebut, $heureFin, $bgColor, $contenu) {
			$this->numJour = $numJour;
			$this->IdCell = $IdCell;
			$this->heureDebut = $heureDebut;
			$this->heureFin = $heureFin;
			$this->bgColor = $bgColor;
			$this->contenu = $contenu;
     }
     
     public function __set($name, $value) {
         if ($name == 'heureDebut' || $name == 'heureFin') {
                 $tabHeure = explode(':', $value);
                 $value = (int)$tabHeure[0];
                 if ($tabHeure[1] == 30)
                         $value += 0.5;
                 $value = $value*60;
         }
         $this->data[$name] = $value;
     }

     public function __get($name) {
         if (array_key_exists($name, $this->data)) {
             return $this->data[$name];
         }

         $trace = debug_backtrace();
         trigger_error(
             'Propriété non-définie via __get(): ' . $name .
             ' dans ' . $trace[0]['file'] .
             ' à la ligne ' . $trace[0]['line'],
             E_USER_NOTICE);
         return null;
     }
     
     public function __toString() {
         $str = 'heure début : '.$this->heureDebut."<br />\n";
         $str .= 'heure fin : '.$this->heureFin."<br />\n";
         $str .= 'couleur : '.$this->bgColor."<br />\n";
         $str .= 'contenu : '.$this->contenu."<br />\n";
         return $str;
     }
}
?>