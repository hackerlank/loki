USE loki;

DROP TABLE IF EXISTS `zonelist`;
CREATE TABLE `zonelist` (
		`game` int(10) unsigned NOT NULL DEFAULT '0',
		`zone` int(10) unsigned NOT NULL default '0',
		`ip` varchar(16) NOT NULL default '127.0.0.1',
		`port` int(10) unsigned NOT NULL default '0',
		`name` varchar(32) NOT NULL default '',
		`description` varchar(100) not null default '',
		`isuse`  int(10) unsigned not null default '0',
		PRIMARY KEY(`game`, `zone`),
		UNIQUE KEY `ip_port` (`ip`, `port`)
		);

