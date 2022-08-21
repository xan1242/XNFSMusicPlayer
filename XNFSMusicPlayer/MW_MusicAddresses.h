#pragma once
// MW Function addresses
#define EAXSOUNDUPDATE_ADDRESS 0x004D0610
#define NOTIFYCHYRON_ADDRESS 0x595ED0
#define QUEUENODE_ADDRESS 0x0082FF80
#define SOMEFUNCTION1_ADDRESS 0x45DA30
#define SOMEFUNCTION2_ADDRESS 0x4E7410
#define SNDSYSLEAVECRITICAL_ADDRESS 0x0081BCD9
#define CLEARALLEVENTS_ADDRESS 0x0082C730
#define SFXOBJNOTIFYCHYRON_ADDRESS 0x004F4FC0

// MW Cave exits
#define TIMEREMAININGCAVEEXIT_ADDRESS 0x0082B48C
#define VOLUMECAVEEXIT_ADDRESS 0x0082B2C5
#define STARTPURSUITMUSEXIT_ADDRESS 0x004DF21E
#define STARTAMBIANCEEXIT_ADDRESS 0x004DF133
#define JUKECAVEEXIT_ADDRESS 0x005AB143
#define SAVEDATAEXIT1_ADDRESS 0x004DF37C
#define SAVEDATAEXIT2_ADDRESS 0x0051E008
#define SAVEDATAEXIT3_ADDRESS 0x005509DB
#define SAVEDATAEXIT4_ADDRESS 0x004F4D3B
#define SAVEDATAWRITEREXIT_ADDRESS 0x0051E083
#define NOTIFYCHYRONCAVEEXIT_ADDRESS 0x4F6F58

// MW Injection addresses
#define RESUMECHANNELCAVE_ADDRESS 0x004B2529
#define PAUSECHANNELCAVE_ADDRESS 0x004B2506
#define STARTPURSUITMUSCAVE_ADDRESS 0x004DF219
#define STARTAMBIENCECAVE_ADDRESS 0x004DF12E

#define STOPCHANNELVTABLE_ADDRESS 0x008C5F3C
#define PAUSECHANNELVTABLE_ADDRESS 0x008C5F40
#define PLAYCHANNELVTABLE_ADDRESS 0x008C5F58

#define TIMEREMAININGCAVE_ADDRESS 0x0082B487
#define SOUNDUPDATERCAVE_ADDRESS 0x00663E8B
#define GETNUMBEROFTRAX_ADDRESS 0x00564F42
#define WRITEJUKEPOINTER_ADDRESS 0x005AB13E
#define DISABLEJUKEWRITE1_ADDRESS 0x005AB770
#define DISABLEJUKEWRITE2_ADDRESS 0x005AB769
#define NULLPATHFINDERFUNCJMP_ADDRESS 0x0082BFD0
#define SAVEDATACAVE1_ADDRESS 0x004DF373
#define SAVEDATACAVE2_ADDRESS 0x0051E000
#define SAVEDATACAVE3_ADDRESS 0x005509D3
#define SAVEDATACAVE4_ADDRESS 0x004F4D32
#define SAVEDATAWRITER1_ADDRESS 0x0051E07C
#define SAVEDATAWRITER2_ADDRESS 0x0051E0F6
#define NOTIFYCHYRONCAVE_ADDRESS 0x004F6F51
#define SETVOLCAVE_ADDRESS 0x0082B2CB
#define VOLMULTIPLIERCAVE_ADDRESS 0x0082B2BF
#define VOLCRITICALDISABLE1_ADDRESS 0x0082B2B3
#define VOLCRITICALDISABLE2_ADDRESS 0x0082B2D5
#define QUEUENODECAVE_ADDRESS 0x00830557
#define PATHFINDERCONSTRUCTOR_ADDRESS 0x004F8A5D
#define PATHFINDERDESTRUCTOR_ADDRESS 0x004F8B85

// !1.5 -- music limit destroyer
#define DISABLEFEDATABISEWRITEFROM_ADDRESS 0x005AB7C3
#define DISABLEFEDATABISEWRITETO_ADDRESS 0x005AB84B

// MW Jukebox struct pointers
#define JUKESTRUCTPOINTER1 0x009123D0
#define JUKESTRUCTPOINTER2 0x009123D4

#define PATHFINDER_THING_ADDRESS 0x9C2964
#define SOUNDLOSTWINFOCUS_ADDRESS 0x982C50
#define THEGAMEFLOWMANAGER_ADDRESS 0x00925E90

// RNG fixer - random mode isn't quite random...
#define MW_RNGFIX_ADDRESS 0x004F4F02
#define BGETTICKER_ADDRESS 0x0045CE40

// allow for track changing everywhere, not just in FE
#define TRACKCHANGE_NOP_ADDR 0x005170CF

#define IS_GAME_PAUSED *(bool*)0x0091CAE4