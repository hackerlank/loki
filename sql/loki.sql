-- MySQL dump 10.13  Distrib 5.5.43, for debian-linux-gnu (x86_64)
--
-- Host: localhost    Database: loki
-- ------------------------------------------------------
-- Server version	5.5.43-0ubuntu0.14.04.1

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `LOGIN`
--

DROP TABLE IF EXISTS `LOGIN`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `LOGIN` (
  `account` varchar(32) NOT NULL,
  `password` varchar(32) NOT NULL DEFAULT '',
  `isforbid` int(10) unsigned NOT NULL DEFAULT '0',
  `accid` int(10) unsigned DEFAULT '0',
  PRIMARY KEY (`account`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `LOGIN`
--

LOCK TABLES `LOGIN` WRITE;
/*!40000 ALTER TABLE `LOGIN` DISABLE KEYS */;
INSERT INTO `LOGIN` VALUES ('loki','123456',0,1);
/*!40000 ALTER TABLE `LOGIN` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `charbase`
--

DROP TABLE IF EXISTS `charbase`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `charbase` (
  `charid` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `accid` int(10) unsigned NOT NULL,
  `name` varchar(32) NOT NULL DEFAULT '',
  `type` int(10) unsigned NOT NULL DEFAULT '0',
  `country` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`charid`),
  KEY `accid` (`accid`),
  KEY `name` (`name`)
) ENGINE=MyISAM AUTO_INCREMENT=2 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `charbase`
--

LOCK TABLES `charbase` WRITE;
/*!40000 ALTER TABLE `charbase` DISABLE KEYS */;
INSERT INTO `charbase` VALUES (1,1,'loki83',0,2);
/*!40000 ALTER TABLE `charbase` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `serverlist`
--

DROP TABLE IF EXISTS `serverlist`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `serverlist` (
  `id` int(10) unsigned NOT NULL,
  `type` int(10) unsigned NOT NULL DEFAULT '0',
  `name` varchar(32) NOT NULL DEFAULT '',
  `ip` varchar(16) NOT NULL DEFAULT '127.0.0.1',
  `port` int(10) unsigned NOT NULL DEFAULT '0',
  `extip` varchar(16) NOT NULL DEFAULT '127.0.0.1',
  `extport` int(10) unsigned NOT NULL DEFAULT '0',
  `nettype` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `serverlist`
--

LOCK TABLES `serverlist` WRITE;
/*!40000 ALTER TABLE `serverlist` DISABLE KEYS */;
INSERT INTO `serverlist` VALUES (1,1,'super server','127.0.0.1',10000,'127.0.0.1',10000,0),(11,11,'record server','127.0.0.1',11001,'127.0.0.1',11001,0),(12,12,'bill server','127.0.0.1',12001,'127.0.0.1',12001,0),(20,20,'session server','127.0.0.1',20001,'127.0.0.1',20001,0),(21,21,'scene server','127.0.0.1',21001,'127.0.0.1',21001,0),(22,21,'scene server','127.0.0.1',21002,'127.0.0.1',21002,0),(200,22,'gateway server','127.0.0.1',22001,'127.0.0.1',22001,0),(201,22,'gateway server','127.0.0.1',22002,'127.0.0.1',22002,0),(202,22,'gateway server','127.0.0.1',22003,'127.0.0.1',22003,0);
/*!40000 ALTER TABLE `serverlist` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `zonelist`
--

DROP TABLE IF EXISTS `zonelist`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `zonelist` (
  `game` int(10) unsigned NOT NULL DEFAULT '0',
  `zone` int(10) unsigned NOT NULL DEFAULT '0',
  `ip` varchar(16) NOT NULL DEFAULT '127.0.0.1',
  `port` int(10) unsigned NOT NULL DEFAULT '0',
  `name` varchar(32) NOT NULL DEFAULT '',
  `description` varchar(100) NOT NULL DEFAULT '',
  `isuse` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`game`,`zone`),
  UNIQUE KEY `ip_port` (`ip`,`port`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `zonelist`
--

LOCK TABLES `zonelist` WRITE;
/*!40000 ALTER TABLE `zonelist` DISABLE KEYS */;
INSERT INTO `zonelist` VALUES (1,1,'127.0.0.1',10000,'loki','loki game',1);
/*!40000 ALTER TABLE `zonelist` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2015-07-04 17:51:14
