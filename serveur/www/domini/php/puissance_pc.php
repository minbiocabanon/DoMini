<?php 
// requete MySQL pour obtenir les données de la BDD
//echo" $host, $login, $passe, $bdd \n";
$link = mysqli_connect($host,$login,$passe,$bdd);

// ------------------- Récupère puissance pc actuelle -------------------------------------
//Requete pour déterminer la valeur moyenne sur les dernières 20 minutes
$SQL="SELECT date_time, puissance
FROM puissance_pc
WHERE date_time >= SUBTIME( NOW( ) ,  '00:15:00' ) 
LIMIT 0,1						
"; 
// //Envoie de la requete
$RESULT = @mysqli_query($link, $SQL);
// lecture du resultat de la requete
$myrow=@mysqli_fetch_array($RESULT); 
//on récupère la dernière température relevée
$puissance_pc = $myrow["puissance"];

//on quitte la BDD
mysqli_free_result($RESULT);
mysqli_close($link);
?>