<?php

class Planning {
         private $joursFr = Array(0=>'Janvier', 1=>'F�vrier', 2=>'Mars', 3=>'Avril', 4=>'Mai', 5=>'Juin', 6=>'Juillet', 7=>'Aout', 8=>'Septembre', 9=>'Octobre', 10=>'Novembre', 11=>'D�cembre',);
         // Fonction mYSQL
		 // DAYOFWEEK(date)  Returns the weekday index for date (1 = Sunday, 2 = Monday, �, 7 = Saturday). These index values correspond to the ODBC standard.
         private $moisDebut;             // mois de d�but du planning (0 � 11)
         private $moisFin;               // jour de fin du planning

         private $jourDebut;    		// jour de d�but de chaque jour (en minutes)
         private $jourFin;              // jour de fin de chaque jour (en minutes)

         private $pas;					// dur�e d'une case (en jour)
         private $minutesKeys;
         
         private $contenu;              // contenu g�n�ral du planning (tableau de PlanningCellule)

         private $tabAnnee;    			// stockage des donn�es (tableau initialis� avec des cellules vides)

         const htmlSpace = '&nbsp;';
         const htmlEmptyCell = '<td>&nbsp;</td>';
         const htmlCellOpen = '<td>';
         const htmlCellClose = '</td>';
         const htmlRowOpen = '<tr>';
         const htmlRowClose = '</tr>';
         const htmlTableOpen = '<table class="tabPlanning">';
         const htmlTableClose = '</table>';
         
         const separateurHeure = 'h';

         public function __construct($moisDebut=0, $moisFin=6, $jourDebut=540, $jourFin=1260, $pas=30, $contenu = Array()){
                 $this->moisDebut = $moisDebut;
                 $this->moisFin = $moisFin;
                 $this->jourDebut = $jourDebut;
                 $this->jourFin = $jourFin;
                 $this->pas = $pas;
                 $this->contenu = $contenu;
                 
                 $this->initTableauSemaine($this->contenu);
                 // $this->debugPHPArrays();
                 $this->insererContenus($contenu);
         }
         
    /**
          * G�n�re un tableau dont les cl�s sont les heures de d�but de chaque case (en minutes)
          * Serviront � identifier facilement chaque case du planning
          * @return unknown_type
          */
         private function genererMinutesKeys() {
                 $keys = Array();
                 for ($key=$this->jourDebut; $key<=$this->jourFin; $key+=$this->pas) {
                         $keys[] = $key;
                 }
                 $this->keys = $keys;
                 return $keys;
         }

    /**
          * G�n�re un tableau correspondant � un jour
          * @return unknown_type
          */
         private function initTableauJour() {
                 if ($this->pas != 0) {
                         $numCells = ($this->jourDebut - $this->jourFin) / $this->pas;
                 } else {
                         echo 'pas == 0 !!';
                 }
                 $keys = $this->genererMinutesKeys();
                 $tabMois = array_fill_keys($keys, self::htmlEmptyCell);
                 return $tabMois;
         }

         private  function initTableauSemaine() {
                 $this->tabAnnee = Array();
                 $tabMois = $this->initTableauJour();
                 for($i=$this->moisDebut; $i<=$this->moisFin; $i++) {
                         $this->tabAnnee[$i] = $tabMois;
                 }
         }
         
         private function getNumeroCellule($minutesDebut, $minutesFin) {
                 return ($minutesFin - $minutesDebut) / $this->pas;
         }
         
    /**
          * Ins�re tous les contenus de cellul�s envoy�s
          * @param $contenuPlanning
          * @return unknown_type
          */
         private function insererContenus($contenuPlanning) {
                 foreach ($contenuPlanning as $contenuCellule) {
                         $this->insererContenu($contenuCellule);
                 }
         }
         
    /**
          * Ins�re le contenu d'une cellule pr�cise
          * @param $contenuCellule
          * @return unknown_type
          */
         private function insererContenu($contenuCellule) {
                 // ajout de la cellule fusionn�e
                 $duree = $this->getNumeroCellule($contenuCellule->jourDebut, $contenuCellule->jourFin);
                 $contenu = $contenuCellule->contenu.'<br />';
                 //$contenu .= $this->convertMinutesEnHeuresMinutes($contenuCellule->jourDebut);
                 //$contenu .= ' - '.$this->convertMinutesEnHeuresMinutes($contenuCellule->jourFin);
                 
                 $this->tabAnnee[$contenuCellule->numJour][$contenuCellule->jourDebut] = $this->genererCelluleHTML($contenu, $duree, '', $contenuCellule->bgColor, $contenuCellule->IdCell);
                 
                 // suppression du contenu suivant
                 $key = $contenuCellule->jourDebut;
                 for ($cpt = $duree-1; $cpt>0; $cpt--) {
                         $key += $this->pas;
                         $this->tabAnnee[$contenuCellule->numJour][$key] = '';                 
                 }
         }
         
