#ifndef _GAME_TYPES_H_
#define _GAME_TYPES_H_

#include "game_itemeffect.h"

#define NPC_NORMAL      0
#define NPC_SHOP        1
#define NPC_STORAGE     2
#define MPC_SKILL        3
#define NPC_CITYPAY     6
#define NPC_SCRAPCHANGE 12
#define NPC_STATRESET   31
#define NPC_SKILLRESET  41
#define NPC_GRIFFMASTER 151

struct CLanguage
{
    std::string id_invalid;
    std::string id_online;
    std::string id_notactive;
    std::string pwd_invalid;
    std::string name_alreadyexist;
    std::string wrong_cliver;
};

struct CTime
{
	clock_t lastAttackTime;
	clock_t lastRegenTime;
	clock_t lastMoveTime;
	clock_t lastSaveTime;
	clock_t lastGG;
	clock_t initSkill;
};

// -----------------------------------------------------------------------------------------
// A 2d point, for positions
// -----------------------------------------------------------------------------------------
struct fPoint {
	float x;
	float y;
};

// Account
struct CAccount
{
    unsigned int clientid;
    unsigned int charid;
	unsigned int userid;
	char username[17];
	char password[33];
	unsigned int accesslevel;
	bool isLoggedIn;
	bool inGame;
};

// Save the stats;
struct CStats
{
    //
    unsigned short MaxHP;
    unsigned short MaxMP;
    unsigned short HP;
    unsigned short MP;
    unsigned int MaxWeight;
    unsigned int Weight;

    // Stats
    unsigned short Str;
    unsigned short Dex;
    unsigned short Int;
    unsigned short Con;

    // Attr
    unsigned int Attack_Power;
    unsigned int Defense;
    unsigned int Magic_Defense;
    unsigned int Accury;
    unsigned int Critical;
    unsigned int Dodge;
    unsigned int Attack_Speed;
    unsigned int Move_Speed;
};

//Items
struct CItem
{
    unsigned int itemid;
    unsigned int add1;
    unsigned int add2;
    unsigned int add3;
    unsigned int val1;
    unsigned int val2;
    unsigned int val3;
};

// Position (include respawnid and save town map)
struct currentition
{
	fPoint source;          // source position
    fPoint current;         // current position    
    fPoint destiny;         // destiny  position
    unsigned int respawn;   // respawn id [player = respawnid | monster = spawnid]
    unsigned int saved;     // saved id [player town | monster = 0]
    clock_t lastMoveTime;
};

// Player information
struct CInfo
{
	char char_name[30];
	unsigned int mobid;
	unsigned int classid;
    unsigned int Exp;
    unsigned int Level;
    long int Gold;
    long int Storage_Gold;
    unsigned int StatPoints;
    unsigned int SkillPoints;
};

// Characters List
struct CCharacter {
	char char_name[30];
	unsigned int Level;
	unsigned int Exp;
	unsigned int Mobid;
	unsigned int Str;
	unsigned int Int;
	unsigned int Dex;
	unsigned int Con;
	long int gold;
	CItem eqitems[15];
};

struct CNPC {
	unsigned short clientid;
	fPoint pos;
	float dir;
    std::string npcname;
    CItem inventory[15];
    CItem shopitems[30];
    unsigned int mobid;
    unsigned int mobcode;
    unsigned int level;
    unsigned int n_str;
    unsigned int n_dex;
    unsigned int n_int;
    unsigned int n_con;
};

struct CEffect
{
	unsigned int effectid;
	unsigned int value;
};

struct CEquip
{
	unsigned int itemid;
	std::string name;
	unsigned int rlvl;
	unsigned int pos;
	unsigned int mesh;
	unsigned int wtype;
	unsigned int req_str;
	unsigned int req_int;
	unsigned int req_dex;
	unsigned int req_con;
	unsigned int price;
	CEffect effects[11];
};

// ITEMS
struct CEquipList
{
    CEquip *Index[8000];
    vector<CEquip*> Data;
    CEquip* nullequip;
};

#endif
