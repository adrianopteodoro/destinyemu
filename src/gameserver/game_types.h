#ifndef _GAME_TYPES_H_
#define _GAME_TYPES_H_

// Account
struct CAccount
{
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
    unsigned int MaxHP;
    unsigned int MaxMP;
    long int HP;
    long int MP;
    unsigned int MaxWeight;
    unsigned int Weight;

    // Stats
    unsigned int Str;
    unsigned int Dex;
    unsigned int Int;
    unsigned int Con;

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
    unsigned int RespawnID;
    unsigned int Pos_x;
    unsigned int Pos_y;
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

#endif
