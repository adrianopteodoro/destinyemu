/*
SQLyog Community Edition- MySQL GUI v6.07
Host - 6.0.2-alpha-community-nt-debug : Database - destiny
*********************************************************************
Server version : 6.0.2-alpha-community-nt-debug
*/

/*!40101 SET NAMES utf8 */;

/*!40101 SET SQL_MODE=''*/;

create database if not exists `destiny`;

USE `destiny`;

/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;

/*Table structure for table `t_characters` */

DROP TABLE IF EXISTS `t_characters`;

CREATE TABLE `t_characters` (
  `Id` int(11) NOT NULL AUTO_INCREMENT,
  `cName` varchar(16) DEFAULT NULL,
  `AccountID` int(10) DEFAULT NULL,
  `BodyID` int(10) DEFAULT '0',
  `ChaosPts` int(10) DEFAULT '70',
  `cLevel` int(10) DEFAULT '0',
  `SkillPts` int(10) NOT NULL DEFAULT '0',
  `StatsPts` int(10) NOT NULL DEFAULT '0',
  `MasterPts` int(10) DEFAULT '0',
  `cStr` int(10) DEFAULT '0',
  `cInt` int(10) DEFAULT '0',
  `cDex` int(10) DEFAULT '0',
  `cCon` int(10) DEFAULT '0',
  `Gold` int(50) DEFAULT '0',
  `cExp` int(50) DEFAULT '0',
  `CharListPos` int(10) DEFAULT '0',
  `Class` int(10) DEFAULT '0',
  `wMaster` int(10) DEFAULT '0',
  `fMaster` int(10) DEFAULT '0',
  `sMaster` int(10) DEFAULT '0',
  `tMaster` int(10) DEFAULT '0',
  PRIMARY KEY (`Id`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC;

/*Data for the table `t_characters` */

insert  into `t_characters`(`Id`,`cName`,`AccountID`,`BodyID`,`ChaosPts`,`cLevel`,`SkillPts`,`StatsPts`,`MasterPts`,`cStr`,`cInt`,`cDex`,`cCon`,`Gold`,`cExp`,`CharListPos`,`Class`,`wMaster`,`fMaster`,`sMaster`,`tMaster`) values (1,'TestTK',1,1,70,0,0,0,0,8,4,7,6,0,0,0,0,0,0,0,0);

/*Table structure for table `t_inventory` */

DROP TABLE IF EXISTS `t_inventory`;

CREATE TABLE `t_inventory` (
  `Id` int(10) NOT NULL AUTO_INCREMENT,
  `CharID` int(10) NOT NULL,
  `Slot` int(10) DEFAULT '0',
  `ItemID` int(10) DEFAULT '0',
  `EF1` int(10) DEFAULT '0',
  `EFV1` int(10) DEFAULT '0',
  `EF2` int(10) DEFAULT '0',
  `EFV2` int(10) DEFAULT '0',
  `EF3` int(10) DEFAULT '0',
  `EFV3` int(10) DEFAULT '0',
  PRIMARY KEY (`Id`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC;

/*Data for the table `t_inventory` */

insert  into `t_inventory`(`Id`,`CharID`,`Slot`,`ItemID`,`EF1`,`EFV1`,`EF2`,`EFV2`,`EF3`,`EFV3`) values (1,1,5,871,43,255,0,0,0,0);

/*Table structure for table `t_storage` */

DROP TABLE IF EXISTS `t_storage`;

CREATE TABLE `t_storage` (
  `Id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `UserID` int(10) NOT NULL,
  `Slot` int(10) NOT NULL DEFAULT '0',
  `ItemID` int(10) NOT NULL DEFAULT '0',
  `EF1` int(10) NOT NULL DEFAULT '0',
  `EFV1` int(10) NOT NULL DEFAULT '0',
  `EF2` int(10) NOT NULL DEFAULT '0',
  `EFV2` int(10) NOT NULL DEFAULT '0',
  `EF3` int(10) NOT NULL DEFAULT '0',
  `EFV3` int(10) NOT NULL DEFAULT '0',
  PRIMARY KEY (`Id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC;

/*Data for the table `t_storage` */

/*Table structure for table `t_user` */

DROP TABLE IF EXISTS `t_user`;

CREATE TABLE `t_user` (
  `Id` int(11) NOT NULL AUTO_INCREMENT,
  `UserID` varchar(12) DEFAULT NULL,
  `UserPWD` varchar(12) DEFAULT NULL,
  `Email` varchar(60) DEFAULT NULL,
  `CtlCode` int(10) DEFAULT '1',
  `Gold` int(12) DEFAULT '0',
  PRIMARY KEY (`Id`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC;

/*Data for the table `t_user` */

insert  into `t_user`(`Id`,`UserID`,`UserPWD`,`Email`,`CtlCode`,`Gold`) values (1,'test','test',NULL,1,0);

/*Table structure for table `t_user_stats` */

DROP TABLE IF EXISTS `t_user_stats`;

CREATE TABLE `t_user_stats` (
  `UserID` int(10) DEFAULT NULL,
  `CharID` int(10) DEFAULT NULL,
  `ChannelID` int(10) DEFAULT NULL,
  `IPAddress` varchar(30) DEFAULT NULL,
  `ConnDate` timestamp NULL DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC;

/*Data for the table `t_user_stats` */

/* Procedure structure for procedure `sp_CheckUserAndPWD` */

/*!50003 DROP PROCEDURE IF EXISTS  `sp_CheckUserAndPWD` */;

DELIMITER $$

/*!50003 CREATE DEFINER=`root`@`localhost` PROCEDURE `sp_CheckUserAndPWD`(IN UID varchar(12), IN PWD varchar(12))
BEGIN
	SELECT count(*) FROM t_user WHERE UserID=UID AND UserPWD=PWD;
    END */$$
DELIMITER ;

/* Procedure structure for procedure `sp_CheckUserID` */

/*!50003 DROP PROCEDURE IF EXISTS  `sp_CheckUserID` */;

DELIMITER $$

/*!50003 CREATE DEFINER=`root`@`localhost` PROCEDURE `sp_CheckUserID`(IN UID varchar(12))
BEGIN
	SELECT count(*) FROM t_user WHERE UserID=UID;
    END */$$
DELIMITER ;

/* Procedure structure for procedure `sp_GetAccountID` */

/*!50003 DROP PROCEDURE IF EXISTS  `sp_GetAccountID` */;

DELIMITER $$

/*!50003 CREATE DEFINER=`root`@`localhost` PROCEDURE `sp_GetAccountID`(IN UID varchar(12))
BEGIN
	SELECT Id FROM t_user WHERE UserID=UID;
    END */$$
DELIMITER ;

/* Procedure structure for procedure `sp_GetAccountInfo` */

/*!50003 DROP PROCEDURE IF EXISTS  `sp_GetAccountInfo` */;

DELIMITER $$

/*!50003 CREATE DEFINER=`root`@`localhost` PROCEDURE `sp_GetAccountInfo`(IN UID varchar(12))
BEGIN
	SELECT * FROM t_user WHERE UserID=UID;
    END */$$
DELIMITER ;

/* Procedure structure for procedure `sp_GetCharInfo` */

/*!50003 DROP PROCEDURE IF EXISTS  `sp_GetCharInfo` */;

DELIMITER $$

/*!50003 CREATE DEFINER=`root`@`localhost` PROCEDURE `sp_GetCharInfo`(IN AID int, IN PID int)
BEGIN
	SELECT Id, cName, BodyID, cLevel, cStr, cInt, cDex, cCon, Gold, cExp, Class, wMaster, fMaster, sMaster, tMaster, SkillPts, StatsPts, MasterPts, ChaosPts FROM t_characters WHERE AccountID=AID AND CharListPos=PID;
    END */$$
DELIMITER ;

/* Procedure structure for procedure `sp_GetCharInfoforCList` */

/*!50003 DROP PROCEDURE IF EXISTS  `sp_GetCharInfoforCList` */;

DELIMITER $$

/*!50003 CREATE DEFINER=`root`@`localhost` PROCEDURE `sp_GetCharInfoforCList`(IN AccID int)
BEGIN
	SELECT Id, cName, BodyID, cLevel, cStr, cInt, cDex, cCon, Gold, cExp, CharListPos, Class, wMaster, fMaster, sMaster, tMaster FROM t_characters WHERE AccountID=AccID;
    END */$$
DELIMITER ;

/* Procedure structure for procedure `sp_GetInventaryItems` */

/*!50003 DROP PROCEDURE IF EXISTS  `sp_GetInventaryItems` */;

DELIMITER $$

/*!50003 CREATE DEFINER=`root`@`localhost` PROCEDURE `sp_GetInventaryItems`(IN CID int)
BEGIN
	SELECT Slot,ItemID,EF1,EFV1,EF2,EFV2,EF3,EFV3 FROM t_inventory WHERE CharID=CID;
    END */$$
DELIMITER ;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