         /* Affichage */ 
         public function debugPHPArrays() {
                 echo '<pre>';
                 print_r($this->tabAnnee);
                 echo '</pre>';          
         }
         
         public function genererHtmlTable() {
                 $htmlTable = self::htmlTableOpen;
                 $htmlTable .= $this->genererBandeauMois();
				 //$htmlTable .= $this->genererBandeauDateJours();
				 //$htmlTable .= $this->genererBandeauTypeJours();
                 
                 $key = $this->jourDebut;
                 $keyEnd = $this->jourFin;
                 for ($key; $key <= $keyEnd; $key+=$this->pas) {
                         $htmlTable .= self::htmlRowOpen;
                         $htmlTable .= '<td class="cellHour">'.$key.'</td>';
                         foreach ($this->tabAnnee as $tabJour) {
                                 $htmlTable .= $tabJour[$key];
                         }
                         $htmlTable .= self::htmlRowClose;
                 }
                 $htmlTable .= self::htmlTableClose;
                 return $htmlTable;
         }
         
         public function afficherHtmlTable() {
                 echo $this->genererHtmlTable();
         }
         
         private function genererBandeauMois() {
                 $daysLine = self::htmlRowOpen;
                 $daysLine .= $this->genererCelluleHTML(self::htmlSpace);
                 $day = $this->moisDebut;
                 while ($day <= $this->moisFin){
                         $daysLine .= $this->genererCelluleHTML($this->jourFr($day), '', 'cellDay');
                         $day++;
                 }
                 $daysLine .= self::htmlRowClose;
                 return $daysLine;
         }
		 
        private function genererBandeauDateJours() {
				// on d�clare ses variables comme globales car d�j� d�finies dans les fichiers includes mais hors de port�e.
				global $num_semaine, $host, $login, $passe, $bdd;
				// requete MySQL pour obtenir les donn�es de la BDD
				//echo" $host, $login, $passe, $bdd \n";
					$link = mysqli_connect($host,$login,$passe,$bdd);
	if (!$link) {
		die('Erreur de connexion (' . mysqli_connect_errno() . ') '
				. mysqli_connect_error());
	}
				

                 $daysLine = self::htmlRowOpen;
                 $daysLine .= $this->genererCelluleHTML(self::htmlSpace);
                 $day = $this->moisDebut;
                 while ($day <= $this->moisFin){
					// on r�cup�re la date
					//requete pour s�lectionner toutes les tranches de 30 minutes pour la semaine en cours
					$SQL="SELECT DATE_FORMAT(`date`, '%d/%m') as day_month, WEEKDAY(now()) as dayweek, WEEKOFYEAR(now()) as weekyear
							FROM `calendrier_30min` 
							WHERE WEEKDAY(`date`) = $day
							AND week( date( `date` ) , 7 ) = $num_semaine
							LIMIT 0,1"; 
							//AND week( date( date ) , 7 ) = $num_semaine
					//on lance la requete
					$RESULT = @mysqli_query($link, $SQL);
					// lecture du resultat de la requete
					$myrow=@mysqli_fetch_array($RESULT);
					//on r�cup�re la temp�rature pr�c�dente
					$date_day_month = $myrow["day_month"];	
					$date_day_week = $myrow["dayweek"];
					$date_week_year = $myrow["weekyear"];
					$colorcell = "#242424";
					//si le jour en cours est trait�, on fait un background vert
					if($date_day_week == $day && $date_week_year == $num_semaine)
						$colorcell = "#24CC24";
					
					 $daysLine .= $this->genererCelluleHTML($date_day_month, '', 'cellDate', $colorcell);
					 $day++;
					 mysqli_free_result($RESULT) ;
                 }
				 mysqli_close($link);
                 $daysLine .= self::htmlRowClose;
                 return $daysLine;
         }		 
 
