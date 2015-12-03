USE loki;

DROP TABLE IF EXISTS `LOGIN`;
CREATE TABLE `LOGIN` (
		`account` varchar(32) not null,
		`password` varchar(32) not null default '',
		`isforbid` int(10) unsigned NOT NULL DEFAULT '0',
		`accid` int(10) unsigned NOT NULL DEFAULT '0',
		PRIMARY KEY(`account`)
		);

