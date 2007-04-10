#ifndef _GAME_TYPES_H_
#define _GAME_TYPES_H_

// Account
struct CAccount
{
	unsigned int userid;
	char username[17];
	char password[33];
	int accesslevel;
	bool isLoggedIn;
	bool inGame;
};

// Save the stats;
struct CStats
{
    //
    int MaxHP;
    int MaxMP;
    long int HP;
    long int MP;
    int MaxWeight;
    int Weight;

    // Stats
    int Str;
    int Dex;
    int Int;
    int Con;

    // Attr
    int Attack_Power;
    int Defense;
    int Magic_Defense;
    int Accury;
    int Critical;
    int Dodge;
    int Attack_Speed;
    int Move_Speed;
};

//Items
struct CItem
{
    int itemid;
    int add1;
    int add2;
    int add3;
    int val1;
    int val2;
    int val3;
};

// Position (include respawnid and save town map)
struct CPosition
{
    int RespawnID;
    int Pos_x;
    int Pos_y;
};

// Player information
struct CInfo
{
	char char_name[30];
	int mobid;
	int classid;
    int Exp;
    int Level;
    long int Gold;
    long int Storage_Gold;
    int StatPoints;
    int SkillPoints;
};

// Characters List
struct CCharacter {
	char char_name[30];
	int Level;
	int Exp;
	int Mobid;
	int Str;
	int Int;
	int Dex;
	int Con;
	long int gold;
	CItem eqitems[14];
};

#endif
