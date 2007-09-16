#ifndef _GAME_TYPES_H_
#define _GAME_TYPES_H_

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
	float z; // Unused?
	unsigned short RespawnID;
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
struct CPosition
{
    UINT RespawnID;
    fPoint Cpos; // Current Position
    fPoint Dpos; // Destine Position
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

struct CEquip
{
	unsigned int itemid;
	std::string name;
	unsigned int level;
	unsigned int damage;
	unsigned int defesa;
	unsigned int hp;
	unsigned int mp;
	unsigned int exp;
	unsigned int i_str;
	unsigned int i_int;
	unsigned int i_dex;
	unsigned int i_con;
	unsigned int special1;
	unsigned int special2;
	unsigned int special3;
	unsigned int special4;
	unsigned int score14;
	unsigned int score15;
	unsigned int pos;
	unsigned int i_class;
	unsigned int r1sidc;
	unsigned int r2sidc;
	unsigned int wtype;
	unsigned int req_str;
	unsigned int req_int;
	unsigned int req_dex;
	unsigned int req_con;
	unsigned int attspeed;
	unsigned int range;
	unsigned int price;
	unsigned int runspeed;
	unsigned int spell;
	unsigned int duration;
	unsigned int parm;
	unsigned int grid;
	unsigned int ground;
	unsigned int clan;
	unsigned int hwordcoin;
	unsigned int lwordcoin;
	unsigned int i_volatile;
	unsigned int keyid;
	unsigned int parry;
	unsigned int hitrate;
	unsigned int critical;
	unsigned int sanc;
	unsigned int savemana;
	unsigned int hpadd;
	unsigned int mpadd;
	unsigned int regenhp;
	unsigned int regenmp;
	unsigned int resist1;
	unsigned int resist2;
	unsigned int resist3;
	unsigned int resist4;
	unsigned int acadd;
	unsigned int resistall;
	unsigned int bonus;
	unsigned int hwordguild;
	unsigned int lwoedguild;
	unsigned int quest;
	unsigned int unique;
	unsigned int magic;
	unsigned int quantidade;
	unsigned int hwordindex;
	unsigned int lwordindex;
	unsigned int init1;
	unsigned int init2;
	unsigned int init3;
	unsigned int damageadd;
	unsigned int magicadd;
	unsigned int hpadd2;
	unsigned int mpadd2;
	unsigned int critical2;
	unsigned int acadd2;
	unsigned int damage2;
	unsigned int specialall;
	unsigned int curkill;
	unsigned int ltokill;
	unsigned int htokill;
	unsigned int incubate;
	unsigned int mountlife;
	unsigned int mounthp;
	unsigned int mountsanc;
	unsigned int mountfeed;
	unsigned int mountkill;
	unsigned int incudelay;
	unsigned int subguild;
	unsigned int grade0;
	unsigned int grade1;
	unsigned int grade2;
	unsigned int grade3;
	unsigned int grade4;
	unsigned int grade5;
};

// ITEMS
struct CEquipList
{
    CEquip *Index[8000];
    vector<CEquip*> Data;
    CEquip* nullequip;
};

#endif
