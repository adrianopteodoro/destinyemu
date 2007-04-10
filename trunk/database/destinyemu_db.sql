/*
SQLyog Community Edition- MySQL GUI v6.0 Beta 1
Host - 5.0.27-community-nt-log : Database - openwyd
*********************************************************************
Server version : 5.0.27-community-nt-log
*/

/*!40101 SET NAMES utf8 */;

/*!40101 SET SQL_MODE=''*/;

/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;

/*Table structure for table `accounts` */

CREATE TABLE `accounts` (
  `id` int(11) NOT NULL auto_increment,
  `username` varchar(16) default NULL,
  `password` varchar(12) default NULL,
  `email` varchar(60) default NULL,
  `active` int(11) default '0',
  `online` int(11) default '0',
  `acesslevel` int(10) default '1',
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=5 DEFAULT CHARSET=utf8;

/*Table structure for table `char_items` */

CREATE TABLE `char_items` (
  `id` int(11) NOT NULL auto_increment,
  `owner` varchar(25) default NULL,
  `slotnum` int(10) default '0',
  `itemid` int(10) default '0',
  `add1` int(10) default '0',
  `addval1` int(10) default '0',
  `add2` int(10) default '0',
  `addval2` int(10) default '0',
  `add3` int(10) default '0',
  `addval3` int(10) default '0',
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=24 DEFAULT CHARSET=utf8;

/*Table structure for table `characters` */

CREATE TABLE `characters` (
  `id` int(11) NOT NULL auto_increment,
  `name` varchar(16) default NULL,
  `uid` varchar(16) default NULL,
  `mobid` int(10) default '0',
  `clevel` int(10) default '0',
  `skpoints` int(10) NOT NULL default '0',
  `stpoints` int(10) NOT NULL default '0',
  `max_hp` int(11) NOT NULL default '1',
  `max_mp` int(11) NOT NULL default '1',
  `cstr` int(10) default '0',
  `cint` int(10) default '0',
  `cdex` int(10) default '0',
  `ccon` int(10) default '0',
  `cskills` varchar(255) NOT NULL default '0',
  `quick_bar` varchar(255) NOT NULL default '0',
  `gold` int(50) default '0',
  `storage_gold` int(60) NOT NULL default '0',
  `cexp` int(50) default '0',
  `posid` int(10) default NULL,
  `classid` int(10) default NULL,
  `online` int(11) default '0',
  `pos_x` int(10) default '2093',
  `pos_y` int(10) default '2097',
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=9 DEFAULT CHARSET=utf8;

/*Table structure for table `storage` */

CREATE TABLE `storage` (
  `id` int(10) unsigned NOT NULL auto_increment,
  `owner` varchar(20) NOT NULL,
  `slotnum` int(10) NOT NULL default '0',
  `itemid` int(10) NOT NULL default '0',
  `add1` int(10) NOT NULL default '0',
  `addval1` int(10) NOT NULL default '0',
  `add2` int(10) NOT NULL default '0',
  `addval2` int(10) NOT NULL default '0',
  `add3` int(10) NOT NULL default '0',
  `addval3` int(10) NOT NULL default '0',
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;