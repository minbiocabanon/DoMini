-- MariaDB dump 10.19  Distrib 10.5.22-MariaDB, for debian-linux-gnu (x86_64)
--
-- Host: localhost    Database: domotique
-- ------------------------------------------------------
-- Server version	10.5.22-MariaDB-1:10.5.22+maria~ubu2004

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `VMC`
--

DROP TABLE IF EXISTS `VMC`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `VMC` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `date_time` datetime NOT NULL,
  `entete` varchar(4) NOT NULL,
  `ana1` float(5,2) NOT NULL,
  `ana2` float(5,2) NOT NULL,
  `ana3` float(5,2) NOT NULL,
  `ana4` float(5,2) NOT NULL,
  `tag` int(1) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=958635 DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci COMMENT='Temperature VMCDF';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `alarme_pir`
--

DROP TABLE IF EXISTS `alarme_pir`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `alarme_pir` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `date_time` datetime NOT NULL,
  `entete` varchar(4) NOT NULL,
  `etat` int(1) NOT NULL,
  `tag` int(1) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=3045 DEFAULT CHARSET=utf8 COLLATE=utf8_general_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `alarme_statut`
--

DROP TABLE IF EXISTS `alarme_statut`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `alarme_statut` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `mode` varchar(10) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=utf8 COLLATE=utf8_general_ci COMMENT='Contient l''état / mode de l''alarme';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `analog1`
--

DROP TABLE IF EXISTS `analog1`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `analog1` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `date_time` datetime NOT NULL,
  `entete` varchar(4) NOT NULL,
  `ana1` float(5,2) NOT NULL,
  `ana2` float(5,2) NOT NULL,
  `ana3` float(5,2) NOT NULL,
  `ana4` float(5,2) NOT NULL,
  `tag` int(1) NOT NULL,
  PRIMARY KEY (`id`),
  KEY `date_time` (`date_time`)
) ENGINE=MyISAM AUTO_INCREMENT=1138503 DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci COMMENT='Données T° + H° + Pt rosée Extérieures';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `analog2`
--

DROP TABLE IF EXISTS `analog2`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `analog2` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `date_time` datetime NOT NULL,
  `entete` varchar(4) NOT NULL,
  `ana1` float(5,2) NOT NULL COMMENT 'Temp. (°C)',
  `ana2` float(5,2) NOT NULL COMMENT 'Humid. (%)',
  `ana3` float(5,2) NOT NULL COMMENT 'Pt rosée (°C)',
  `ana4` float(5,2) NOT NULL COMMENT 'Tension Batt. (V)',
  `tag` int(1) NOT NULL,
  PRIMARY KEY (`id`),
  KEY `date_time` (`date_time`)
) ENGINE=MyISAM AUTO_INCREMENT=1243000 DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci COMMENT='Données T° + H° + Pt rosée Intérieures';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `analog3`
--

DROP TABLE IF EXISTS `analog3`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `analog3` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `date_time` datetime NOT NULL,
  `entete` varchar(4) NOT NULL,
  `ana1` float(5,2) NOT NULL COMMENT 'Temp. (°C)',
  `ana2` float(5,2) NOT NULL COMMENT 'Humid. (%)',
  `ana3` float(5,2) NOT NULL COMMENT 'Pt rosée (°C)',
  `ana4` float(5,2) NOT NULL COMMENT 'Tension Batt. (V)',
  `tag` int(1) NOT NULL,
  PRIMARY KEY (`id`),
  KEY `date_time` (`date_time`)
) ENGINE=MyISAM AUTO_INCREMENT=1173044 DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci COMMENT='Données T° + H° + Pt rosée Puits Canadien';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `analog4`
--

DROP TABLE IF EXISTS `analog4`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `analog4` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `date_time` datetime NOT NULL,
  `entete` varchar(4) NOT NULL,
  `ana1` float(5,2) NOT NULL,
  `ana2` float(5,2) NOT NULL,
  `ana3` float(5,2) NOT NULL,
  `ana4` float(5,2) NOT NULL,
  `tag` int(1) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=378156 DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci COMMENT='Sonde sortie bouche insufflation';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `analog5`
--

DROP TABLE IF EXISTS `analog5`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `analog5` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `date_time` datetime NOT NULL,
  `entete` varchar(4) NOT NULL,
  `ana1` float(5,2) NOT NULL,
  `ana2` float(5,2) NOT NULL,
  `ana3` float(5,2) NOT NULL,
  `ana4` float(5,2) NOT NULL,
  `tag` int(1) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=171674 DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci COMMENT='Sonde dans les combles';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `analog6`