         private function genererBandeauTypeJours() {
				// on d�clare ses variables comme globales car d�j� d�finies dans les fichiers includes mais hors de port�e.
				global $num_semaine, $host, $login, $passe, $bdd;
				// requete MySQL pour obtenir les donn�es de la BDD
				//echo" $host, $login, $passe, $bdd \n";
					$link = mysqli_connect($host,$login,$passe,$bdd);
	if (!$link) {
		die('Erreur de connexion (' . mysqli_connect_errno() . ') '
				. mysqli_connect_error());
	}
				
		 
                 $daysLine = self::htmlRowOpen;
                 $daysLine .= $this->genererCelluleHTML(self::htmlSpace);
                 $day = $this->moisDebut;
                 while ($day <= $this->moisFin) {
					// on r�cup�re le type de jour pour s�lectionner le bon jour dans la liste d�roulante
					$SQL="	SELECT `type_jour`
							FROM `calendrier` 
							WHERE WEEKDAY(`date`) = $day
							AND week(`date` , 7 ) = $num_semaine 
							LIMIT 0 , 1"; 
					//on lance la requete
					$RESULT = @mysqli_query($link, $SQL);
					// lecture du resultat de la requete
					$myrow=@mysqli_fetch_array($RESULT);
					$typejour = $myrow["type_jour"];
					
					// on r�cup�re les id de d�but du jour pour AJAX en cas de mis jour par l'utilisateur (click sur cellule)
					$SQL="	SELECT `id`
							FROM `calendrier_30min` 
							WHERE WEEKDAY(`date`) = $day
							AND week( `date` , 7 ) = $num_semaine
							AND `heure_debut` >= '05:30:00'
							LIMIT 1"; 
							//ORDER BY `id` ASC
					//on lance la requete
					$RESULT = @mysqli_query($link, $SQL);
					// lecture du resultat de la requete
					$myrow=@mysqli_fetch_array($RESULT);
					$id_debut = $myrow["id"];
					
					// on r�cup�re les id de fin du jour pour AJAX en cas de mis jour par l'utilisateur (click sur cellule)
					$SQL="	SELECT `id`
							FROM `calendrier_30min` 
							WHERE WEEKDAY(`date`) = $day
							AND week( `date` , 7 ) = $num_semaine
							AND `heure_fin` <= '23:00:00'							
							ORDER BY `id` DESC
							LIMIT 1"; 
					//on lance la requete
					$RESULT = @mysqli_query($link, $SQL);
					// lecture du resultat de la requete
					$myrow=@mysqli_fetch_array($RESULT);
					$id_fin = $myrow["id"];

					// on r�cup�re les id de fin du jour pour AJAX en cas de mis jour par l'utilisateur (click sur cellule)
					$SQL="	SELECT `id`  
						FROM `calendrier` 
						WHERE WEEKDAY( `date` ) = $day
						AND week( `date` , 7 ) = $num_semaine"; 
					//on lance la requete
					$RESULT = @mysqli_query($link, $SQL);
					// lecture du resultat de la requete
					$myrow=@mysqli_fetch_array($RESULT);
					$idjour = $myrow["id"];					
					
					// on �crit le code html pour faire une liste d�roulante et on ajoute SELECTED sur la bonne option( type de jour d�fini dans le calendrier)
					$contenucell = "<select name=\"type_jour\" onchange=\"ChangeTypeJour(this.value, $idjour, $id_debut, $id_fin)\" ><option value=\"T\" ";
					if($typejour == "T") { $contenucell .= "selected=\"selected\"";}
					$contenucell .=">Travail</option><option value=\"W\"";
					if($typejour == "W") { $contenucell .= "selected=\"selected\"";}
					$contenucell .=">WeekEnd</option><option value=\"S\"";
					if($typejour == "S") { $contenucell .= "selected=\"selected\"";}
					$contenucell .=">Special</option><option value=\"A\"";
					if($typejour == "A") { $contenucell .= "selected=\"selected\"";}
					$contenucell .=">Absent</option></select>";
					// on g�n�re la cellule
					$daysLine .= $this->genererCelluleHTML($contenucell, '', 'cellDate', '');
					$day++;
					mysqli_free_result($RESULT) ;
                 }
				 mysqli_close($link);
                 $daysLine .= self::htmlRowClose;
                 return $daysLine;
         }
 
     /**
          * G�n�re une ligne HTML contenant le libell� des jours utilis�s dans le planning
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
          * Renvoie le libell� d'un jour en Fran�ais
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
         
         public function __construct($numJour, $IdCell, $jourDebut, $jourFin, $bgColor, $contenu) {
			$this->numJour = $numJour;
			$this->IdCell = $IdCell;
			$this->jourDebut = $jourDebut;
			$this->jourFin = $jourFin;
			$this->bgColor = $bgColor;
			$this->contenu = $contenu;
     }
     
     public function __set($name, $value) {
         if ($name == 'jourDebut' || $name == 'jourFin') {
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
             'Propri�t� non-d�finie via __get(): ' . $name .
             ' dans ' . $trace[0]['file'] .
             ' � la ligne ' . $trace[0]['line'],
             E_USER_NOTICE);
         return null;
     }
     
     public function __toString() {
         $str = 'jour d�but : '.$this->jourDebut."<br />\n";
         $str .= 'jour fin : '.$this->jourFin."<br />\n";
         $str .= 'couleur : '.$this->bgColor."<br />\n";
         $str .= 'contenu : '.$this->contenu."<br />\n";
         return $str;
     }
}
?>