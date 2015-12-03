use loki;

DROP TABLE IF EXISTS `charbase`;
CREATE TABLE `charbase` (
		`charid` int(10) unsigned NOT NULL auto_increment,
		`accid` int(10) unsigned NOT NULL ,
		`name` varchar(32) NOT NULL default '',
		`type` int(10) unsigned NOT NULL default 0,
		PRIMARY KEY (`charid`),
		key (`accid`),
		key (`name`)
		)ENGINE=MyISAM;