--

DROP TABLE IF EXISTS `analog6`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `analog6` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `date_time` datetime NOT NULL,
  `entete` varchar(4) NOT NULL,
  `ana1` float(5,2) NOT NULL,
  `ana2` float(5,2) NOT NULL,
  `ana3` float(5,2) NOT NULL,
  `ana4` float(5,2) NOT NULL,
  `tag` int(1) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=367877 DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci COMMENT='Sondes autour de la VMCDF';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bypass_pc_log`
--

DROP TABLE IF EXISTS `bypass_pc_log`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bypass_pc_log` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `date_time` datetime NOT NULL,
  `consigne` int(11) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=291623 DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci COMMENT='log des consignes envoyées au bypass du puitscanadien';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `calendrier`
--

DROP TABLE IF EXISTS `calendrier`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `calendrier` (
  `id` bigint(20) NOT NULL,
  `date` date NOT NULL,
  `saison` varchar(1) NOT NULL,
  `type_jour` varchar(1) NOT NULL,
  UNIQUE KEY `id` (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci COMMENT='calendrier avec type de jour et saison';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `calendrier_30min`
--

DROP TABLE IF EXISTS `calendrier_30min`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `calendrier_30min` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `date` date NOT NULL,
  `heure_debut` time NOT NULL,
  `heure_fin` time NOT NULL,
  `temperature` float DEFAULT NULL,
  `priorite` int(11) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=17569 DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci COMMENT='Calendrier, découpage de 30 minutes';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `calendrier_saison`
--

DROP TABLE IF EXISTS `calendrier_saison`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `calendrier_saison` (
  `id` int(16) NOT NULL AUTO_INCREMENT,
  `type` varchar(1) NOT NULL,
  `consigne_temperature` float(3,1) NOT NULL,
  `flux solaire` int(4) NOT NULL,
  `commentaire` varchar(50) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=6 DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci COMMENT='type de saison et température de consignes';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `calendrier_type_jour`
--

DROP TABLE IF EXISTS `calendrier_type_jour`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `calendrier_type_jour` (
  `id` int(3) NOT NULL AUTO_INCREMENT,
  `type_jour` varchar(1) NOT NULL,
  `heure_debut` time NOT NULL,
  `heure_fin` time NOT NULL,
  `commentaire` varchar(50) NOT NULL,
  `priorite` int(1) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=8 DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `chauffage_PID`
--

DROP TABLE IF EXISTS `chauffage_PID`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `chauffage_PID` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `D_prec` float NOT NULL,
  `Consigne_prec` float NOT NULL,
  `ecart_prec` float NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=3 DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci COMMENT='Variables du régulateur PID';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `chauffage_log`
--

DROP TABLE IF EXISTS `chauffage_log`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `chauffage_log` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `date_time` datetime NOT NULL,
  `etat` int(1) NOT NULL,
  `etat_prec` int(1) NOT NULL,
  `mode` int(1) NOT NULL,
  `mode_prec` int(1) NOT NULL,
  `consigne_utilisateur` int(3) NOT NULL,
  `heure_debut_consigne` datetime NOT NULL,
  `heure_fin_consigne` datetime NOT NULL,
  `heure_demarrage` datetime NOT NULL,
  `temp_int` float(3,1) NOT NULL,
  `temp_consigne` float(3,1) NOT NULL,
  `delta_time` int(11) NOT NULL,
  `puissance` int(3) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=559394 DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci COMMENT='Table pour le log des consignes chauffage calculées par le r';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `chauffage_statut`
--

DROP TABLE IF EXISTS `chauffage_statut`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `chauffage_statut` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `etat` int(1) NOT NULL,
  `etat_prec` int(1) NOT NULL,
  `mode` int(1) NOT NULL,
  `mode_prec` int(1) NOT NULL,
  `consigne_utilisateur` int(3) NOT NULL,
  `heure_debut` datetime NOT NULL,
  `consigne_anticipe` int(3) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=2 DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci COMMENT='Etat de la machine d''état gérant le chauffage';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `donnees_instant`
--

DROP TABLE IF EXISTS `donnees_instant`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `donnees_instant` (
  `id` bigint(20) NOT NULL AUTO_INCREMENT,
  `date_time` datetime NOT NULL,
  `date_int` datetime NOT NULL,
  `T_int` float(5,2) NOT NULL,
  `H_int` float(5,2) NOT NULL,
  `pente_T_int` float(5,2) NOT NULL,
  `date_ext` datetime NOT NULL,
  `T_ext` float(5,2) NOT NULL,
  `H_ext` float(5,2) NOT NULL,
  `pente_T_ext` float(5,2) NOT NULL,
  `T_pc` float(5,2) NOT NULL,
  `date_pc` datetime NOT NULL,
  `H_pc` float(5,2) NOT NULL,
  `vmc_temp_air_neuf` float(5,2) NOT NULL,
  `vmc_temp_air_ext` float(5,2) NOT NULL,
  `vmc_temp_air_repris` float(5,2) NOT NULL,
  `vmc_temp_air_extrait` float(5,2) NOT NULL,
  `vmc_temp_air_bouche` float(5,2) NOT NULL,
  `temp_comble` float(5,2) NOT NULL,
  `temp_garage` float(5,2) NOT NULL,
  `pente_T_pc` float(5,2) NOT NULL,
  `puissance_pc` float(7,2) NOT NULL,
  `consigne_pc` int(3) NOT NULL,
  `puissance_poele` int(3) NOT NULL,
  `flux_solaire` float(6,2) NOT NULL,
  `date_teleinfo` datetime NOT NULL,
  `tarif_watt` varchar(4) NOT NULL,
  `conso_watt` int(5) NOT NULL,
  `icon_Ti` varchar(20) NOT NULL,
  `icon_Hi` varchar(20) NOT NULL,
  `icon_Te` varchar(20) NOT NULL,
  `icon_He` varchar(20) NOT NULL,
  `icon_led_edf` varchar(20) NOT NULL,
  `icon_led_Ti` varchar(20) NOT NULL,
  `icon_led_Te` varchar(20) NOT NULL,
  `icon_led_pc` varchar(20) NOT NULL,
  `icon_led_vmc` varchar(20) NOT NULL,
  `icon_led_combles` varchar(20) NOT NULL,
  `icon_led_airneuf` varchar(20) NOT NULL,
  `bypass_vmcdf` int(1) NOT NULL,
  `ups_datetime` datetime NOT NULL DEFAULT current_timestamp(),
  `ups_status` char(10) NOT NULL,
  `ups_battery_charge` float NOT NULL,
  `ups_battery_runtime` int(5) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=3 DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci COMMENT='Données actuelles pour l''affichage rapide';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `ephemerides`
--

DROP TABLE IF EXISTS `ephemerides`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `ephemerides` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `mois-jour` date NOT NULL,
  `lever` time NOT NULL DEFAULT '00:00:00',
  `coucher` time NOT NULL DEFAULT '00:00:00',
  PRIMARY KEY (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=369 DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `internet_connex`
--

DROP TABLE IF EXISTS `internet_connex`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `internet_connex` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `date_time` datetime NOT NULL,
  `status` int(1) NOT NULL,
  `ping` int(5) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=340730 DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `linky_offset`
--

DROP TABLE IF EXISTS `linky_offset`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `linky_offset` (
  `id` int(1) NOT NULL AUTO_INCREMENT,
  `date_time` datetime NOT NULL DEFAULT current_timestamp(),
  `linky_hc` int(5) NOT NULL,
  `linky_hp` int(5) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `mode_domotique`
--

DROP TABLE IF EXISTS `mode_domotique`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `mode_domotique` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `mode` varchar(10) NOT NULL,
  `alarme` int(1) NOT NULL,
  `videosurveillance` int(1) NOT NULL,
  UNIQUE KEY `id` (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=2 DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci COMMENT='Mode de fonctionnement de la domotique';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `pellets`
--

DROP TABLE IF EXISTS `pellets`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `pellets` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `date_time` date NOT NULL,
  `credit` int(11) NOT NULL COMMENT 'sac',
  `debit` int(11) NOT NULL COMMENT 'sac',
  `capital` int(11) NOT NULL COMMENT 'sac',
  PRIMARY KEY (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=1417 DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci COMMENT='Données stock et consommation de sacs de granulés';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `pellets_rsv`
--

DROP TABLE IF EXISTS `pellets_rsv`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `pellets_rsv` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `date_time` datetime NOT NULL,
  `entete` varchar(4) NOT NULL,
  `nvg` int(4) NOT NULL COMMENT '(cm)',
  `tag` int(1) NOT NULL,
  PRIMARY KEY (`id`),
  KEY `date_time` (`date_time`)
) ENGINE=MyISAM AUTO_INCREMENT=23797 DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci COMMENT='Niveau de granules dans le réservoir';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `puissance_pc`
--

DROP TABLE IF EXISTS `puissance_pc`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `puissance_pc` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `date_time` datetime NOT NULL,
  `entete` varchar(4) NOT NULL,
  `puissance` float(7,2) NOT NULL COMMENT '(W)',
  `puissance_simple` float(7,2) NOT NULL COMMENT 'puissance selon formule simplifiee',
  PRIMARY KEY (`id`),
  KEY `date_time` (`date_time`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci COMMENT='Puissance du puits canadien';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `pyranometre`
--

DROP TABLE IF EXISTS `pyranometre`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `pyranometre` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `date_time` datetime NOT NULL,
  `entete` varchar(4) NOT NULL,
  `ana1` float(5,2) NOT NULL COMMENT 'Volts (indique des W/m2)',
  `tag` int(1) NOT NULL,
  PRIMARY KEY (`id`),
  KEY `date_time` (`date_time`)
) ENGINE=MyISAM AUTO_INCREMENT=1004198 DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci COMMENT='Données du pyranometre';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `teleinfo`
--

DROP TABLE IF EXISTS `teleinfo`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `teleinfo` (
  `id` bigint(11) NOT NULL AUTO_INCREMENT,
  `date_time` datetime NOT NULL,
  `entete` varchar(4) NOT NULL,
  `HC` int(9) NOT NULL COMMENT 'Wh',
  `HP` int(9) NOT NULL COMMENT 'Wh',
  `tarif` varchar(4) NOT NULL COMMENT 'HP ou HC',
  `courant` int(3) NOT NULL COMMENT '(A)',
  `courant max` int(3) NOT NULL COMMENT '(A)',
  `puissance` int(5) NOT NULL COMMENT '(W)',
  `tag` int(1) NOT NULL,
  KEY `id` (`id`),
  KEY `date_time` (`date_time`)
) ENGINE=MyISAM AUTO_INCREMENT=23801394 DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci COMMENT='Données du compteur EDF';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `tondeuse_pos`
--

DROP TABLE IF EXISTS `tondeuse_pos`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `tondeuse_pos` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `date_time` datetime NOT NULL,
  `entete` varchar(4) NOT NULL,
  `lat` double NOT NULL,
  `lon` double NOT NULL,
  `cap` int(11) NOT NULL,
  `vitesse` float(3,1) NOT NULL,
  `temperature` int(11) NOT NULL,
  `tension` float(2,1) NOT NULL,
  PRIMARY KEY (`id`),
  KEY `id` (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=113 DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci COMMENT='Position lat/lon du robot tondeuse';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `tx_msg_radio`
--

DROP TABLE IF EXISTS `tx_msg_radio`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `tx_msg_radio` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `date_time` datetime NOT NULL,
  `message` varchar(66) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=1263716 DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci COMMENT='Table servant de pile des messages à émettre vers la radio (';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `ups`
--

DROP TABLE IF EXISTS `ups`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `ups` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `date_time` datetime NOT NULL,
  `ups_status` char(20) NOT NULL,
  `battery_charge` int(11) NOT NULL,
  `battery_runtime` int(11) NOT NULL,
  `output_voltage` float NOT NULL,
  `tag` int(1) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=284899 DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `voletroulant_log`
--

DROP TABLE IF EXISTS `voletroulant_log`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `voletroulant_log` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `date_time` datetime NOT NULL,
  `bureau` int(11) NOT NULL,
  `salon` int(11) NOT NULL,
  `chambreM` int(11) NOT NULL,
  `chambreJF` int(11) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=148697 DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci COMMENT='log pour voir l''évolution des volets';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `voletroulant_statut`
--

DROP TABLE IF EXISTS `voletroulant_statut`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `voletroulant_statut` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `date_time` datetime NOT NULL,
  `mode` varchar(1) NOT NULL,
  `bureau` int(1) NOT NULL,
  `salon` int(1) NOT NULL,
  `chambreM` int(1) NOT NULL,
  `chambreJF` int(1) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=2 DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci COMMENT='Etat des volets roulants';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `voletroulant_table_verite`
--

DROP TABLE IF EXISTS `voletroulant_table_verite`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `voletroulant_table_verite` (
  `cas` int(11) NOT NULL,
  `saison` char(1) NOT NULL,
  `flux solaire` int(11) NOT NULL,
  `jour/nuit` int(11) NOT NULL,
  `Temperature consigne` int(11) NOT NULL,
  `VOLET BUREAU` int(11) NOT NULL,
  `VOLET SALON` int(11) NOT NULL,
  `VOLET CHM` int(11) NOT NULL,
  `VOLET CHJF` int(11) NOT NULL
) ENGINE=MyISAM DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci COMMENT='table de vérité état des volets roulants';
/*!40101 SET character_set_client = @saved_cs_client */;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2024-08-18  0:00:03
