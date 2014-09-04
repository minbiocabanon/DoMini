<?PHP
	include("../infos/config.inc.php"); // on inclu le fichier de config 
	$status = 0;
	$ping = 0;
	
	//	on test si google est joignable
	if (!$sock = @fsockopen('www.google.fr', 80, $num, $error, 5)){
		// si non on met $status à 0
		echo 'HORS LIGNE</br>';
		$status = 0;
	}
	else{
		// si non on met $status à 1
		echo 'OK</br>';
		$status = 1;
		
		// on fait une mesure du ping
		$ping_res = shell_exec('ping -c 4 www.google.com');
		$ping = between ('time=', ' ms', $ping_res);
		echo 'ping= '.$ping.' ms';
			   
	}
	
	// requete MySQL pour stocker les résultats les données de la BDD
	// echo" </br>$host, $login, $passe, $bdd ";
	@mysql_connect($host,$login,$passe) or die("Impossible de se connecter à la base de données");
	@mysql_select_db("$bdd") or die("Impossible de se connecter à la base de données");
	//requete pour récupérer la dernière consommation instantanée

	//Envoie de la requete
	$SQL="INSERT INTO  `domotique`.`internet_connex` (`id`, `date_time`, `status`, `ping`) VALUES ( NULL , NOW(), '$status',  '$ping') ";
	//Execution de la requete
	mysql_query($SQL) or die('Erreur SQL !'.$SQL.'<br>'.mysql_error());
	//on quitte la BDD
	mysql_close();
	

	
	function after ($this, $inthat)
    {
        if (!is_bool(strpos($inthat, $this)))
        return substr($inthat, strpos($inthat,$this)+strlen($this));
    };

    function before ($this, $inthat)
    {
        return substr($inthat, 0, strpos($inthat, $this));
    };

    function between ($this, $that, $inthat)
    {
        return before ($that, after($this, $inthat));
    };
	
?>