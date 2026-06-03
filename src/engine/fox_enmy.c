#include "global.h"
#include "assets/ast_corneria.h"
#include "assets/ast_sector_x.h"
#include "assets/ast_sector_y.h"
#include "assets/ast_sector_z.h"
#include "assets/ast_venom_1.h"
#include "assets/ast_venom_2.h"
#include "assets/ast_katina.h"
#include "assets/ast_fortuna.h"
#include "assets/ast_aquas.h"
#include "assets/ast_bolse.h"
#include "assets/ast_meteo.h"
#include "assets/ast_solar.h"
#include "assets/ast_macbeth.h"
#include "assets/ast_titania.h"
#include "assets/ast_andross.h"
#include "assets/ast_area_6.h"
#include "assets/ast_training.h"
#include "assets/ast_versus.h"
#include "assets/ast_zoness.h"
#include "port/hooks/Events.h"

s32 D_enmy_Timer_80161670[4];
s32 gLastPathChange;
u8 gMissedZoSearchlight;

ObjectInit* gLevelObjectInits[] = {
    aCoOnRailsLevelObjects, D_ME_6026CC4,      D_SX_602A164,  D_A6_6023F64,
    D_A6_60287A4,           D_SY_602E4F4,      D_VE1_6007E74, D_SO_601F234,
    D_ZO_6026714,           D_ANDROSS_C035154, D_TR_6006AA4,  D_MA_6031000,
    D_TI_6006C60,           D_AQ_602E5C8,      D_FO_600EAD4,  NULL,
    D_KA_6011044,           D_BO_600FF74,      D_SZ_6006EB4,  D_VE2_6014D94,
    D_versus_302DE3C,
};
s32 D_enmy_800CFDF4[] = {
    /*  0 */ OBJ_SCENERY_CO_STONE_ARCH,
    /*  1 */ OBJ_ITEM_SILVER_RING,
    /*  2 */ OBJ_ITEM_SILVER_RING,
    /*  3 */ OBJ_ITEM_SILVER_RING,
    /*  4 */ OBJ_ITEM_SILVER_RING,
    /*  5 */ OBJ_ITEM_BOMB,
    /*  6 */ OBJ_ITEM_BOMB,
    /*  7 */ OBJ_ITEM_BOMB,
    /*  8 */ OBJ_ITEM_BOMB,
    /*  9 */ OBJ_ITEM_LASERS,
    /* 10 */ OBJ_ITEM_LASERS,
    /* 11 */ OBJ_ITEM_LASERS,
    /* 12 */ OBJ_ITEM_LASERS,
    /* 13 */ OBJ_ITEM_1UP,
    /* 14 */ OBJ_ITEM_GOLD_RING,
    /* 15 */ OBJ_ITEM_GOLD_RING,
    /* 16 */ OBJ_ITEM_GOLD_RING,
    /* 17 */ OBJ_ITEM_GOLD_RING,
    /* 18 */ OBJ_ITEM_GOLD_RING,
    /* 19 */ OBJ_ITEM_LASERS,
    /* 20 */ OBJ_ITEM_BOMB,
    /* 21 */ OBJ_ITEM_SILVER_RING,
    /* 22 */ OBJ_ITEM_SILVER_RING,
    /* 23 */ OBJ_ITEM_WING_REPAIR,
    /* 24 */ OBJ_SCENERY_CO_STONE_ARCH,
    /* 25 */ OBJ_ITEM_SILVER_STAR,
};
f32 D_enmy_800CFE5C[] = {
    0.0f, 1.0f, 0.5f, 0.33f, 0.25f, 1.0f,  0.5f,  0.33f, 0.25f, 1.0f, 0.5f, 0.33f, 0.25f,
    1.0f, 1.0f, 1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 0.1f, 1.0f, 1.0f,  1.0f,
};
Vec3f D_enmy_800CFEC4[] = {
    { 0.0f, 0.0f, 50.0f }, { -50.0f, 0.0f, 0.0f }, { 50.0f, 0.0f, 0.0f },
    { 0.0f, 50.0f, 0.0f }, { 0.0f, 0.0f, -50.0f }, { 0.0f, -50.0f, 0.0f },
};
Vec3f D_enmy_800CFF0C[] = {
    { 0.0f, 0.0f, 0.0f },   { 0.0f, -90.0f, 0.0f }, { 0.0f, 90.0f, 0.0f },
    { -90.0f, 0.0f, 0.0f }, { 0.0f, 180.0f, 0.0f }, { 90.0f, 0.0f, 0.0f },
};
u32 gWarpRingSfx[] = {
    NA_SE_WARP_RING_1, NA_SE_WARP_RING_2, NA_SE_WARP_RING_3, NA_SE_WARP_RING_4, NA_SE_WARP_RING_5,
    NA_SE_WARP_RING_6, NA_SE_WARP_RING_7, NA_SE_WARP_RING_7, NA_SE_WARP_RING_7,
};

void Object_PlayerSfx(f32* pos, u32 sfxId, s32 playerNum) {
    PRINTF("CHIME SET \n");
    PRINTF("BOMB SET 1\n");
    PRINTF("BOMB SET 2\n");
    PRINTF("center_X        %f\n");
    PRINTF("Enm->obj.pos.x  %f\n");
    if (!gVersusMode) {
        AUDIO_PLAY_SFX(sfxId, gDefaultSfxSource, 4);
    } else {
        AUDIO_PLAY_SFX(sfxId, pos, playerNum);
    }
}

void Object_Kill(Object* obj, f32* sfxSrc) {
    obj->status = OBJ_FREE;
    Audio_KillSfxBySource(sfxSrc);
}

bool func_enmy_80060FE4(Vec3f* arg0, f32 arg1) {
    Vec3f src;
    Vec3f dest;

    if ((gLevelMode != LEVELMODE_ALL_RANGE) && (gPlayer[0].state != PLAYERSTATE_LEVEL_INTRO)) {
        return true;
    }

    Matrix_RotateY(gCalcMatrix, gPlayer[gPlayerNum].camYaw, MTXF_NEW);

    src.x = arg0->x - gPlayer[gPlayerNum].cam.eye.x;
    src.y = 0.0f;
    src.z = arg0->z - gPlayer[gPlayerNum].cam.eye.z;

    Matrix_MultVec3fNoTranslate(gCalcMatrix, &src, &dest);

    // @port: Extend draw distance up to 32/9
    if ((dest.z < 1000.0f) && (arg1 < dest.z) && (fabsf(dest.x) < (fabsf(dest.z * /* 0.5f */ 1.5f) + 2000.0f))) {
        return true;
    }
    return false;
}

// Unused
bool func_enmy_80061148(Vec3f* arg0, f32 arg1) {
    Vec3f src;
    Vec3f dest;

    if (gLevelMode != LEVELMODE_ALL_RANGE) {
        return true;
    }

    if (gPlayer[0].state == PLAYERSTATE_LEVEL_COMPLETE) {
        return func_enmy_80060FE4(arg0, arg1);
    }

    Matrix_RotateY(gCalcMatrix, gPlayer[gPlayerNum].camYaw, MTXF_NEW);

    src.x = arg0->x - gPlayer[gPlayerNum].cam.eye.x;
    src.y = 0.0f;
    src.z = arg0->z - gPlayer[gPlayerNum].cam.eye.z;

    Matrix_MultVec3fNoTranslate(gCalcMatrix, &src, &dest);

    if ((dest.z < 0.0f) && (arg1 < dest.z) && (fabsf(dest.x) < (fabsf(dest.z * 0.5f) + 500.0f))) {
        return true;
    }
    return false;
}

// -----------------------------------------------------------------------------
// Randomizer helpers (cleaned, commented, verbose logging, relaxed matching)
// -----------------------------------------------------------------------------
// Notes:
// - We maintain a per-level actor pool built from level ObjectInit entries and
//   the static event actor table `sEventActorInfo[]`. If the pool is too small
//   we optionally fill from the global actor table.
// - Replacement candidates exclude a hardcoded list (partners / special actors).
// - We prefer similar draw/action/cullDistance matches but fall back to looser
//   rules when no strict candidate exists.
// - Extensive printf logging added to diagnose decisions at runtime.
// -----------------------------------------------------------------------------

// Randomizer filter toggles — set to 0 to disable (allow) the filter, 1 to enable (enforce) it
#ifndef RANDOMIZER_ENABLED
#define RANDOMIZER_ENABLED 1
#endif

#ifndef RANDOMIZER_FILTER_CULL_DISTANCE
#define RANDOMIZER_FILTER_CULL_DISTANCE 0 // 0 = ignore cull, 1 = enforce cull checks
#endif

#ifndef RANDOMIZER_FILTER_DRAWTYPE
#define RANDOMIZER_FILTER_DRAWTYPE 0 // 0 = ignore drawType mismatch, 1 = enforce
#endif

#ifndef RANDOMIZER_FILTER_ACTION
#define RANDOMIZER_FILTER_ACTION 0 // 0 = ignore action mismatch, 1 = enforce
#endif

#ifndef RANDOMIZER_FILTER_EXCLUDE_PARTNERS
#define RANDOMIZER_FILTER_EXCLUDE_PARTNERS 1 // 0 = allow partners, 1 = exclude
#endif

#ifndef RANDOMIZER_FILTER_EXCLUDE_EVENTS
#define RANDOMIZER_FILTER_EXCLUDE_EVENTS 0 // 0 = allow events, 1 = exclude events from non-event spawns
#endif

// Object type categorization for logging
typedef enum {
    OBJ_CATEGORY_FLIGHT,  // Flying enemies in formation (Garuda, enemy ships, etc.)
    OBJ_CATEGORY_GROUND,  // Ground-based enemies (robots, skibots, etc.)
    OBJ_CATEGORY_BOSS,    // Boss encounters
    OBJ_CATEGORY_TERRAIN, // Obstacles and scenery
    OBJ_CATEGORY_SPECIAL, // Crewmates, event actors, etc.
    OBJ_CATEGORY_UNKNOWN
} ObjectCategory;

// ObjectId -> string mapping (lookup table) for readable logs.
// The enum values in sf64object.h map 0..OBJ_ID_MAX-1 to names below.
static const char* sObjectIdNames[OBJ_ID_MAX] = {
    "OBJ_SCENERY_CO_STONE_ARCH",         /* 0 */
    "OBJ_SCENERY_CO_BUMP_1",             /* 1 */
    "OBJ_SCENERY_CO_BUMP_2",             /* 2 */
    "OBJ_SCENERY_CO_BUMP_3",             /* 3 */
    "OBJ_SCENERY_CO_BUMP_4",             /* 4 */
    "OBJ_SCENERY_CO_BUMP_5",             /* 5 */
    "OBJ_SCENERY_CO_HIGHWAY_1",          /* 6 */
    "OBJ_SCENERY_CO_HIGHWAY_2",          /* 7 */
    "OBJ_SCENERY_CO_HIGHWAY_3",          /* 8 */
    "OBJ_SCENERY_CO_HIGHWAY_4",          /* 9 */
    "OBJ_SCENERY_CO_BUILDING_1",         /*10*/
    "OBJ_SCENERY_CO_BUILDING_2",         /*11*/
    "OBJ_SCENERY_CO_BUILDING_3",         /*12*/
    "OBJ_SCENERY_CO_BUILDING_4",         /*13*/
    "OBJ_SCENERY_CO_BUILDING_5",         /*14*/
    "OBJ_SCENERY_CO_BUILDING_6",         /*15*/
    "OBJ_SCENERY_CO_BUILDING_7",         /*16*/
    "OBJ_SCENERY_CO_BUILDING_8",         /*17*/
    "OBJ_SCENERY_CO_BUILDING_ON_FIRE",   /*18*/
    "OBJ_SCENERY_CO_TOWER",              /*19*/
    "OBJ_SCENERY_CO_ARCH_1",             /*20*/
    "OBJ_SCENERY_CO_ARCH_2",             /*21*/
    "OBJ_SCENERY_CO_ARCH_3",             /*22*/
    "OBJ_SCENERY_CO_RADAR_DISH",         /*23*/
    "OBJ_SCENERY_CO_HIGHWAY_5",          /*24*/
    "OBJ_SCENERY_CO_HIGHWAY_6",          /*25*/
    "OBJ_SCENERY_CO_HIGHWAY_7",          /*26*/
    "OBJ_SCENERY_CO_HIGHWAY_8",          /*27*/
    "OBJ_SCENERY_CO_HIGHWAY_9",          /*28*/
    "OBJ_SCENERY_TI_SKULL",              /*29*/
    "OBJ_SCENERY_TI_RIB_0",              /*30*/
    "OBJ_SCENERY_TI_RIB_1",              /*31*/
    "OBJ_SCENERY_TI_RIB_2",              /*32*/
    "OBJ_SCENERY_TI_RIB_3",              /*33*/
    "OBJ_SCENERY_TI_RIB_4",              /*34*/
    "OBJ_SCENERY_TI_RIB_5",              /*35*/
    "OBJ_SCENERY_TI_RIB_6",              /*36*/
    "OBJ_SCENERY_TI_RIB_7",              /*37*/
    "OBJ_SCENERY_TI_RIB_8",              /*38*/
    "OBJ_SCENERY_ME_TUNNEL",             /*39*/
    "OBJ_SCENERY_CO_BUILDING_9",         /*40*/
    "OBJ_SCENERY_CO_BUILDING_10",        /*41*/
    "OBJ_SCENERY_IBEAM",                 /*42*/
    "OBJ_SCENERY_ZO_ROCK",               /*43*/
    "OBJ_SCENERY_ZO_OIL_RIG_1",          /*44*/
    "OBJ_SCENERY_ZO_OIL_RIG_2",          /*45*/
    "OBJ_SCENERY_ZO_OIL_RIG_3",          /*46*/
    "OBJ_SCENERY_ZO_ISLAND",             /*47*/
    "OBJ_SCENERY_VE1_WALL_1",            /*48*/
    "OBJ_SCENERY_VE1_WALL_2",            /*49*/
    "OBJ_SCENERY_VE1_WALL_3",            /*50*/
    "OBJ_SCENERY_VE1_HALLWAY_OBSTACLE",  /*51*/
    "OBJ_SCENERY_VE1_GENERATOR",         /*52*/
    "OBJ_SCENERY_VE1_WATCH_POST",        /*53*/
    "OBJ_SCENERY_CO_WATERFALL",          /*54*/
    "OBJ_SCENERY_CO_ROCKWALL",           /*55*/
    "OBJ_SCENERY_CO_DOORS",              /*56*/
    "OBJ_SCENERY_TI_PILLAR",             /*57*/
    "OBJ_SCENERY_TI_BRIDGE",             /*58*/
    "OBJ_SCENERY_MA_BUILDING_1",         /*59*/
    "OBJ_SCENERY_MA_BUILDING_2",         /*60*/
    "OBJ_SCENERY_MA_TOWER",              /*61*/
    "OBJ_SCENERY_MA_WALL_1",             /*62*/
    "OBJ_SCENERY_GUILLOTINE_HOUSING",    /*63*/
    "OBJ_SCENERY_MA_GUILLOTINE",         /*64*/
    "OBJ_SCENERY_MA_PROXIMITY_LIGHT",    /*65*/
    "OBJ_SCENERY_MA_WALL_2",             /*66*/
    "OBJ_SCENERY_MA_WALL_3",             /*67*/
    "OBJ_SCENERY_MA_WALL_4",             /*68*/
    "OBJ_SCENERY_MA_TERRAIN_BUMP",       /*69*/
    "OBJ_SCENERY_MA_FLOOR_1",            /*70*/
    "OBJ_SCENERY_MA_FLOOR_2",            /*71*/
    "OBJ_SCENERY_MA_FLOOR_3",            /*72*/
    "OBJ_SCENERY_MA_FLOOR_4",            /*73*/
    "OBJ_SCENERY_MA_FLOOR_5",            /*74*/
    "OBJ_SCENERY_MA_FLOOR_6",            /*75*/
    "OBJ_SCENERY_MA_WEAPONS_FACTORY",    /*76*/
    "OBJ_SCENERY_MA_INDICATOR_SIGN",     /*77*/
    "OBJ_SCENERY_MA_DISTANCE_SIGN_1",    /*78*/
    "OBJ_SCENERY_MA_DISTANCE_SIGN_2",    /*79*/
    "OBJ_SCENERY_MA_DISTANCE_SIGN_3",    /*80*/
    "OBJ_SCENERY_MA_DISTANCE_SIGN_4",    /*81*/
    "OBJ_SCENERY_MA_DISTANCE_SIGN_5",    /*82*/
    "OBJ_SCENERY_MA_TRAIN_STOP_BLOCK",   /*83*/
    "OBJ_SCENERY_MA_RAILROAD_SWITCH_1",  /*84*/
    "OBJ_SCENERY_MA_RAILROAD_SWITCH_2",  /*85*/
    "OBJ_SCENERY_MA_RAILROAD_SWITCH_3",  /*86*/
    "OBJ_SCENERY_MA_RAILROAD_SWITCH_4",  /*87*/
    "OBJ_SCENERY_MA_RAILROAD_SWITCH_5",  /*88*/
    "OBJ_SCENERY_MA_RAILROAD_SWITCH_6",  /*89*/
    "OBJ_SCENERY_MA_RAILROAD_SWITCH_7",  /*90*/
    "OBJ_SCENERY_MA_RAILROAD_SWITCH_8",  /*91*/
    "OBJ_SCENERY_MA_TRAIN_TRACK_1",      /*92*/
    "OBJ_SCENERY_MA_TRAIN_TRACK_2",      /*93*/
    "OBJ_SCENERY_MA_TRAIN_TRACK_3",      /*94*/
    "OBJ_SCENERY_MA_TRAIN_TRACK_4",      /*95*/
    "OBJ_SCENERY_MA_TRAIN_TRACK_5",      /*96*/
    "OBJ_SCENERY_MA_TRAIN_TRACK_6",      /*97*/
    "OBJ_SCENERY_MA_TRAIN_TRACK_7",      /*98*/
    "OBJ_SCENERY_MA_TRAIN_TRACK_8",      /*99*/
    "OBJ_SCENERY_MA_TRAIN_TRACK_9",      /*100*/
    "OBJ_SCENERY_MA_TRAIN_TRACK_10",     /*101*/
    "OBJ_SCENERY_MA_TRAIN_TRACK_11",     /*102*/
    "OBJ_SCENERY_MA_TRAIN_TRACK_12",     /*103*/
    "OBJ_SCENERY_MA_SWITCH_TRACK",       /*104*/
    "OBJ_SCENERY_MA_TRAIN_TRACK_13",     /*105*/
    "OBJ_SCENERY_SY_SHIP_1",             /*106*/
    "OBJ_SCENERY_SY_SHIP_2",             /*107*/
    "OBJ_SCENERY_SY_SHIP_3",             /*108*/
    "OBJ_SCENERY_SY_SHIP_3_DESTROYED",   /*109*/
    "OBJ_SCENERY_SY_SHIP_4",             /*110*/
    "OBJ_SCENERY_SY_SHIP_DEBRIS",        /*111*/
    "OBJ_SCENERY_SY_SHIP_2_DESTROYED",   /*112*/
    "OBJ_SPRITE_SY_SHIP_2",              /*113*/
    "OBJ_SPRITE_SY_SHIP_3",              /*114*/
    "OBJ_SCENERY_SY_SHIP_MISSILE",       /*115*/
    "OBJ_SCENERY_SY_SHIP_WINDOWS",       /*116*/
    "OBJ_SCENERY_AQ_CORAL_REEF_1",       /*117*/
    "OBJ_SCENERY_AQ_TUNNEL_1",           /*118*/
    "OBJ_SCENERY_AQ_ARCH",               /*119*/
    "OBJ_SCENERY_AQ_CORAL_REEF_2",       /*120*/
    "OBJ_SCENERY_AQ_ROCK",               /*121*/
    "OBJ_SCENERY_AQ_WALL_1",             /*122*/
    "OBJ_SCENERY_AQ_ROOF",               /*123*/
    "OBJ_SCENERY_AQ_BUMP_1",             /*124*/
    "OBJ_SCENERY_AQ_TUNNEL_2",           /*125*/
    "OBJ_SCENERY_AQ_BUMP_2",             /*126*/
    "OBJ_SCENERY_VE1_TEMPLE_ENTRANCE",   /*127*/
    "OBJ_SCENERY_VE1_TEMPLE_INTERIOR_1", /*128*/
    "OBJ_SCENERY_VE1_TEMPLE_INTERIOR_2", /*129*/
    "OBJ_SCENERY_VE1_TEMPLE_INTERIOR_3", /*130*/
    "OBJ_SCENERY_AND_PASSAGE",           /*131*/
    "OBJ_SCENERY_AND_DOOR",              /*132*/
    "OBJ_SCENERY_TR_BUILDING",           /*133*/
    "OBJ_SCENERY_AND_PATH_INTERSECTION", /*134*/
    "OBJ_SCENERY_AND_PATH_WALLS",        /*135*/
    "OBJ_SCENERY_AND_PATH_EXIT",         /*136*/
    "OBJ_SCENERY_AND_PATH_ENTRANCE",     /*137*/
    "OBJ_SCENERY_VS_BUILDING_1",         /*138*/
    "OBJ_SCENERY_VS_BUILDING_2",         /*139*/
    "OBJ_SCENERY_VS_PYRAMID_1",          /*140*/
    "OBJ_SCENERY_VS_PYRAMID_2",          /*141*/
    "OBJ_SCENERY_VS_ARCH",               /*142*/
    "OBJ_SCENERY_VS_KA_FLBASE",          /*143*/
    "OBJ_SCENERY_VS_SPACE_JUNK_1",       /*144*/
    "OBJ_SCENERY_VS_SPACE_JUNK_2",       /*145*/
    "OBJ_SCENERY_VS_SPACE_JUNK_3",       /*146*/
    "OBJ_SCENERY_LEVEL_OBJECTS",         /*147*/
    "OBJ_SCENERY_FO_MOUNTAIN_1",         /*148*/
    "OBJ_SCENERY_FO_MOUNTAIN_2",         /*149*/
    "OBJ_SCENERY_FO_MOUNTAIN_3",         /*150*/
    "OBJ_SCENERY_FO_TOWER",              /*151*/
    "OBJ_SCENERY_BO_POLE",               /*152*/
    "OBJ_SCENERY_BO_BUILDING",           /*153*/
    "OBJ_SCENERY_KA_FLBASE",             /*154*/
    "OBJ_SCENERY_UNK_155",               /*155*/
    "OBJ_SCENERY_SY_SHOGUN_SHIP",        /*156*/
    "OBJ_SCENERY_SZ_SPACE_JUNK_3",       /*157*/
    "OBJ_SCENERY_SZ_SPACE_JUNK_1",       /*158*/
    "OBJ_SCENERY_VE2_TOWER",             /*159*/
    "OBJ_SCENERY_VE2_MOUNTAIN",          /*160*/
    "OBJ_SPRITE_CO_POLE",                /*161*/
    "OBJ_SPRITE_CO_TREE",                /*162*/
    "OBJ_SPRITE_FO_POLE",                /*163*/
    "OBJ_SPRITE_FOG_SHADOW",             /*164*/
    "OBJ_SPRITE_CO_RUIN1",               /*165*/
    "OBJ_SPRITE_CO_RUIN2",               /*166*/
    "OBJ_SPRITE_167",                    /*167*/
    "OBJ_SPRITE_168",                    /*168*/
    "OBJ_SPRITE_TI_CACTUS",              /*169*/
    "OBJ_SPRITE_CO_SMOKE",               /*170*/
    "OBJ_SPRITE_VE1_BOSS_TRIGGER1",      /*171*/
    "OBJ_SPRITE_VE1_BOSS_TRIGGER2",      /*172*/
    "OBJ_SPRITE_VE1_BOSS_TRIGGER3",      /*173*/
    "OBJ_SPRITE_VE1_BOSS_TRIGGER4",      /*174*/
    "OBJ_SPRITE_GFOX_TARGET",            /*175*/
    "OBJ_ACTOR_CO_GARUDA_1",             /*176*/
    "OBJ_ACTOR_CO_GARUDA_2",             /*177*/
    "OBJ_ACTOR_CO_GARUDA_3",             /*178*/
    "OBJ_ACTOR_CO_GARUDA_DESTROY",       /*179*/
    "OBJ_ACTOR_ME_MOLAR_ROCK",           /*180*/
    "OBJ_ACTOR_ME_METEOR_1",             /*181*/
    "OBJ_ACTOR_ME_METEOR_2",             /*182*/
    "OBJ_ACTOR_ME_METEOR_SHOWER_1",      /*183*/
    "OBJ_ACTOR_ME_METEOR_SHOWER_2",      /*184*/
    "OBJ_ACTOR_ME_METEOR_SHOWER_3",      /*185*/
    "OBJ_ACTOR_ME_LASER_CANNON_1",       /*186*/
    "OBJ_ACTOR_ME_LASER_CANNON_2",       /*187*/
    "OBJ_ACTOR_AQ_UNK_188",              /*188*/
    "OBJ_ACTOR_DEBRIS",                  /*189*/
    "OBJ_ACTOR_MISSILE_SEEK_TEAM",       /*190*/
    "OBJ_ACTOR_MISSILE_SEEK_PLAYER",     /*191*/
    "OBJ_ACTOR_CO_SKIBOT",               /*192*/
    "OBJ_ACTOR_CO_RADAR",                /*193*/
    "OBJ_ACTOR_ME_MORA",                 /*194*/
    "OBJ_ACTOR_CUTSCENE",                /*195*/
    "OBJ_ACTOR_CO_MOLE_MISSILE",         /*196*/
    "OBJ_ACTOR_ALLRANGE",                /*197*/
    "OBJ_ACTOR_TEAM_BOSS",               /*198*/
    "OBJ_ACTOR_TEAM_ARWING",             /*199*/
    "OBJ_ACTOR_EVENT",                   /*200*/
    "OBJ_ACTOR_ME_METEO_BALL",           /*201*/
    "OBJ_ACTOR_ME_HOPBOT",               /*202*/
    "OBJ_ACTOR_SX_SLIPPY",               /*203*/
    "OBJ_ACTOR_SY_ROBOT",                /*204*/
    "OBJ_ACTOR_MA_LOCOMOTIVE",           /*205*/
    "OBJ_ACTOR_MA_TRAIN_CAR_1",          /*206*/
    "OBJ_ACTOR_207",                     /*207*/
    "OBJ_ACTOR_MA_TRAIN_CAR_2",          /*208*/
    "OBJ_ACTOR_MA_TRAIN_CAR_3",          /*209*/
    "OBJ_ACTOR_MA_TRAIN_CAR_4",          /*210*/
    "OBJ_ACTOR_MA_TRAIN_CAR_5",          /*211*/
    "OBJ_ACTOR_MA_TRAIN_CAR_6",          /*212*/
    "OBJ_ACTOR_MA_TRAIN_CAR_7",          /*213*/
    "OBJ_ACTOR_MA_RAILROAD_SWITCH",      /*214*/
    "OBJ_ACTOR_MA_BOULDER",              /*215*/
    "OBJ_ACTOR_MA_HORIZONTAL_LOCK_BAR",  /*216*/
    "OBJ_ACTOR_MA_VERTICAL_LOCK_BAR",    /*217*/
    "OBJ_ACTOR_MA_BARRIER",              /*218*/
    "OBJ_ACTOR_MA_FALLING_BOULDER",      /*219*/
    "OBJ_ACTOR_MA_BOMBDROP",             /*220*/
    "OBJ_ACTOR_MA_SPEAR",                /*221*/
    "OBJ_ACTOR_MA_SHOCK_BOX",            /*222*/
    "OBJ_ACTOR_MA_RAILWAY_SIGNAL",       /*223*/
    "OBJ_ACTOR_TI_TERRAIN",              /*224*/
    "OBJ_ACTOR_TI_LANDMINE",             /*225*/
    "OBJ_ACTOR_TI_DESERT_ROVER",         /*226*/
    "OBJ_ACTOR_TI_DELPHOR",              /*227*/
    "OBJ_ACTOR_TI_DELPHOR_HEAD",         /*228*/
    "OBJ_ACTOR_TI_DESERT_CRAWLER",       /*229*/
    "OBJ_ACTOR_TI_BOULDER",              /*230*/
    "OBJ_ACTOR_TI_BOMB",                 /*231*/
    "OBJ_ACTOR_TI_RASCO",                /*232*/
    "OBJ_ACTOR_TI_FEKUDA",               /*233*/
    "OBJ_ACTOR_TI_GREAT_FOX",            /*234*/
    "OBJ_ACTOR_ZO_BIRD",                 /*235*/
    "OBJ_ACTOR_ZO_DODORA",               /*236*/
    "OBJ_ACTOR_UNK_237",                 /*237*/
    "OBJ_ACTOR_ZO_FISH",                 /*238*/
    "OBJ_ACTOR_ZO_DODORA_WP_COUNT",      /*239*/
    "OBJ_ACTOR_ZO_Z_GULL",               /*240*/
    "OBJ_ACTOR_ZO_ENERGY_BALL",          /*241*/
    "OBJ_ACTOR_ZO_TROIKA",               /*242*/
    "OBJ_ACTOR_ZO_SHRIMP",               /*243*/
    "OBJ_ACTOR_ZO_OBNEMA",               /*244*/
    "OBJ_ACTOR_ZO_BALL",                 /*245*/
    "OBJ_ACTOR_ZO_MINE",                 /*246*/
    "OBJ_ACTOR_ZO_BARRIER",              /*247*/
    "OBJ_ACTOR_ZO_CRANE_MAGNET",         /*248*/
    "OBJ_ACTOR_SPIKEBALL",               /*249*/
    "OBJ_ACTOR_ZO_TANKER",               /*250*/
    "OBJ_ACTOR_ZO_CONTAINER",            /*251*/
    "OBJ_ACTOR_ZO_RADARBUOY",            /*252*/
    "OBJ_ACTOR_ZO_SUPPLYCRANE",          /*253*/
    "OBJ_ACTOR_ZO_SEARCHLIGHT",          /*254*/
    "OBJ_ACTOR_255",                     /*255*/
    "OBJ_ACTOR_256",                     /*256*/
    "OBJ_ACTOR_257",                     /*257*/
    "OBJ_ACTOR_AQ_PEARL",                /*258*/
    "OBJ_ACTOR_AQ_ANGLERFISH",           /*259*/
    "OBJ_ACTOR_AQ_GAROA",                /*260*/
    "OBJ_ACTOR_AQ_SCULPIN",              /*261*/
    "OBJ_ACTOR_AQ_SPINDLYFISH",          /*262*/
    "OBJ_ACTOR_AQ_SQUID",                /*263*/
    "OBJ_ACTOR_AQ_SEAWEED",              /*264*/
    "OBJ_ACTOR_AQ_BOULDER",              /*265*/
    "OBJ_ACTOR_AQ_CORAL",                /*266*/
    "OBJ_ACTOR_AQ_JELLYFISH",            /*267*/
    "OBJ_ACTOR_AQ_FISHGROUP",            /*268*/
    "OBJ_ACTOR_AQ_STONE_COLUMN",         /*269*/
    "OBJ_ACTOR_AQ_OYSTER",               /*270*/
    "OBJ_ACTOR_BO_SHIELD_REACTOR",       /*271*/
    "OBJ_ACTOR_BO_LASER_CANNON",         /*272*/
    "OBJ_ACTOR_FO_RADAR",                /*273*/
    "OBJ_ACTOR_SZ_SPACE_JUNK",           /*274*/
    "OBJ_ACTOR_SO_ROCK_1",               /*275*/
    "OBJ_ACTOR_SO_ROCK_2",               /*276*/
    "OBJ_ACTOR_SO_ROCK_3",               /*277*/
    "OBJ_ACTOR_SO_WAVE",                 /*278*/
    "OBJ_ACTOR_SO_PROMINENCE",           /*279*/
    "OBJ_ACTOR_VE1_PILLAR_1",            /*280*/
    "OBJ_ACTOR_VE1_PILLAR_2",            /*281*/
    "OBJ_ACTOR_VE1_PILLAR_3",            /*282*/
    "OBJ_ACTOR_VE1_PILLAR_4",            /*283*/
    "OBJ_ACTOR_VE1_MONKEY_STATUE",       /*284*/
    "OBJ_ACTOR_AND_LASER_EMITTER",       /*285*/
    "OBJ_ACTOR_AND_BRAIN_WASTE",         /*286*/
    "OBJ_ACTOR_AND_EXPLOSION",           /*287*/
    "OBJ_ACTOR_AND_RADIO",               /*288*/
    "OBJ_ACTOR_AND_JAMES_TRIGGER",       /*289*/
    "OBJ_ACTOR_AND_BOSS_TIMER_SET",      /*290*/
    "OBJ_ACTOR_SUPPLIES",                /*291*/
    "OBJ_BOSS_CO_GRANGA",                /*292*/
    "OBJ_BOSS_CO_CARRIER",               /*293*/
    "OBJ_BOSS_CO_CARRIER_LEFT",          /*294*/
    "OBJ_BOSS_CO_CARRIER_UPPER",         /*295*/
    "OBJ_BOSS_CO_CARRIER_BOTTOM",        /*296*/
    "OBJ_BOSS_ME_CRUSHER",               /*297*/
    "OBJ_BOSS_ME_CRUSHER_SHIELD",        /*298*/
    "OBJ_BOSS_UNK_299",                  /*299*/
    "OBJ_BOSS_UNK_300",                  /*300*/
    "OBJ_BOSS_AQ_UNK_301",               /*301*/
    "OBJ_BOSS_A6_GORGON",                /*302*/
    "OBJ_BOSS_SX_SPYBORG",               /*303*/
    "OBJ_BOSS_SX_SPYBORG_LEFT_ARM",      /*304*/
    "OBJ_BOSS_SX_SPYBORG_RIGHT_ARM",     /*305*/
    "OBJ_BOSS_TI_GORAS",                 /*306*/
    "OBJ_BOSS_ZO_SARUMARINE",            /*307*/
    "OBJ_BOSS_FO_BASE",                  /*308*/
    "OBJ_BOSS_BO_BASE",                  /*309*/
    "OBJ_BOSS_BO_BASE_SHIELD",           /*310*/
    "OBJ_BOSS_BO_BASE_CORE",             /*311*/
    "OBJ_BOSS_VE2_BASE",                 /*312*/
    "OBJ_BOSS_SZ_GREAT_FOX",             /*313*/
    "OBJ_BOSS_SY_SHOGUN",                /*314*/
    "OBJ_BOSS_SO_VULKAIN",               /*315*/
    "OBJ_BOSS_KA_SAUCERER",              /*316*/
    "OBJ_BOSS_KA_FLBASE",                /*317*/
    "OBJ_BOSS_AQ_BACOON",                /*318*/
    "OBJ_BOSS_VE1_GOLEMECH",             /*319*/
    "OBJ_BOSS_AND_ANDROSS",              /*320*/
    "OBJ_BOSS_AND_BRAIN",                /*321*/
    "OBJ_ITEM_LASERS",                   /*322*/
    "OBJ_ITEM_CHECKPOINT",               /*323*/
    "OBJ_ITEM_SILVER_RING",              /*324*/
    "OBJ_ITEM_SILVER_STAR",              /*325*/
    "OBJ_ITEM_METEO_WARP",               /*326*/
    "OBJ_ITEM_BOMB",                     /*327*/
    "OBJ_ITEM_PATH_SPLIT_X",             /*328*/
    "OBJ_ITEM_PATH_TURN_LEFT",           /*329*/
    "OBJ_ITEM_PATH_TURN_RIGHT",          /*330*/
    "OBJ_ITEM_PATH_SPLIT_Y",             /*331*/
    "OBJ_ITEM_PATH_TURN_UP",             /*332*/
    "OBJ_ITEM_PATH_TURN_DOWN",           /*333*/
    "OBJ_ITEM_RING_CHECK",               /*334*/
    "OBJ_ITEM_1UP",                      /*335*/
    "OBJ_ITEM_GOLD_RING",                /*336*/
    "OBJ_ITEM_WING_REPAIR",              /*337*/
    "OBJ_ITEM_TRAINING_RING",            /*338*/
    "OBJ_EFFECT_FIRE_SMOKE_1",           /*339*/
    "OBJ_EFFECT_FIRE_SMOKE_2",           /*340*/
    "OBJ_EFFECT_FIRE_SMOKE_3",           /*341*/
    "OBJ_EFFECT_SMOKE_1",                /*342*/
    "OBJ_EFFECT_SMOKE_2",                /*343*/
    "OBJ_EFFECT_EXPLOSION_MARK_1",       /*344*/
    "OBJ_EFFECT_LASER_MARK_1",           /*345*/
    "OBJ_EFFECT_346",                    /*346*/
    "OBJ_EFFECT_347",                    /*347*/
    "OBJ_EFFECT_348",                    /*348*/
    "OBJ_EFFECT_349",                    /*349*/
    "OBJ_EFFECT_350",                    /*350*/
    "OBJ_EFFECT_351",                    /*351*/
    "OBJ_EFFECT_CLOUDS",                 /*352*/
    "OBJ_EFFECT_ENEMY_LASER_1",          /*353*/
    "OBJ_EFFECT_354",                    /*354*/
    "OBJ_EFFECT_355",                    /*355*/
    "OBJ_EFFECT_356",                    /*356*/
    "OBJ_EFFECT_357",                    /*357*/
    "OBJ_EFFECT_KA_ENERGY_PARTICLES",    /*358*/
    "OBJ_EFFECT_359",                    /*359*/
    "OBJ_EFFECT_360",                    /*360*/
    "OBJ_EFFECT_361",                    /*361*/
    "OBJ_EFFECT_362",                    /*362*/
    "OBJ_EFFECT_363",                    /*363*/
    "OBJ_EFFECT_364",                    /*364*/
    "OBJ_EFFECT_365",                    /*365*/
    "OBJ_EFFECT_366",                    /*366*/
    "OBJ_EFFECT_367",                    /*367*/
    "OBJ_EFFECT_368",                    /*368*/
    "OBJ_EFFECT_369",                    /*369*/
    "OBJ_EFFECT_370",                    /*370*/
    "OBJ_EFFECT_371",                    /*371*/
    "OBJ_EFFECT_372",                    /*372*/
    "OBJ_EFFECT_TIMED_SFX",              /*373*/
    "OBJ_EFFECT_374",                    /*374*/
    "OBJ_EFFECT_375",                    /*375*/
    "OBJ_EFFECT_376",                    /*376*/
    "OBJ_EFFECT_377",                    /*377*/
    "OBJ_EFFECT_378",                    /*378*/
    "OBJ_EFFECT_379",                    /*379*/
    "OBJ_EFFECT_380",                    /*380*/
    "OBJ_EFFECT_381",                    /*381*/
    "OBJ_EFFECT_382",                    /*382*/
    "OBJ_EFFECT_383",                    /*383*/
    "OBJ_EFFECT_384",                    /*384*/
    "OBJ_EFFECT_385",                    /*385*/
    "OBJ_EFFECT_386",                    /*386*/
    "OBJ_EFFECT_387",                    /*387*/
    "OBJ_EFFECT_388",                    /*388*/
    "OBJ_EFFECT_389",                    /*389*/
    "OBJ_EFFECT_390",                    /*390*/
    "OBJ_EFFECT_391",                    /*391*/
    "OBJ_EFFECT_392",                    /*392*/
    "OBJ_EFFECT_393",                    /*393*/
    "OBJ_EFFECT_394",                    /*394*/
    "OBJ_EFFECT_395",                    /*395*/
    "OBJ_EFFECT_396",                    /*396*/
    "OBJ_EFFECT_397",                    /*397*/
    "OBJ_EFFECT_398",                    /*398*/
    "OBJ_EFFECT_399",                    /*399*/
    "OBJ_ENV_SMALL_ROCKS_ENABLE",        /*400*/
    "OBJ_ENV_SMALL_ROCKS_DISABLE",       /*401*/
    "OBJ_UNK_402",                       /*402*/
    "OBJ_UNK_403",                       /*403*/
    "OBJ_UNK_404",                       /*404*/
    "OBJ_UNK_405"                        /*405*/
};

// Returns human-readable name for an ObjectId (handles event ids).
static const char* Randomizer_ObjectIdToString(s32 id) {
    if (id >= 0 && id < OBJ_ID_MAX) {
        return sObjectIdNames[id];
    }
    if (id >= ACTOR_EVENT_ID) {
        static char buf[64];
        snprintf(buf, sizeof(buf), "EVENT_%d", id - ACTOR_EVENT_ID);
        return buf;
    }
    return "OBJ_INVALID_OR_UNKNOWN";
}

static ObjectCategory GetObjectCategory(ObjectId id) {
    // Handle event actors
    if (id >= ACTOR_EVENT_ID) {
        s32 evIndex = id - ACTOR_EVENT_ID;
        // Check event actor type by looking at the event index
        // Events 22 (Slippy), 50 (Peppy), 80 (Falco) are flight-type crewmates
        if ((evIndex == 22) || (evIndex == 50) || (evIndex == 80)) {
            return OBJ_CATEGORY_FLIGHT; // Crewmates flying alongside you
        }
        return OBJ_CATEGORY_SPECIAL; // Other events
    }

    // Flight types
    if ((id >= OBJ_ACTOR_CO_GARUDA_1 && id <= OBJ_ACTOR_CO_GARUDA_3) || (id == OBJ_ACTOR_SX_SLIPPY) ||
        (id == OBJ_ACTOR_SY_ROBOT) || (id == OBJ_ACTOR_ME_HOPBOT) || (id == OBJ_ACTOR_ZO_Z_GULL) ||
        (id == OBJ_ACTOR_ZO_DODORA) || (id == OBJ_ACTOR_ME_MORA) || (id == OBJ_ACTOR_ALLRANGE)) {
        return OBJ_CATEGORY_FLIGHT;
    }
    // Ground types
    if ((id == OBJ_ACTOR_CO_SKIBOT) || (id == OBJ_ACTOR_TI_DESERT_ROVER) || (id == OBJ_ACTOR_TI_DELPHOR) ||
        (id == OBJ_ACTOR_TI_RASCO) || (id == OBJ_ACTOR_TI_FEKUDA) || (id == OBJ_ACTOR_ME_METEOR_1) ||
        (id == OBJ_ACTOR_ME_METEOR_2) || (id == OBJ_ACTOR_ME_LASER_CANNON_1) || (id == OBJ_ACTOR_ME_LASER_CANNON_2) ||
        (id == OBJ_ACTOR_ME_MOLAR_ROCK) || (id == OBJ_ACTOR_ZO_SHRIMP) || (id == OBJ_ACTOR_ZO_OBNEMA) ||
        (id == OBJ_ACTOR_ZO_TROIKA) || (id == OBJ_ACTOR_AQ_ANGLERFISH) || (id == OBJ_ACTOR_AQ_GAROA) ||
        (id == OBJ_ACTOR_AQ_SCULPIN) || (id == OBJ_ACTOR_AQ_JELLYFISH)) {
        return OBJ_CATEGORY_GROUND;
    }
    // Boss types
    if (id >= OBJ_BOSS_START && id < OBJ_BOSS_MAX) {
        return OBJ_CATEGORY_BOSS;
    }
    // Terrain/scenery
    if (id < OBJ_SCENERY_MAX || (id >= OBJ_SPRITE_START && id < OBJ_SPRITE_MAX)) {
        return OBJ_CATEGORY_TERRAIN;
    }
    // Special (crewmates, event actors, team members)
    if ((id == OBJ_ACTOR_TEAM_BOSS) || (id == OBJ_ACTOR_TEAM_ARWING) || (id == OBJ_ACTOR_EVENT) ||
        (id == OBJ_ACTOR_CUTSCENE) || (id == OBJ_ACTOR_ALLRANGE)) {
        return OBJ_CATEGORY_SPECIAL;
    }
    // Items
    if (id >= OBJ_ITEM_START && id < OBJ_ITEM_MAX) {
        return OBJ_CATEGORY_TERRAIN;
    }

    return OBJ_CATEGORY_UNKNOWN;
}

static const char* GetCategoryName(ObjectCategory cat) {
    switch (cat) {
        case OBJ_CATEGORY_FLIGHT:
            return "FLIGHT";
        case OBJ_CATEGORY_GROUND:
            return "GROUND";
        case OBJ_CATEGORY_BOSS:
            return "BOSS";
        case OBJ_CATEGORY_TERRAIN:
            return "TERRAIN";
        case OBJ_CATEGORY_SPECIAL:
            return "SPECIAL";
        default:
            return "UNKNOWN";
    }
}

// Enhanced logging helper
static void LogReplacement(const char* context, ObjectId origId, ObjectId newId, bool replaced, const char* reason) {
    ObjectCategory origCat = GetObjectCategory(origId);
    const char* origStr = Randomizer_ObjectIdToString((s32) origId);
    const char* newStr = Randomizer_ObjectIdToString((s32) newId);

    if (replaced) {
        printf("[RANDOMIZER] %s | %s | REPLACED | %d (%s) → %d (%s) | Reason: %s\n", context, GetCategoryName(origCat),
               (s32) origId, origStr, (s32) newId, newStr, reason);
    } else {
        printf("[RANDOMIZER] %s | %s | NOT_REPLACED | %d (%s) | Reason: %s\n", context, GetCategoryName(origCat),
               (s32) origId, origStr, reason);
    }
}

#define LEVEL_ACTOR_POOL_MAX 256
#define MIN_LEVEL_POOL_SIZE 3

static u16 s_levelActorPool[LEVEL_ACTOR_POOL_MAX];
static s32 s_levelActorCount = 0;
static u8 s_poolBuiltLevel = 0;



// Helper: compare cull distances with relative tolerance + small-object special-case.
// strict == true uses stricter ratio; false uses relaxed ratio.
static bool CullDistanceCompatible(f32 origCull, f32 candCull, bool strict) {
    if (!RANDOMIZER_FILTER_CULL_DISTANCE) {
        return true;
    }

    const f32 SMALL_MAX_STRICT = 500.0f; // both smaller than this -> compatible
    const f32 SMALL_MAX_RELAXED = 1000.0f;
    const f32 RATIO_STRICT = 4.0f;
    const f32 RATIO_RELAXED = 8.0f;

    if (origCull <= 0.0f || candCull <= 0.0f) {
        return true;
    }

    if (strict) {
        if ((origCull < SMALL_MAX_STRICT) && (candCull < SMALL_MAX_STRICT)) {
            return true;
        }
    } else {
        if ((origCull < SMALL_MAX_RELAXED) && (candCull < SMALL_MAX_RELAXED)) {
            return true;
        }
    }

    f32 a = origCull;
    f32 b = candCull;
    f32 ratio = (a > b) ? (a / b) : (b / a);

    return strict ? (ratio <= RATIO_STRICT) : (ratio <= RATIO_RELAXED);
}

// IDs that should never be swapped into or out of
static const ObjectId s_excludeActorIds[] = {
    OBJ_ACTOR_TEAM_BOSS, OBJ_ACTOR_TEAM_ARWING,
    // Add other partner/special IDs here if you find them in logs.
};

static bool IsExcludedActor(ObjectId id) {
    for (size_t i = 0; i < ARRAY_COUNT(s_excludeActorIds); ++i) {
        if (s_excludeActorIds[i] == id) {
            return true;
        }
    }
    return false;
}

// Event actor metadata is defined in fox_enmy2.c as `sEventActorInfo[]`.
typedef struct {
    void* dList;
    f32* hitbox;
    f32 scale;
    f32 cullDistance;
    f32 unk_10;
    u8 info_unk_16;
    u8 info_unk_14;
    u8 sfx;
    u8 info_unk_19;
    f32 targetOffset;
    u8 bonus;
} EventActorInfo;
extern EventActorInfo sEventActorInfo[];
#define EVENT_ACTOR_COUNT 108

// Helper to get cullDistance and whether the id is an event actor.
// Always safe — reads sEventActorInfo[] for event ids and gObjectInfo[] otherwise.
static void GetCullAndFlags(s32 id, f32* outCull, bool* outIsEvent) {
    if (id >= ACTOR_EVENT_ID) {
        s32 evIndex = id - ACTOR_EVENT_ID;
        if (evIndex >= 0 && evIndex < EVENT_ACTOR_COUNT) {
            *outCull = sEventActorInfo[evIndex].cullDistance;
            *outIsEvent = true;
            return;
        }
        // Out-of-range event id - fallback to a conservative cullDistance
        *outCull = 1000.0f;
        *outIsEvent = true;
    } else {
        // Normal actor/object
        *outCull = gObjectInfo[id].cullDistance;
        *outIsEvent = false;
    }
}

// Build the actor candidate pool. This is only run once per level load
// (controlled by s_poolBuiltLevel). It collects:
//  1) actors from the level ObjectInit table (if present).
//  2) event/script actors from the compile-time sEventActorInfo table.
//  3) optional global fallback from gObjectInfo when pool is too small.
//
// Verbose logging shows what we added and final pool size.
static void BuildActorPoolIfNeeded(void) {
    if (s_poolBuiltLevel) {
        return;
    }

    s_levelActorCount = 0;
    ObjectInit* levelObjs = gLevelObjects;
    if (levelObjs == NULL) {
        levelObjs = SEGMENTED_TO_VIRTUAL(gLevelObjectInits[gCurrentLevel]);
    }

    printf("RANDOMIZER :: BuildActorPoolIfNeeded :: Building actor pool for level %d\n", gCurrentLevel);

    // 1) From level ObjectInit
    if (levelObjs != NULL) {
        for (s32 i = 0; i < 10000; ++i) {
            s16 id = levelObjs[i].id;
            if (id <= OBJ_INVALID) {
                break;
            }
            if ((id >= OBJ_ACTOR_START) && (id < OBJ_ACTOR_MAX)) {
                if (IsExcludedActor((ObjectId) id)) {
                    continue;
                }
                // dedupe
                s32 found = 0;
                for (s32 j = 0; j < s_levelActorCount; ++j) {
                    if ((s32) s_levelActorPool[j] == id) {
                        found = 1;
                        break;
                    }
                }
                if (!found && s_levelActorCount < LEVEL_ACTOR_POOL_MAX) {
                    s_levelActorPool[s_levelActorCount++] = (u16) id;
                }
            }
        }
    }

    // 2) Add event/scripted actor types (safe static table)
    for (s32 ev = 0; ev < EVENT_ACTOR_COUNT; ++ev) {
        s32 candidate = ACTOR_EVENT_ID + ev;
        if (IsExcludedActor((ObjectId) candidate)) {
            continue;
        }
        // dedupe
        s32 found = 0;
        for (s32 j = 0; j < s_levelActorCount; ++j) {
            if ((s32) s_levelActorPool[j] == candidate) {
                found = 1;
                break;
            }
        }
        if (!found && s_levelActorCount < LEVEL_ACTOR_POOL_MAX) {
            s_levelActorPool[s_levelActorCount++] = (u16) candidate;
        }
    }

    // 3) Fallback: add from global actor range if pool is too small
    if (s_levelActorCount < MIN_LEVEL_POOL_SIZE) {
        printf("RANDOMIZER :: BuildActorPoolIfNeeded :: level-derived pool small (%d entries) — falling back to global actor list\n",
               s_levelActorCount);
        for (s32 id = OBJ_ACTOR_START; id < OBJ_ACTOR_MAX; ++id) {
            if (IsExcludedActor((ObjectId) id)) {
                continue;
            }
            s32 found = 0;
            for (s32 j = 0; j < s_levelActorCount; ++j) {
                if ((s32) s_levelActorPool[j] == id) {
                    found = 1;
                    break;
                }
            }
            if (!found && s_levelActorCount < LEVEL_ACTOR_POOL_MAX) {
                s_levelActorPool[s_levelActorCount++] = (u16) id;
            }
        }
    }

    // Log pool summary (show up to first 24 entries)
    printf("RANDOMIZER :: BuildActorPoolIfNeeded :: final pool size = %d\n", s_levelActorCount);
    {
        char buf[256];
        int off = 0;
        off += snprintf(buf + off, sizeof(buf) - off, "RANDOMIZER :: BuildActorPoolIfNeeded :: pool ids:");
        for (s32 i = 0; i < s_levelActorCount && i < 24; ++i) {
            off += snprintf(buf + off, sizeof(buf) - off, " %d", s_levelActorPool[i]);
        }
        if (s_levelActorCount > 24) {
            snprintf(buf + off, sizeof(buf) - off, " ...");
        }
        printf("%s\n", buf);
    }

    s_poolBuiltLevel = 1;
}

static s16 PickRandomActorFromPoolDifferent(s16 originalId, bool allowEvent) {
    if (!RANDOMIZER_ENABLED) {
        LogReplacement("PickRandomActorFromPoolDifferent", originalId, originalId, false, "RANDOMIZER_ENABLED=0");
        return originalId;
    }

    BuildActorPoolIfNeeded();

    if (s_levelActorCount <= 0) {
        LogReplacement("PickRandomActorFromPoolDifferent", originalId, originalId, false, "Pool is empty");
        return originalId;
    }

    if (s_levelActorCount == 1 && (s16) s_levelActorPool[0] == originalId) {
        LogReplacement("PickRandomActorFromPoolDifferent", originalId, originalId, false,
                       "Only 1 candidate equals original");
        return originalId;
    }

    s32 candidates_any[LEVEL_ACTOR_POOL_MAX];
    s32 anyCount = 0;
    s32 rejectedCount = 0;

    f32 origCull;
    bool origIsEvent;
    GetCullAndFlags(originalId, &origCull, &origIsEvent);

    ObjectInfo origInfo;
    if (!origIsEvent) {
        origInfo = gObjectInfo[originalId];
    }

    printf("[RANDOMIZER_DETAIL] Pool iteration for %d (%s), pool size=%d, allowEvent=%d\n", originalId,
           Randomizer_ObjectIdToString(originalId), s_levelActorCount, allowEvent);

    for (s32 i = 0; i < s_levelActorCount; ++i) {
        s16 candId = (s16) s_levelActorPool[i];
        const char* rejectReason = NULL;

        // Skip self
        if (candId == originalId) {
            continue;
        }

        // Check event filter
        if (!allowEvent && (candId >= ACTOR_EVENT_ID)) {
            rejectReason = "Event actor (allowEvent=false)";
        }
        // Check exclude events filter
        else if (RANDOMIZER_FILTER_EXCLUDE_EVENTS && (candId >= ACTOR_EVENT_ID)) {
            rejectReason = "Event actor (RANDOMIZER_FILTER_EXCLUDE_EVENTS=1)";
        }
        // Check excluded partners
        else if (RANDOMIZER_FILTER_EXCLUDE_PARTNERS && IsExcludedActor((ObjectId) candId)) {
            rejectReason = "Excluded actor (partner/special)";
        }

        // If already rejected, log and skip
        if (rejectReason != NULL) {
            printf("[RANDOMIZER_DETAIL]   Candidate %d (%s) REJECTED: %s\n", candId,
                   Randomizer_ObjectIdToString(candId), rejectReason);
            rejectedCount++;
            continue;
        }

        f32 candCull;
        bool candIsEvent;
        GetCullAndFlags(candId, &candCull, &candIsEvent);

        bool passes = true;
        const char* failReason = NULL;

        // Apply drawType filter if enabled
        if (RANDOMIZER_FILTER_DRAWTYPE && !origIsEvent && !candIsEvent) {
            ObjectInfo candInfo = gObjectInfo[candId];
            if (candInfo.drawType != origInfo.drawType) {
                passes = false;
                failReason = "drawType mismatch";
            }
        }

        // Apply action filter if enabled
        if (passes && RANDOMIZER_FILTER_ACTION && !origIsEvent && !candIsEvent) {
            ObjectInfo candInfo = gObjectInfo[candId];
            if ((candInfo.action != NULL) && (origInfo.action != NULL) && (candInfo.action != origInfo.action)) {
                passes = false;
                failReason = "action mismatch";
            }
        }

        // Apply cull distance filter if enabled
        if (passes && RANDOMIZER_FILTER_CULL_DISTANCE) {
            if (!CullDistanceCompatible(origCull, candCull, false)) {
                passes = false;
                failReason = "cull distance incompatible";
            }
        }

        if (passes) {
            candidates_any[anyCount++] = i;
            printf("[RANDOMIZER_DETAIL]   Candidate %d (%s) ACCEPTED (cull: %.1f)\n", candId,
                   Randomizer_ObjectIdToString(candId), candCull);
        } else {
            printf("[RANDOMIZER_DETAIL]   Candidate %d (%s) REJECTED: %s\n", candId,
                   Randomizer_ObjectIdToString(candId), failReason);
            rejectedCount++;
        }
    }

    printf("[RANDOMIZER_DETAIL] Summary: %d accepted, %d rejected\n", anyCount, rejectedCount);

    if (anyCount <= 0) {
        LogReplacement("PickRandomActorFromPoolDifferent", originalId, originalId, false,
                       "No candidates passed filters");
        return originalId;
    }

    s32 finalPickIdx = (s32) (Rand_ZeroOne() * (f32) anyCount);
    if (finalPickIdx < 0)
        finalPickIdx = 0;
    if (finalPickIdx >= anyCount)
        finalPickIdx = anyCount - 1;
    s32 chosenPoolIndex = candidates_any[finalPickIdx];

    s16 chosenId = (s16) s_levelActorPool[chosenPoolIndex];
    LogReplacement("PickRandomActorFromPoolDifferent", originalId, chosenId, true, "Randomly selected from candidates");
    return chosenId;
}

// MaybeRandomizeObjectInit: called when level objects are being loaded/inserted.
// It will attempt to replace actor-range ids with a picked replacement.
static void MaybeRandomizeObjectInit(ObjectInit* objInit) {
    // Only run for Corneria during testing
    //if (gCurrentLevel != LEVEL_CORNERIA) {
    //    return;
    //}

    // Handle both regular actors AND event actors
    bool isRegularActor = (objInit->id >= OBJ_ACTOR_START) && (objInit->id < OBJ_ACTOR_MAX);
    bool isEventActor = (objInit->id >= ACTOR_EVENT_ID);

    if (!isRegularActor && !isEventActor) {
        LogReplacement("MaybeRandomizeObjectInit", objInit->id, objInit->id, false, "Not an actor ID");
        return;
    }

    if (IsExcludedActor((ObjectId) objInit->id)) {
        LogReplacement("MaybeRandomizeObjectInit", objInit->id, objInit->id, false, "Excluded actor");
        return;
    }

    s16 original = (s16) objInit->id;
    // For event actors, allow picking other events; for regular actors, also allow events
    s16 newId = PickRandomActorFromPoolDifferent((s16) objInit->id, true);
    if (newId != original) {
        ObjectCategory cat = GetObjectCategory((ObjectId) original);
        printf("[RANDOMIZER_PLACEMENT] %s Object at pos(x=%.1f, y=%.1f, z=%.1f) replaced\n", GetCategoryName(cat),
               (f32) objInit->xPos, (f32) objInit->yPos, (f32) objInit->zPos1);
        objInit->id = (s16) newId;
    } else {
        LogReplacement("MaybeRandomizeObjectInit", original, original, false, "No replacement found");
    }
}

// -----------------------------------------------------------------------------
// Object_SetInfo: safely handle both normal and event actor ids when populating
// an ObjectInfo structure. Event actor ids are encoded as ACTOR_EVENT_ID + eventType.
// -----------------------------------------------------------------------------
void Object_SetInfo(ObjectInfo* info, u32 objId) {
    // If this is an event-actor id, populate from sEventActorInfo table.
    if (objId >= ACTOR_EVENT_ID) {
        s32 evIndex = (s32) objId - ACTOR_EVENT_ID;
        if ((evIndex >= 0) && (evIndex < EVENT_ACTOR_COUNT)) {
            EventActorInfo* ev = &sEventActorInfo[evIndex];

            // Map EventActorInfo fields into ObjectInfo conservatively.
            info->dList = (Gfx*) ev->dList;
            // drawType is inferred — event actors often use a display list, but this is a conservative guess.
            info->drawType = (ev->dList != NULL) ? 1 : 0;
            info->action = NULL; // event actors run through the event system
            info->hitbox = SEGMENTED_TO_VIRTUAL(ev->hitbox);
            info->cullDistance = ev->cullDistance;
            info->unk_14 = (s16) ev->info_unk_14;
            info->unk_16 = (s16) ev->info_unk_16;
            info->damage = 0;
            info->unk_19 = ev->info_unk_19;
            info->targetOffset = ev->targetOffset;
            info->bonus = ev->bonus;

            if (gLevelMode == LEVELMODE_TURRET) {
                info->cullDistance += 200.0f;
            }
            printf("RANDOMIZER :: Object_SetInfo :: Object_SetInfo filled from sEventActorInfo index %d (objId=%d)\n", evIndex, objId);
            return;
        } else {
            // Unexpected event index — log and fall through to safe fallback
            printf("RANDOMIZER :: Object_SetInfo :: Object_SetInfo received out-of-range event objId %u — falling back\n", objId);
        }
    }

    // Defensive bounds check: use OBJ_ID_MAX (enum) instead of ARRAY_COUNT(gObjectInfo)
    // to avoid referencing the size of an externally-defined array whose type may be incomplete here.
    if (objId > OBJ_ID_MAX) {
        printf("RANDOMIZER :: Object_SetInfo :: Object_SetInfo called with invalid objId %u — using fallback values\n", objId);
        memset(info, 0, sizeof(*info));
        info->hitbox = NULL;
        info->cullDistance = 1000.0f;
        return;
    }

    // Normal object id: copy from gObjectInfo.
    *info = gObjectInfo[objId];
    info->hitbox = SEGMENTED_TO_VIRTUAL(gObjectInfo[objId].hitbox);
    if (gLevelMode == LEVELMODE_TURRET) {
        info->cullDistance += 200.0f;
    }
    printf("RANDOMIZER :: Object_SetInfo :: Object_SetInfo filled from gObjectInfo for objId %u\n", objId);
}

void Scenery_Initialize(Scenery* this) {
    s32 i;
    u8* ptr = (u8*) this;

    for (i = 0; i < sizeof(Scenery); i++, ptr++) {
        *ptr = 0;
    }
}

void Sprite_Initialize(Sprite* this) {
    s32 i;
    u8* ptr = (u8*) this;

    for (i = 0; i < sizeof(Sprite); i++, ptr++) {
        *ptr = 0;
    }
}

void Actor_Initialize(Actor* this) {
    s32 i;
    u8* ptr = (u8*) this;

    for (i = 0; i < sizeof(Actor); i++, ptr++) {
        *ptr = 0;
    }
    this->scale = 1.0f;
}

void Boss_Initialize(Boss* this) {
    s32 i;
    u8* ptr = (u8*) this;

    for (i = 0; i < sizeof(Boss); i++, ptr++) {
        *ptr = 0;
    }
    this->scale = 1.0f;
}

void Item_Initialize(Item* this) {
    s32 i;
    u8* ptr = (u8*) this;

    for (i = 0; i < sizeof(Item); i++, ptr++) {
        *ptr = 0;
    }
}

void Effect_Initialize(Effect* this) {
    s32 i;
    u8* ptr = (u8*) this;

    for (i = 0; i < sizeof(Effect); i++, ptr++) {
        *ptr = 0;
    }
    this->scale2 = 1.0f;
}

void Scenery_Load(Scenery* this, ObjectInit* objInit) {
    Scenery_Initialize(this);
    this->obj.status = OBJ_INIT;
    this->obj.pos.z = -objInit->zPos1;
    this->obj.pos.z += -3000.0f + objInit->zPos2;
    this->obj.pos.x = objInit->xPos;
    this->obj.pos.y = objInit->yPos;
    this->obj.rot.x = objInit->rot.x;
    this->obj.rot.y = objInit->rot.y;
    this->obj.rot.z = objInit->rot.z;
    this->obj.id = objInit->id;
    Object_SetInfo(&this->info, this->obj.id);
}

void Sprite_Load(Sprite* this, ObjectInit* objInit) {
    Sprite_Initialize(this);
    this->obj.status = OBJ_INIT;
    this->obj.pos.z = -objInit->zPos1;
    this->obj.pos.z += -3000.0f + objInit->zPos2;
    this->obj.pos.x = objInit->xPos;
    this->obj.pos.y = objInit->yPos;
    this->obj.rot.y = objInit->rot.y;
    this->obj.rot.x = objInit->rot.x;
    this->obj.rot.z = objInit->rot.z;
    this->obj.id = objInit->id;
    Object_SetInfo(&this->info, this->obj.id);
}

void Actor_Load(Actor* this, ObjectInit* objInit) {
    Actor_Initialize(this);
    this->obj.status = OBJ_INIT;
    this->obj.pos.z = -objInit->zPos1;
    this->obj.pos.z += -3000.0f + objInit->zPos2;
    this->obj.pos.x = objInit->xPos;
    this->obj.pos.y = objInit->yPos;
    this->obj.rot.y = objInit->rot.y;
    this->obj.rot.x = objInit->rot.x;
    this->obj.rot.z = objInit->rot.z;
    this->obj.id = objInit->id;
    Object_SetInfo(&this->info, this->obj.id);
}

void Boss_Load(Boss* this, ObjectInit* objInit) {
    Boss_Initialize(this);
    this->obj.status = OBJ_INIT;
    this->obj.pos.z = -objInit->zPos1;
    this->obj.pos.z += -3000.0f + objInit->zPos2;
    this->obj.pos.x = objInit->xPos;
    this->obj.pos.y = objInit->yPos;
    this->obj.rot.y = objInit->rot.y;
    this->obj.rot.x = objInit->rot.x;
    this->obj.rot.z = objInit->rot.z;
    this->obj.id = objInit->id;
    Object_SetInfo(&this->info, this->obj.id);
}

void Item_Load(Item* this, ObjectInit* objInit) {
    Item_Initialize(this);
    this->obj.status = OBJ_INIT;
    this->obj.pos.z = -objInit->zPos1;
    this->obj.pos.z += -3000.0f + objInit->zPos2;
    this->obj.pos.x = objInit->xPos;
    this->obj.pos.y = objInit->yPos;
    this->obj.rot.y = objInit->rot.y;
    this->obj.rot.x = objInit->rot.x;
    this->obj.rot.z = objInit->rot.z;
    this->obj.id = objInit->id;
    this->width = 1.0f;
    CALL_CANCELLABLE_EVENT(ItemDropEvent, this) {
        Object_SetInfo(&this->info, this->obj.id);
    }
}

void Effect_Effect346_Setup(Effect346* this, f32 xPos, f32 yPos, f32 zPos) {
    Effect_Initialize(this);
    this->obj.status = OBJ_INIT;
    this->obj.id = OBJ_EFFECT_346;
    this->timer_50 = 50;
    this->scale2 = 0.2f;

    if (gCurrentLevel == LEVEL_AQUAS) {
        this->timer_50 = 200;
        this->scale2 = 0.3f;
        this->scale1 = RAND_FLOAT(255.0f);
    }

    this->obj.pos.x = xPos;
    this->obj.pos.y = yPos;
    this->obj.pos.z = zPos;
    this->obj.rot.z = RAND_FLOAT(360.0f);

    Object_SetInfo(&this->info, this->obj.id);

    if (gLevelType == LEVELTYPE_PLANET) {
        this->info.cullDistance = 100.0f;
    }
}

void func_enmy_80061A4C(void) {
    s32 i;
    f32 x;
    f32 y;
    f32 z;

    for (i = 0; i < ARRAY_COUNT(gEffects); i++) {
        if (gEffects[i].obj.status == OBJ_FREE) {
            x = gPlayer[0].pos.x + RAND_FLOAT_CENTERED(400.0f) + (5.0f * gPlayer[0].vel.x);
            y = gPlayer[0].pos.y + RAND_FLOAT_CENTERED(400.0f) + (5.0f * gPlayer[0].vel.y);
            z = -gPathProgress - 500.0f;
            if (gPathVelZ < 0.0f) {
                z = -gPathProgress + 500.0f;
            }
            Effect_Effect346_Setup(&gEffects[i], x, y, z);
            break;
        }
    }
}

// For Aquas
void func_enmy_80061B68(void) {
    s32 i;
    f32 x;
    f32 y;
    f32 z;

    for (i = 0; i < ARRAY_COUNT(gEffects); i++) {
        if (gEffects[i].obj.status == OBJ_FREE) {
            x = gPlayer[0].pos.x + RAND_FLOAT_CENTERED(2000.0f) + (5.0f * gPlayer[0].vel.x);
            y = 0;
            while (y <= gGroundHeight) {
                y = gPlayer[0].pos.y + RAND_FLOAT_CENTERED(2000.0f) + (5.0f * gPlayer[0].vel.y);
            }
            z = -gPathProgress - 3000.0f;
            if (gPathVelZ < 0.0f) {
                z = -gPathProgress + 1000.0f;
            }
            Effect_Effect346_Setup(&gEffects[i], x, y, z);
            break;
        }
    }
}

// Unused
void func_enmy_80061CD0(void) {
    s32 i;
    f32 x;
    f32 y;
    f32 z;

    for (i = 0; i < ARRAY_COUNT(gEffects); i++) {
        if (gEffects[i].obj.status == OBJ_FREE) {
            x = gPlayer[gPlayerNum].pos.x + RAND_FLOAT_CENTERED(3000.0f) + (5.0f * gPlayer[gPlayerNum].vel.x);
            y = gPlayer[gPlayerNum].pos.y + 1000.0f + RAND_FLOAT_CENTERED(500.0f) + (5.0f * gPlayer[gPlayerNum].vel.y);
            z = -gPathProgress - RAND_FLOAT(2000.0f);
            if (gPathVelZ < 0.0f) {
                z = -gPathProgress + 1000.0f;
            }
            Effect_Effect346_Setup(&gEffects[i], x, y, z);
            break;
        }
    }
}

// Unused
void func_enmy_80061E48(Actor* this, f32 xPos, f32 yPos, f32 zPos) {
    Actor_Initialize(this);
    this->obj.status = OBJ_INIT;

    this->obj.id = OBJ_ACTOR_ME_METEOR_1;
    if (Rand_ZeroOne() < 0.5f) {
        this->obj.id = OBJ_ACTOR_ME_LASER_CANNON_1;
    }
    if (Rand_ZeroOne() < 0.5f) {
        this->obj.id = OBJ_ACTOR_ME_METEOR_2;
    }

    this->obj.pos.x = xPos;
    this->obj.pos.y = yPos;
    this->obj.pos.z = zPos;
    this->vel.z = 10.0f;
    Object_SetInfo(&this->info, this->obj.id);
}

void ActorEvent_Load(ActorEvent* this, ObjectInit* objInit, s32 index) {
    Vec3f src;

    Actor_Initialize(this);
    this->obj.status = OBJ_ACTIVE;
    this->index = index;
    this->obj.pos.z = -objInit->zPos1;
    this->obj.pos.z += -3000.0f + objInit->zPos2;
    this->obj.pos.x = objInit->xPos;
    this->obj.pos.y = objInit->yPos;
    this->obj.rot.y = this->rot_0F4.y = objInit->rot.y;
    this->obj.rot.x = this->rot_0F4.x = objInit->rot.x;
    this->rot_0F4.z = objInit->rot.z;
    this->obj.id = OBJ_ACTOR_EVENT;
    this->timer_0C2 = 10;
    this->eventType = EVID_FFF;
    this->aiType = objInit->id - ACTOR_EVENT_ID;

    Object_SetInfo(&this->info, this->obj.id);

    this->info.cullDistance = 3000.0f;
    this->fwork[25] = 20000.0f;
    this->iwork[1] = gPrevEventActorIndex;
    this->iwork[10] = gActors[gPrevEventActorIndex].aiType;
    this->fwork[22] = gArwingSpeed;

    Matrix_RotateZ(gCalcMatrix, -gFormationInitRot.z * M_DTOR, MTXF_NEW);
    Matrix_RotateX(gCalcMatrix, -gFormationInitRot.x * M_DTOR, MTXF_APPLY);
    Matrix_RotateY(gCalcMatrix, -gFormationInitRot.y * M_DTOR, MTXF_APPLY);

    src.x = this->obj.pos.x - gFormationInitPos.x;
    src.y = this->obj.pos.y - gFormationInitPos.y;
    src.z = this->obj.pos.z - gFormationInitPos.z;

    Matrix_MultVec3fNoTranslate(gCalcMatrix, &src, &this->vwork[28]);
    this->iwork[9] = gFormationLeaderIndex;
    gPrevEventActorIndex = index;
    Actor_Update(this);
}

void Object_Load(ObjectInit* objInit, f32 xMax, f32 xMin, f32 yMax, f32 yMin) {
    s32 i;

    if ((xMax > objInit->xPos - gPlayer[0].xPath) && (objInit->xPos - gPlayer[0].xPath > xMin) &&
        (yMax > objInit->yPos - gPlayer[0].yPath) && (objInit->yPos - gPlayer[0].yPath > yMin)) {
        if (objInit->id < OBJ_SCENERY_MAX) {
            for (i = 0; i < ARRAY_COUNT(gScenery); i++) {
                if (gScenery[i].obj.status == OBJ_FREE) {
                    Scenery_Load(&gScenery[i], objInit);
                    break;
                }
            }
        }
        if ((objInit->id >= OBJ_SPRITE_START) && (objInit->id < OBJ_SPRITE_MAX)) {
            for (i = 0; i < ARRAY_COUNT(gSprites); i++) {
                if (gSprites[i].obj.status == OBJ_FREE) {
                    Sprite_Load(&gSprites[i], objInit);
                    break;
                }
            }
        }
        if ((objInit->id >= OBJ_ACTOR_START) && (objInit->id < OBJ_ACTOR_MAX)) {
            if ((objInit->id == OBJ_ACTOR_AQ_JELLYFISH) || (objInit->id == OBJ_ACTOR_ZO_SEARCHLIGHT)) {
                for (i = ARRAY_COUNT(gActors) - 1; i >= 0; i--) {
                    if (gActors[i].obj.status == OBJ_FREE) {
                        Actor_Load(&gActors[i], objInit);
                        break;
                    }
                }
            } else if (objInit->id == OBJ_ACTOR_TEAM_BOSS) {
                for (i = 0; i < 3; i++) {
                    if (gActors[i].obj.status == OBJ_FREE) {
                        Actor_Load(&gActors[i], objInit);
                        break;
                    }
                }
            } else {
                for (i = 4; i < ARRAY_COUNT(gActors); i++) {
                    if (gActors[i].obj.status == OBJ_FREE) {
                        Actor_Load(&gActors[i], objInit);
                        break;
                    }
                }
            }
        }
        if ((objInit->id >= OBJ_BOSS_START) && (objInit->id < OBJ_BOSS_MAX)) {
            for (i = 0; i < ARRAY_COUNT(gBosses); i++) {
                if (gBosses[i].obj.status == OBJ_FREE) {
                    Boss_Load(&gBosses[i], objInit);
                    break;
                }
            }
        }
        if ((objInit->id >= OBJ_ITEM_START) && (objInit->id < OBJ_ITEM_MAX)) {
            for (i = 0; i < ARRAY_COUNT(gItems); i++) {
                if (gItems[i].obj.status == OBJ_FREE) {
                    Item_Load(&gItems[i], objInit);
                    break;
                }
            }
        }
        if ((objInit->id >= OBJ_EFFECT_START) && (objInit->id <= OBJ_ID_MAX)) {
            switch (objInit->id) {
                case OBJ_UNK_403:
                    D_MA_801BA1E8 = 99;
                    break;
                case OBJ_UNK_404:
                    D_MA_801BA1E8 = 0;
                    break;
                case OBJ_UNK_405:
                    D_MA_801BA1E8 = 98;
                    break;
                case OBJ_UNK_402:
                    D_Andross_801A7F78 = objInit->rot.z * 0.1f;
                    D_Andross_801A7F60 = -(f32) objInit->rot.x;
                    break;
                case OBJ_ENV_SMALL_ROCKS_ENABLE:
                    gDrawSmallRocks++;
                    break;
                case OBJ_ENV_SMALL_ROCKS_DISABLE:
                    if (gDrawSmallRocks > 0) {
                        gDrawSmallRocks--;
                    }
                    break;
            }
        }
        if (objInit->id > OBJ_ID_MAX) {
            for (i = 0; i < ARRAY_COUNT(gActors); i++) {
                if (gActors[i].obj.status == OBJ_FREE) {
                    ActorEvent_Load(&gActors[i], objInit, i);
                    break;
                }
            }
        }
    }
}

void func_enmy_80062568(void) {
    ObjectInit* objInit;
    s32 i;
    s32 temp = gCurrentLevel; // seems fake

    if (1) {}
    gLevelObjects = SEGMENTED_TO_VIRTUAL(gLevelObjectInits[temp]);
    i = gSavedObjectLoadIndex - 40;
    objInit = &gLevelObjects[i];

    for (; i < gSavedObjectLoadIndex; i++, objInit++) {
        // --- Call insertion in func_enmy_80062568 ---
        MaybeRandomizeObjectInit(objInit); // <-- insert here
        Object_Load(objInit, 4000.0f, -4000.0f, 4000.0f, -4000.0f);
    }
}

void Object_LoadLevelObjects(void) {
    ObjectInit* objInit;
    f32 xMax;
    f32 xMin;
    f32 yMax;
    f32 yMin;
    s32 i;
    s32 j;

    if ((gCurrentLevel == LEVEL_METEO) && (gLevelPhase == 1)) {
        gLevelObjects = SEGMENTED_TO_VIRTUAL(D_ME_602B148);
    } else if ((gCurrentLevel == LEVEL_SECTOR_X) && (gLevelPhase == 1)) {
        gLevelObjects = SEGMENTED_TO_VIRTUAL(D_SX_602F18C);
    } else if ((gCurrentLevel == LEVEL_VENOM_ANDROSS) && (gLevelPhase == 1)) {
        gLevelObjects = SEGMENTED_TO_VIRTUAL(D_ANDROSS_C0356A4);
    } else if ((gCurrentLevel == LEVEL_VENOM_1) && (gLevelPhase == 1)) {
        gLevelObjects = SEGMENTED_TO_VIRTUAL(D_VE1_6010088);
    } else {
        gLevelObjects = SEGMENTED_TO_VIRTUAL(gLevelObjectInits[gCurrentLevel]);
    }
    if (gGroundClipMode == 0) {
        for (j = 0; j < gDrawSmallRocks; j++) {
            if (gCurrentLevel == LEVEL_AQUAS) {
                func_enmy_80061B68();
            } else {
                func_enmy_80061A4C();
            }
        }
    }

    if (gCurrentLevel == LEVEL_METEO) {
        yMax = xMax = 10000.0f;
        yMin = xMin = -10000.0f;
    } else if (gCurrentLevel == LEVEL_SECTOR_Y) {
        yMax = xMax = 6000.0f;
        yMin = xMin = -6000.0f;
    } else if (gCurrentLevel == LEVEL_VENOM_1) {
        yMax = xMax = 3500.0f;
        yMin = xMin = -3500.0f;
    } else {
        yMax = xMax = 4000.0f;
        yMin = xMin = -4000.0f;
    }

    if ((gPlayer[0].pathChangeTimer != 0) && (gPlayer[0].pathChangeYaw < 0.0f)) {
        xMax = 10000.0f;
    }
    if ((gPlayer[0].pathChangeTimer != 0) && (gPlayer[0].pathChangeYaw > 0.0f)) {
        xMin = -10000.0f;
    }

    gLastPathChange = 0;

// Level Boss Tester
#if 0
    switch (gCurrentLevel) {
    case LEVEL_AQUAS:
        static ObjectInit bossaqInit[] = { { 100.6f, -4035, 0, 0, { 0, 0, 0 }, OBJ_BOSS_AQ_BACOON } };
        gLevelObjects = bossaqInit;
        break;

    case LEVEL_TITANIA:
        static ObjectInit aTiLevelObjects_Boss[] = { { 300.0f, -1000, 0, 0, { 0, 0, 0 }, OBJ_BOSS_TI_GORAS } };
        gLevelObjects = aTiLevelObjects_Boss;
        break;

    case LEVEL_SECTOR_X:
        static ObjectInit aSxLevelObjects_Boss[] = { { 100.0f, 4000, 0, 1103, { 0, 0, 0 }, OBJ_BOSS_SX_SPYBORG } };
        gLevelObjects = aSxLevelObjects_Boss;
        break;

    case LEVEL_SOLAR:
        static ObjectInit aSoLevelObjects_Boss[] = { { 100.0f, 0, 0, 0, { 0, 0, 0 }, OBJ_BOSS_SO_VULKAIN } };
        gLevelObjects = aSoLevelObjects_Boss;
        break;
    }
#endif

    for (i = 0, objInit = &gLevelObjects[gObjectLoadIndex]; i < 10000; i++, gObjectLoadIndex++, objInit++) {
        if (objInit->id <= OBJ_INVALID) {
            break;
        }
        if ((gPathProgress <= objInit->zPos1) && (objInit->zPos1 <= gPathProgress + 200.0f)) {
            if ((gCurrentLevel == LEVEL_VENOM_1) && (objInit->id >= ACTOR_EVENT_ID)) {
                if (((objInit->rot.y < 180.0f) && (objInit->xPos < gPlayer[0].xPath)) ||
                    ((objInit->rot.y > 180.0f) && (gPlayer[0].xPath < objInit->xPos))) {
                    MaybeRandomizeObjectInit(objInit); // <-- insert here
                    Object_Load(objInit, xMax, xMin, yMax, yMin);
                }
            } else {
                MaybeRandomizeObjectInit(objInit); // <-- and here
                Object_Load(objInit, xMax, xMin, yMax, yMin);
            }
        } else {
            break;
        }
    }
}

void func_enmy_80062B60(f32 xPos, f32 zPos, s32 state, f32 scale) {
    s32 i;

    if (gLevelType == LEVELTYPE_PLANET) {
        for (i = 0; i < ARRAY_COUNT(gEffects); i++) {
            if (gEffects[i].obj.status == OBJ_FREE) {
                Effect_Initialize(&gEffects[i]);
                gEffects[i].obj.status = OBJ_INIT;
                gEffects[i].obj.id = OBJ_EFFECT_348;
                gEffects[i].obj.pos.x = xPos;
                gEffects[i].obj.pos.y = gGroundHeight + 3.0f;
                gEffects[i].obj.pos.z = zPos;
                gEffects[i].scale2 = 10.0f;
                gEffects[i].scale1 = scale;
                gEffects[i].unk_44 = 80;
                gEffects[i].state = state;
                Object_SetInfo(&gEffects[i].info, gEffects[i].obj.id);
                break;
            }
        }
    }
}

void func_enmy_80062C38(f32 xPos, f32 yPos) {
    s32 i;

    if (gLevelType == LEVELTYPE_PLANET) {
        for (i = 0; i < ARRAY_COUNT(gEffects); i++) {
            if (gEffects[i].obj.status == OBJ_FREE) {
                Effect_Initialize(&gEffects[i]);
                gEffects[i].obj.status = OBJ_INIT;
                gEffects[i].obj.id = OBJ_EFFECT_349;
                gEffects[i].obj.pos.x = xPos;
                gEffects[i].obj.pos.y = gGroundHeight + 3.0f;
                gEffects[i].obj.pos.z = yPos;
                gEffects[i].scale2 = 1.0f;
                gEffects[i].scale1 = 1.3f;
                gEffects[i].unk_44 = 120;
                Object_SetInfo(&gEffects[i].info, gEffects[i].obj.id);
                break;
            }
        }
    }
}

void func_enmy_80062D04(f32 xPos, f32 yPos) {
    s32 i;

    for (i = 0; i < ARRAY_COUNT(gEffects); i++) {
        if (gEffects[i].obj.status == OBJ_FREE) {
            Effect_Initialize(&gEffects[i]);
            gEffects[i].obj.status = OBJ_INIT;
            gEffects[i].obj.id = OBJ_EFFECT_350;
            gEffects[i].obj.pos.x = xPos;
            gEffects[i].obj.pos.y = gGroundHeight + 3.0f;
            gEffects[i].obj.pos.z = yPos;
            gEffects[i].scale2 = 3.0f;
            gEffects[i].scale1 = 2.0f;
            gEffects[i].unk_44 = 120;
            Object_SetInfo(&gEffects[i].info, gEffects[i].obj.id);
            break;
        }
    }
}

bool Object_CheckHitboxCollision(Vec3f* pos, f32* hitboxData, Object* obj, f32 xRot, f32 yRot, f32 zRot) {
    s32 i;
    Vec3f hitRot;
    Vec3f hitPos;
    f32 rotate;
    s32 count;
    Vec3f sp80;
    Vec3f sp74;
    Hitbox* hitbox;

    count = *hitboxData;
    if (count != 0) {
        hitboxData++;

        for (i = 0; i < count; i++, hitboxData += 6) {
            rotate = 0.0f;
            hitRot.x = hitRot.y = hitRot.z = 0.0f;
            if (*hitboxData >= HITBOX_SHADOW) {
                return false;
            }
            if (*hitboxData == HITBOX_ROTATED) {
                rotate = 1.0f;
                hitRot.x = hitboxData[1];
                hitRot.y = hitboxData[2];
                hitRot.z = hitboxData[3];
                hitboxData += 4;
            }

            if ((obj->rot.y == 0.0f) && (obj->rot.z == 0.0f) && (obj->rot.x == 0.0f) && (rotate == 0.0f)) {
                hitPos.x = pos->x;
                hitPos.y = pos->y;
                hitPos.z = pos->z;
            } else {
                Matrix_RotateZ(gCalcMatrix, -hitRot.z * M_DTOR, MTXF_NEW);
                Matrix_RotateX(gCalcMatrix, -hitRot.x * M_DTOR, MTXF_APPLY);
                Matrix_RotateY(gCalcMatrix, -hitRot.y * M_DTOR, MTXF_APPLY);

                Matrix_RotateZ(gCalcMatrix, -obj->rot.z * M_DTOR, MTXF_APPLY);
                Matrix_RotateX(gCalcMatrix, -obj->rot.x * M_DTOR, MTXF_APPLY);
                Matrix_RotateY(gCalcMatrix, -obj->rot.y * M_DTOR, MTXF_APPLY);

                if ((xRot != 0.0f) || (yRot != 0.0f) || (zRot != 0.0f)) {
                    Matrix_RotateZ(gCalcMatrix, -zRot * M_DTOR, MTXF_APPLY);
                    Matrix_RotateX(gCalcMatrix, -xRot * M_DTOR, MTXF_APPLY);
                    Matrix_RotateY(gCalcMatrix, -yRot * M_DTOR, MTXF_APPLY);
                }

                sp80.x = pos->x - obj->pos.x;
                sp80.y = pos->y - obj->pos.y;
                sp80.z = pos->z - obj->pos.z;

                Matrix_MultVec3fNoTranslate(gCalcMatrix, &sp80, &sp74);

                hitPos.x = obj->pos.x + sp74.x;
                hitPos.y = obj->pos.y + sp74.y;
                hitPos.z = obj->pos.z + sp74.z;
            }

            hitbox = (Hitbox*) hitboxData;
            if (((hitbox->z.size + 20.0f) > fabsf(hitbox->z.offset + obj->pos.z - hitPos.z)) &&
                ((hitbox->x.size + 20.0f) > fabsf(hitbox->x.offset + obj->pos.x - hitPos.x)) &&
                ((hitbox->y.size + 10.0f) > fabsf(hitbox->y.offset + obj->pos.y - hitPos.y))) {
                return true;
            }
        }
    }
    return false;
}

bool Object_CheckSingleHitbox(Vec3f* checkPos, f32* hitboxData, Vec3f* hitboxPos) {
    if ((s32) hitboxData[0] != 0) {
        if ((fabsf(hitboxData[1] + hitboxPos->z - checkPos->z) < (hitboxData[2] + 20.0f)) &&
            (fabsf(hitboxData[5] + hitboxPos->x - checkPos->x) < (hitboxData[6] + 20.0f)) &&
            (fabsf(hitboxData[3] + hitboxPos->y - checkPos->y) < (hitboxData[4] + 20.0f))) {
            return true;
        }
    }
    return false;
}

bool Object_CheckPolyCollision(Vec3f* pos, Vec3f* vel, ObjectId objId, Object* obj) {
    Vec3f sp74;
    Vec3f sp68;
    Vec3f relPos;
    Vec3f objPos;
    Vec3f sp44;
    s32 colId;
    s32 pad1[2];
    f32 sp30[2];
    s32 pad2;

    sp74.x = pos->x - obj->pos.x;
    sp74.z = pos->z - obj->pos.z;

    if (((fabsf(sp74.x) < 1100.0f) && (fabsf(sp74.z) < 1100.0f)) || (objId == OBJ_ACTOR_ME_MOLAR_ROCK)) {
        sp74.y = pos->y - obj->pos.y;

        Matrix_RotateY(gCalcMatrix, -obj->rot.y * M_DTOR, MTXF_NEW);
        Matrix_MultVec3fNoTranslate(gCalcMatrix, &sp74, &sp68);

        relPos.x = obj->pos.x + sp68.x;
        relPos.y = obj->pos.y + sp68.y;
        relPos.z = obj->pos.z + sp68.z;

        objPos.x = obj->pos.x;
        objPos.y = obj->pos.y;
        objPos.z = obj->pos.z;

        if ((objId == OBJ_ACTOR_ME_MOLAR_ROCK) || (objId == OBJ_SCENERY_FO_MOUNTAIN_2) ||
            (objId == OBJ_SCENERY_FO_MOUNTAIN_3) || (objId == OBJ_BOSS_FO_BASE) || (objId == OBJ_BOSS_SZ_GREAT_FOX) ||
            (objId == OBJ_BOSS_VE2_BASE) || (objId == OBJ_BOSS_BO_BASE) || (objId == OBJ_SCENERY_ME_TUNNEL)) {
            colId = COL1_0;
            if (objId == OBJ_BOSS_VE2_BASE) {
                colId = COL1_9;
            }
            if (objId == OBJ_SCENERY_ME_TUNNEL) {
                colId = COL1_1;
            } else if (objId == OBJ_BOSS_FO_BASE) {
                colId = COL1_4;
            } else if (objId == OBJ_BOSS_BO_BASE) {
                colId = COL1_7;
            } else if (objId == OBJ_SCENERY_FO_MOUNTAIN_2) {
                colId = COL1_5;
            } else if (objId == OBJ_SCENERY_FO_MOUNTAIN_3) {
                colId = COL1_6;
            } else if (objId == OBJ_BOSS_SZ_GREAT_FOX) {
                colId = COL1_8;
            }
            if (func_col1_800998FC(&relPos, &objPos, vel, colId, &sp44, sp30) > 0) {
                return true;
            }
        } else {
            colId = COL2_0;
            if (objId == OBJ_SCENERY_CO_BUMP_2) {
                colId = COL2_2;
            }
            if (objId == OBJ_SCENERY_CO_BUMP_3) {
                colId = COL2_3;
            }
            if (objId == OBJ_SCENERY_VS_PYRAMID_1) {
                colId = COL2_4;
            }
            if (objId == OBJ_SCENERY_VS_PYRAMID_2) {
                colId = COL2_6;
            }
            if (objId == OBJ_SCENERY_AQ_CORAL_REEF_1) {
                colId = COL2_14;
            } else if ((objId == OBJ_SCENERY_CO_BUMP_4) || (objId == OBJ_SCENERY_CO_BUMP_5)) {
                colId = COL2_1;
            }

            if (func_col2_800A3690(&relPos, &objPos, colId, &sp44)) {
                return true;
            }
        }
    }
    return false;
}

s32 Object_CheckCollision(s32 index, Vec3f* pos, Vec3f* vel, s32 mode) {
    Scenery360* scenery360;
    Scenery* scenery;
    Sprite* sprite;
    Boss* boss;
    Actor* actor;
    Vec3f temp;
    s32 i;

    if ((gLevelMode == LEVELMODE_ALL_RANGE) && (gCurrentLevel != LEVEL_KATINA)) {
        scenery360 = gScenery360;
        for (i = 0; i < 200; i++, scenery360++) {
            if (scenery360->obj.status == OBJ_ACTIVE) {
                if ((scenery360->obj.id == OBJ_SCENERY_CO_BUMP_1) || (scenery360->obj.id == OBJ_SCENERY_CO_BUMP_3) ||
                    (scenery360->obj.id == OBJ_SCENERY_AQ_CORAL_REEF_1) ||
                    (scenery360->obj.id == OBJ_SCENERY_VS_PYRAMID_2) ||
                    (scenery360->obj.id == OBJ_SCENERY_FO_MOUNTAIN_3) ||
                    (scenery360->obj.id == OBJ_SCENERY_FO_MOUNTAIN_2) ||
                    (scenery360->obj.id == OBJ_SCENERY_FO_MOUNTAIN_1) ||
                    (scenery360->obj.id == OBJ_SCENERY_VS_PYRAMID_1)) {
                    if (Object_CheckPolyCollision(pos, vel, scenery360->obj.id, &scenery360->obj)) {
                        return 999;
                    }
                } else if ((fabsf(pos->x - scenery360->obj.pos.x) < 2000.0f) &&
                           (fabsf(pos->z - scenery360->obj.pos.z) < 2000.0f)) {
                    if (Object_CheckHitboxCollision(pos, scenery360->info.hitbox, &scenery360->obj, 0.0f, 0.0f, 0.0f)) {
                        return 2;
                    }
                }
            }
        }
    }

    scenery = &gScenery[0];
    for (i = 0; (i < ARRAY_COUNT(gScenery)) && (gLevelMode == LEVELMODE_ON_RAILS); i++, scenery++) {
        if (scenery->obj.status == OBJ_ACTIVE) {
            if ((scenery->obj.id == OBJ_SCENERY_CO_BUMP_1) || (scenery->obj.id == OBJ_SCENERY_CO_BUMP_4) ||
                (scenery->obj.id == OBJ_SCENERY_CO_BUMP_5) || (scenery->obj.id == OBJ_SCENERY_CO_BUMP_2) ||
                (scenery->obj.id == OBJ_SCENERY_ME_TUNNEL) || (scenery->obj.id == OBJ_SCENERY_CO_BUMP_3)) {
                if (Object_CheckPolyCollision(pos, vel, scenery->obj.id, &scenery->obj)) {
                    return 2;
                }
            } else if ((fabsf(pos->x - scenery->obj.pos.x) < 2000.0f) &&
                       (fabsf(pos->z - scenery->obj.pos.z) < 2000.0f)) {
                if (Object_CheckHitboxCollision(pos, scenery->info.hitbox, &scenery->obj, 0.0f, 0.0f, 0.0f)) {
                    return i + 10;
                }
            }
        }
    }

    sprite = &gSprites[0];
    for (i = 0; i < ARRAY_COUNT(gSprites); i++, sprite++) {
        if ((sprite->obj.status == OBJ_ACTIVE) && (fabsf(pos->x - sprite->obj.pos.x) < 500.0f) &&
            (fabsf(pos->z - sprite->obj.pos.z) < 500.0f) &&
            Object_CheckSingleHitbox(pos, sprite->info.hitbox, &sprite->obj.pos)) {
            if ((sprite->obj.id == OBJ_SPRITE_FO_POLE) || (sprite->obj.id == OBJ_SPRITE_CO_TREE) ||
                (sprite->obj.id == OBJ_SPRITE_CO_TREE)) {
                sprite->destroy = true;
            }
            return 0;
        }
    }

    if ((mode == 0) || (mode == 2) || (mode == 3)) {
        if (mode != 2) {
            boss = &gBosses[0];
            for (i = 0; i < ARRAY_COUNT(gBosses); i++, boss++) {
                if (boss->obj.status == OBJ_ACTIVE) {
                    if ((boss->obj.id == OBJ_BOSS_FO_BASE) || (boss->obj.id == OBJ_BOSS_VE2_BASE) ||
                        (boss->obj.id == OBJ_BOSS_SZ_GREAT_FOX) || (boss->obj.id == OBJ_BOSS_BO_BASE)) {
                        if (Object_CheckPolyCollision(pos, vel, boss->obj.id, &boss->obj)) {
                            return 2;
                        }
                    } else if (boss->obj.id == OBJ_BOSS_BO_BASE_SHIELD) {
                        temp.x = fabsf(boss->obj.pos.x - pos->x) * (5.0f / 6.0f);
                        temp.y = fabsf(boss->obj.pos.y - pos->y) * 2;
                        temp.z = fabsf(boss->obj.pos.z - pos->z) * (5.0f / 6.0f);
                        if ((VEC3F_MAG(&temp)) < 1500.0f) {
                            boss->dmgType = DMG_BEAM;
                            return 2;
                        }
                    } else {
                        if (boss->obj.id == OBJ_BOSS_KA_SAUCERER) {
                            temp.x = fabsf(boss->obj.pos.x - pos->x);
                            temp.y = fabsf(boss->obj.pos.y - 300.0f - pos->y) * 7.42f;
                            temp.z = fabsf(boss->obj.pos.z - pos->z);
                            if ((VEC3F_MAG(&temp)) < 2700.0f) {
                                return 2;
                            }
                        }
                        if ((fabsf(pos->x - boss->obj.pos.x) < 2000.0f) &&
                            (fabsf(pos->z - boss->obj.pos.z) < 2000.0f)) {
                            if (Object_CheckHitboxCollision(pos, boss->info.hitbox, &boss->obj, 0.0f, 0.0f, 0.0f)) {
                                return 2;
                            }
                        }
                    }
                }
            }
        }

        actor = &gActors[0];
        for (i = 0; i < ARRAY_COUNT(gActors); i++, actor++) {
            if ((actor->obj.status >= OBJ_ACTIVE) && (fabsf(pos->x - actor->obj.pos.x) < 1000.0f) &&
                (fabsf(pos->z - actor->obj.pos.z) < 1500.0f) && (index != i) && (actor->info.unk_16 != 2) &&
                !((actor->obj.id == OBJ_ACTOR_ALLRANGE) && (actor->aiType <= AI360_PEPPY)) && (actor->timer_0C2 == 0)) {
                if (actor->obj.id == OBJ_ACTOR_ME_MOLAR_ROCK) {
                    if (Object_CheckPolyCollision(pos, vel, actor->obj.id, &actor->obj)) {
                        return 2;
                    }
                } else if (actor->scale < 0.0f) {
                    if (Object_CheckHitboxCollision(pos, actor->info.hitbox, &actor->obj, actor->vwork[29].x,
                                                    actor->vwork[29].y, actor->vwork[29].z + actor->rot_0F4.z)) {
                        actor->dmgType = DMG_BEAM;
                        actor->damage = 10;
                        actor->dmgPart = -1;
                        actor->hitPos.x = pos->x;
                        actor->hitPos.y = pos->y;
                        actor->hitPos.z = pos->z;
                        return 2;
                    }
                } else if ((mode != 2) && (mode != 3)) {
                    if (Object_CheckSingleHitbox(pos, actor->info.hitbox, &actor->obj.pos)) {
                        actor->dmgType = DMG_BEAM;
                        actor->damage = 10;
                        actor->dmgPart = -1;
                        if (!(((actor->obj.id == OBJ_ACTOR_EVENT) && (actor->iwork[12] != 0)) ||
                              ((actor->obj.id == OBJ_ACTOR_ALLRANGE) &&
                               ((actor->aiType < AI360_ENEMY) || (actor->aiType >= AI360_GREAT_FOX))))) {
                            actor->health = 0;
                        }
                        actor->hitPos.x = pos->x;
                        actor->hitPos.y = pos->y;
                        actor->hitPos.z = pos->z;
                        return 1;
                    }
                }
            }
        }
    }
    return 0;
}

void Actor_CoRadar_Init(Scenery* this) {
    s32 i;

    for (i = 0; i < ARRAY_COUNT(gActors); i++) {
        if (gActors[i].obj.status == OBJ_FREE) {
            Actor_Initialize(&gActors[i]);
            gActors[i].obj.status = OBJ_INIT;
            gActors[i].obj.id = OBJ_ACTOR_CO_RADAR;
            gActors[i].obj.pos.x = this->obj.pos.x;
            gActors[i].obj.pos.y = this->obj.pos.y;
            gActors[i].obj.pos.z = this->obj.pos.z;
            gActors[i].obj.rot.y = RAND_FLOAT(360.0f);
            Object_SetInfo(&gActors[i].info, gActors[i].obj.id);
            break;
        }
    }
}

void Scenery_Corneria_Init(CoDoors* this) {
    s32 i;

    this->obj.pos.y = gGroundHeight;

    for (i = 0; i < ARRAY_COUNT(gSprites); i++) {
        if (gSprites[i].obj.status == OBJ_FREE) {
            Sprite_Initialize(&gSprites[i]);
            gSprites[i].obj.status = OBJ_INIT;
            gSprites[i].obj.id = OBJ_SPRITE_FOG_SHADOW;
            gSprites[i].sceneryId = this->obj.id;
            gSprites[i].obj.pos.x = this->obj.pos.x;
            gSprites[i].obj.pos.y = 5.0f;
            gSprites[i].obj.pos.z = this->obj.pos.z;

            if ((this->obj.id == OBJ_SCENERY_CO_STONE_ARCH) || (this->obj.id == OBJ_SCENERY_CO_HIGHWAY_1) ||
                (this->obj.id == OBJ_SCENERY_CO_HIGHWAY_2) || (this->obj.id == OBJ_SCENERY_CO_DOORS) ||
                (this->obj.id == OBJ_SCENERY_CO_ARCH_1) || (this->obj.id == OBJ_SCENERY_CO_ARCH_2) ||
                (this->obj.id == OBJ_SCENERY_CO_ARCH_3)) {
                gSprites[i].obj.rot.y = this->obj.rot.y;
            } else {
                gSprites[i].obj.rot.y = 44.9f;
            }

            Object_SetInfo(&gSprites[i].info, gSprites[i].obj.id);
            break;
        }
    }
}

void Scenery_CoStoneArch_Init(CoStoneArch* this, f32* hitboxData) {
    s32 i;
    Item* item;

    for (i = 0, item = gItems; i < ARRAY_COUNT(gItems); i++, item++) {
        if (item->obj.status == OBJ_FREE) {
            Item_Initialize(&gItems[i]);
            item->obj.status = OBJ_INIT;
            item->obj.id = OBJ_ITEM_RING_CHECK;
            item->obj.pos.x = this->obj.pos.x;
            item->obj.pos.y = this->obj.pos.y;
            item->obj.pos.z = this->obj.pos.z;
            item->obj.rot.y = this->obj.rot.y;
            item->info.hitbox = LOAD_ASSET(hitboxData);
            CALL_CANCELLABLE_EVENT(ItemDropEvent, item) {
                Object_SetInfo(&item->info, item->obj.id);
            }
            break;
        }
    }
}

// Unused
void func_enmy_80063F4C(s32 arg0) {
    /* Unimplemented */
}

void func_enmy_80063F58(Item* item) {
    item->width = item->obj.rot.z * 100.0f;
}

void func_enmy_80063F74(Item* item) {
    item->width = item->obj.rot.z * 100.0f;
}

// -----------------------------------------------------------------------------
// Helper: Convert an existing actor slot into an Event actor (in-place).
// This replicates the minimal initialization done by ActorEvent_Load but
// preserves the slot's current position/rotation so dynamic spawns keep their coordinates.
// -----------------------------------------------------------------------------
static void ConvertActorSlotToEvent(s32 index, s32 eventPick) {
    if ((index < 0) || (index >= ARRAY_COUNT(gActors))) {
        printf("RANDOMIZER :: ConvertActorSlotToEvent :: invalid index %d\n", index);
        return;
    }

    s32 evIndex = eventPick - ACTOR_EVENT_ID;
    if ((evIndex < 0) || (evIndex >= EVENT_ACTOR_COUNT)) {
        printf("RANDOMIZER :: ConvertActorSlotToEvent :: invalid event index %d (pick=%d)\n", evIndex, eventPick);
        return;
    }

    Actor* a = &gActors[index];

    // Keep current position/rotation; replicate essential ActorEvent_Load state
    a->obj.id = OBJ_ACTOR_EVENT;
    a->obj.status = OBJ_ACTIVE;
    a->index = index;

    // approx. ActorEvent_Load defaults
    a->timer_0C2 = 10;
    a->eventType = EVID_FFF; // default placeholder used by ActorEvent_Load
    a->aiType = evIndex;     // event actor index
    // preserve current rotation in rot_0F4 as ActorEvent_Load would set from ObjectInit
    a->rot_0F4.x = a->obj.rot.x;
    a->rot_0F4.y = a->obj.rot.y;
    a->rot_0F4.z = a->obj.rot.z;

    // Fill ObjectInfo for event actor id (safe)
    Object_SetInfo(&a->info, a->obj.id);
    a->info.cullDistance = 3000.0f;

    // Mimic ActorEvent_Load work/iwrok initial values used by formation/event code
    a->fwork[25] = 20000.0f;
    a->fwork[22] = gArwingSpeed;

    // remember previous/leader indices like ActorEvent_Load
    a->iwork[1] = gPrevEventActorIndex;
    if ((gPrevEventActorIndex >= 0) && (gPrevEventActorIndex < ARRAY_COUNT(gActors))) {
        a->iwork[10] = gActors[gPrevEventActorIndex].aiType;
    } else {
        a->iwork[10] = 0;
    }
    a->iwork[9] = gFormationLeaderIndex;

    // update global last event actor index as ActorEvent_Load does
    gPrevEventActorIndex = index;

    printf("RANDOMIZER :: ConvertActorSlotToEvent :: converted actor slot %d -> EVENT (evIndex=%d)\n", index, evIndex);

    // Run one update to let event-specific initialization run (mirrors ActorEvent_Load calling Actor_Update)
    Actor_Update(a);
}

void Object_Init(s32 index, ObjectId objId) {
    s32 i;
    s32 j;
    f32 xRot;
    f32 zRot;
    f32 sp4C;
    PosRot* var_v0;

    // Dynamic-spawn coverage: randomize inside Object_Init for actor cases
    if ((objId >= OBJ_ACTOR_START) && (objId < OBJ_ACTOR_MAX)) {
        s16 original = (s16) objId;
        s16 pick = PickRandomActorFromPoolDifferent(original, true);

        if (pick != original) {
            printf("[RANDOMIZER_DYNAMIC_SPAWN] At index %d\n", index);
            if (pick >= ACTOR_EVENT_ID) {
                ConvertActorSlotToEvent(index, pick);
                return;
            }

            if ((pick >= OBJ_ACTOR_START) && (pick < OBJ_ACTOR_MAX)) {
                objId = (ObjectId) pick;
                if ((index >= 0) && (index < ARRAY_COUNT(gActors))) {
                    gActors[index].obj.id = (u16) pick;
                }
            }
        }
    }

    switch (objId) {
        case OBJ_SPRITE_CO_SMOKE:
            Effect_SpawnTimedSfxAtPos(&gSprites[index].obj.pos, NA_SE_OB_SMOKE);
            break;
        case OBJ_ACTOR_TI_GREAT_FOX:
            AUDIO_PLAY_SFX(NA_SE_GREATFOX_ENGINE, gActors[index].sfxSource, 0);
            break;
        case OBJ_SCENERY_CO_WATERFALL:
            AUDIO_PLAY_SFX(NA_SE_FALL, gScenery[index].sfxSource, 0);
            break;
        case OBJ_ACTOR_TEAM_BOSS:
            ActorTeamBoss_Init(&gActors[index]);
            break;
        case OBJ_ACTOR_ZO_BIRD:
            gActors[index].fwork[10] = fabsf(Math_ModF(gActors[index].obj.pos.x, 100.0f));
            break;
        case OBJ_ACTOR_ZO_BARRIER:
            Zoness_ZoBarrier_Init(&gActors[index]);
            break;
        case OBJ_EFFECT_368:
            if (gCurrentLevel == LEVEL_TITANIA) {
                Ground_801B6E20(gEffects[index].obj.pos.x, gEffects[index].obj.pos.z + gPathProgress, &xRot, &sp4C,
                                &zRot);
                gEffects[index].obj.pos.y = sp4C + 3.0f;
                gEffects[index].obj.rot.x = RAD_TO_DEG(xRot);
                gEffects[index].obj.rot.z = RAD_TO_DEG(zRot);
            } else if (gCurrentLevel == LEVEL_MACBETH) {
                gEffects[index].obj.status = OBJ_FREE;
            }
            break;
        case OBJ_SCENERY_TI_RIB_0:
            Scenery_CoStoneArch_Init(&gScenery[index], D_TI_6006940);
            break;
        case OBJ_SCENERY_TI_RIB_1:
            Scenery_CoStoneArch_Init(&gScenery[index], D_TI_600695C);
            break;
        case OBJ_SCENERY_TI_RIB_2:
            Scenery_CoStoneArch_Init(&gScenery[index], D_TI_6006978);
            break;
        case OBJ_SCENERY_TI_RIB_3:
            Scenery_CoStoneArch_Init(&gScenery[index], D_TI_6006994);
            break;
        case OBJ_SCENERY_TI_RIB_4:
            Scenery_CoStoneArch_Init(&gScenery[index], D_TI_60069B0);
            break;
        case OBJ_SCENERY_TI_RIB_5:
            Scenery_CoStoneArch_Init(&gScenery[index], D_TI_60069CC);
            break;
        case OBJ_SCENERY_TI_RIB_6:
            Scenery_CoStoneArch_Init(&gScenery[index], D_TI_60069E8);
            break;
        case OBJ_SCENERY_TI_RIB_7:
            Scenery_CoStoneArch_Init(&gScenery[index], D_TI_6006A04);
            break;
        case OBJ_SCENERY_TI_RIB_8:
            Scenery_CoStoneArch_Init(&gScenery[index], D_TI_6006A20);
            break;
        case OBJ_SCENERY_CO_RADAR_DISH:
            Actor_CoRadar_Init(&gScenery[index]);
            break;
        case OBJ_ITEM_CHECKPOINT:
            if (gSavedObjectLoadIndex != 0) {
                gItems[index].obj.status = OBJ_FREE;
            }
            break;
        case OBJ_ITEM_METEO_WARP:
            if (gRingPassCount < 0) {
                gItems[index].obj.status = OBJ_FREE;
            }
            break;
        case OBJ_ITEM_PATH_SPLIT_Y:
        case OBJ_ITEM_PATH_TURN_UP:
        case OBJ_ITEM_PATH_TURN_DOWN:
            func_enmy_80063F74(&gItems[index]);
            break;
        case OBJ_ITEM_PATH_SPLIT_X:
        case OBJ_ITEM_PATH_TURN_LEFT:
            func_enmy_80063F58(&gItems[index]);
            break;
        case OBJ_ITEM_PATH_TURN_RIGHT:
            if (((gRingPassCount >= 7) && (gCurrentLevel == LEVEL_CORNERIA) && (gTeamShields[TEAM_ID_FALCO] > 0)) ||
                (gCurrentLevel != LEVEL_CORNERIA)) {
                func_enmy_80063F58(&gItems[index]);
            } else {
                gItems[index].obj.status = OBJ_FREE;
            }
            break;
        case OBJ_SCENERY_CO_STONE_ARCH:
            Scenery_CoStoneArch_Init(&gScenery[index], gItemRingCheckHitbox);
            /* fallthrough */
        case OBJ_SCENERY_CO_HIGHWAY_1:
        case OBJ_SCENERY_CO_HIGHWAY_2:
        case OBJ_SCENERY_CO_ARCH_1:
        case OBJ_SCENERY_CO_ARCH_2:
        case OBJ_SCENERY_CO_ARCH_3:
        case OBJ_SCENERY_CO_DOORS:
            Scenery_Corneria_Init(&gScenery[index]);
            break;
        case OBJ_ACTOR_ME_LASER_CANNON_2:
            gActors[index].fwork[0] = gActors[index].obj.pos.x;
            gActors[index].fwork[1] = gActors[index].obj.pos.y;
            gActors[index].obj.rot.z = gActors[index].obj.rot.x;
            gActors[index].obj.rot.x = 0.0f;
            break;
        case OBJ_ACTOR_ME_METEOR_2:
        case OBJ_ACTOR_ME_LASER_CANNON_1:
            gActors[index].work_046 = gFogRed;
            gActors[index].work_048 = gFogNear;
            gActors[index].obj.rot.x = RAND_FLOAT(360.0f);
            gActors[index].obj.rot.y = RAND_FLOAT(360.0f);
            break;
        case OBJ_ACTOR_ME_METEOR_1:
            gActors[index].obj.rot.x = RAND_FLOAT(360.0f);
            gActors[index].obj.rot.y = RAND_FLOAT(360.0f);
            gActors[index].health = 200;
            break;
        case OBJ_ACTOR_ME_HOPBOT:
            gActors[index].health = 30;
            break;
        case OBJ_ACTOR_ZO_RADARBUOY:
            if (gPlayer[0].pos.z < gActors[index].obj.pos.z) {
                Object_Kill(&gActors[index].obj, gActors[index].sfxSource);
            }
            break;
        case OBJ_ACTOR_ZO_DODORA_WP_COUNT:
            gActors[index].iwork[0] = gZoDodoraWaypointCount;
            gZoDodoraWaypointCount++;
            break;
        case OBJ_ACTOR_ZO_DODORA:
            gZoDodoraWaypointCount = 0;
            gActors[index].rot_0F4.x = gActors[index].obj.rot.x;
            gActors[index].rot_0F4.y = gActors[index].obj.rot.y;
            gActors[index].obj.rot.x = gActors[index].obj.rot.y = 0.0f;
            gActors[index].fwork[2] = gActors[index].obj.pos.y;
            var_v0 = gZoDodoraPosRots;
            for (i = 0; i < 200; i++, var_v0++) {
                var_v0->pos.x = gActors[index].obj.pos.x;
                var_v0->pos.y = gActors[index].obj.pos.y;
                var_v0->pos.z = gActors[index].obj.pos.z;
                var_v0->rot.x = gActors[index].obj.rot.x;
                var_v0->rot.y = gActors[index].obj.rot.y;
                var_v0->rot.z = gActors[index].obj.rot.z;
            }
            break;
        case OBJ_ACTOR_ME_MORA:
            gActors[index].work_046 = 100;
            for (i = 0; i < 2; i++) {
                if (gMeMoraStatus[i] == 0) {
                    gMeMoraStatus[i] = 1;
                    gActors[index].work_046 = i;
                    for (j = 0; j < 100; j++) {
                        gMeMoraYpos[i][j] = gActors[index].obj.pos.y;
                        gMeMoraZpos[i][j] = gActors[index].obj.pos.z;
                    }
                    break;
                }
            }
            if (gActors[index].work_046 == 100) {
                gActors[index].obj.status = OBJ_FREE;
            }
            break;
        case OBJ_ACTOR_MISSILE_SEEK_TEAM:
        case OBJ_ACTOR_MISSILE_SEEK_PLAYER:
            AUDIO_PLAY_SFX(NA_SE_EN_MISSILE_ENGINE, gActors[index].sfxSource, 4);
            break;
        case OBJ_ACTOR_CO_SKIBOT:
            gActors[index].drawShadow = true;
            break;
        case OBJ_BOSS_AND_ANDROSS:
            Andross_AndAndross_Init(&gBosses[index]);
            break;
        case OBJ_BOSS_KA_SAUCERER:
            Katina_KaSaucerer_Init(&gBosses[index]);
            break;
        case OBJ_BOSS_SY_SHOGUN:
            SectorY_SyShogun_Init(&gBosses[index]);
            break;
        case OBJ_ACTOR_MA_LOCOMOTIVE:
        case OBJ_ACTOR_MA_TRAIN_CAR_1:
        case OBJ_ACTOR_MA_TRAIN_CAR_2:
        case OBJ_ACTOR_MA_TRAIN_CAR_3:
        case OBJ_ACTOR_MA_TRAIN_CAR_4:
        case OBJ_ACTOR_MA_TRAIN_CAR_5:
        case OBJ_ACTOR_MA_TRAIN_CAR_6:
        case OBJ_ACTOR_MA_TRAIN_CAR_7:
            Macbeth_Train_Init(&gActors[index]);
            break;
        case OBJ_ACTOR_207:
            Macbeth_Actor207_Init(&gActors[index]);
            break;
        case OBJ_ACTOR_MA_RAILROAD_SWITCH:
            Macbeth_MaMaRailroadSwitch_Init(&gActors[index]);
            break;
        case OBJ_ACTOR_MA_BOULDER:
            Macbeth_MaBoulder_Init(&gActors[index]);
            break;
        case OBJ_ACTOR_MA_RAILWAY_SIGNAL:
            Macbeth_MaRailwaySignal_Init(&gActors[index]);
            break;
        case OBJ_ACTOR_MA_HORIZONTAL_LOCK_BAR:
        case OBJ_ACTOR_MA_VERTICAL_LOCK_BAR:
            Macbeth_LockBars_Init(&gActors[index]);
            break;
        case OBJ_ACTOR_MA_BARRIER:
            Macbeth_MaBarrier_Init(&gActors[index]);
            break;
        case OBJ_SCENERY_MA_PROXIMITY_LIGHT:
            Macbeth_MaProximityLight_Init(&gScenery[index]);
            break;
        case OBJ_ACTOR_CO_GARUDA_2:
        case OBJ_ACTOR_CO_GARUDA_3:
            Corneria_CoIBeam_Init(&gActors[index]);
            /* fallthrough */
        case OBJ_ACTOR_CO_GARUDA_1:
            gActors[index].health = 24;
            AUDIO_PLAY_SFX(NA_SE_EN_TANK_RB_ENGINE, gActors[index].sfxSource, 4);
            break;
        case OBJ_BOSS_ME_CRUSHER:
            Meteo_MeCrusher_Init(&gBosses[index]);
            break;
        case OBJ_BOSS_UNK_299:
            Boss299_Init(&gBosses[index]);
            break;
        case OBJ_BOSS_UNK_300:
            Boss300_Init(&gBosses[index]);
            break;
        case OBJ_BOSS_CO_GRANGA:
            Corneria_Granga_Init(&gBosses[index]);
            break;
        case OBJ_BOSS_CO_CARRIER:
            Corneria_CoCarrier_Init(&gBosses[index]);
            break;
        case OBJ_BOSS_A6_GORGON:
            Area6_A6Gorgon_Init(&gBosses[index]);
            break;
        case OBJ_ACTOR_TI_BOMB:
            Titania_TiBomb_Init(&gActors[index]);
            break;
        case OBJ_ACTOR_TI_RASCO:
            Titania_TiRasco_Init(&gActors[index]);
            break;
        case OBJ_ACTOR_TI_FEKUDA:
            Titania_TiFekuda_Init(&gActors[index]);
            break;
        case OBJ_ACTOR_TI_DESERT_CRAWLER:
            Titania_TiDesertCrawler_Init(&gActors[index]);
            break;
        case OBJ_ACTOR_TI_DELPHOR:
            Titania_TiDelphor_Init(&gActors[index]);
            break;
        case OBJ_ACTOR_TI_DELPHOR_HEAD:
            Titania_TiDelphorHead_Init(&gActors[index]);
            break;
        case OBJ_SPRITE_TI_CACTUS:
            Titania_TiCactus_Init(&gSprites[index]);
            break;
        case OBJ_BOSS_TI_GORAS:
            Titania_TiGoras_Init(&gBosses[index]);
            break;
        case OBJ_ACTOR_ZO_Z_GULL:
            Zoness_ZoEnergyBall_Init(&gActors[index]);
            break;
        case OBJ_ACTOR_ZO_ENERGY_BALL:
            Zoness_ZoEnergyBall_Init2(&gActors[index]);
            break;
        case OBJ_BOSS_ZO_SARUMARINE:
            Zoness_ZoSarumarine_Init(&gBosses[index]);
            break;
        case OBJ_ACTOR_ZO_TANKER:
            Zoness_ZoTanker_Init(&gActors[index]);
            break;
        case OBJ_ACTOR_ZO_CONTAINER:
            Zoness_ZoContainer_Init(&gActors[index]);
            break;
        case OBJ_ACTOR_ZO_SUPPLYCRANE:
            Zoness_ZoSupplyCrane_Init(&gActors[index]);
            break;
        case OBJ_ACTOR_255:
            Aquas_Actor255_Init(&gActors[index]);
            break;
        case OBJ_ACTOR_256:
            Aquas_Actor256_Init(&gActors[index]);
            break;
        case OBJ_ACTOR_257:
            Aquas_Actor257_Init(&gActors[index]);
            break;
        case OBJ_BOSS_AQ_BACOON:
            Aquas_AqBacoon_Init(&gBosses[index]);
            break;
        case OBJ_ACTOR_AQ_ANGLERFISH:
            Aquas_AqAnglerFish_Init(&gActors[index]);
            break;
        case OBJ_ACTOR_AQ_SPINDLYFISH:
            Aquas_AqSpindlyFish_Init(&gActors[index]);
            break;
        case OBJ_ACTOR_AQ_GAROA:
            Aquas_AqGaroa_Init(&gActors[index]);
            break;
        case OBJ_SCENERY_TI_PILLAR:
            Titania_TiPillar_Init(&gScenery[index]);
            break;
        case OBJ_BOSS_VE1_GOLEMECH:
            Venom1_Ve1Golemech_Init(&gBosses[index]);
            break;
        case OBJ_ACTOR_VE1_PILLAR_1:
            Venom1_Ve1Pillar1_Init(&gActors[index]);
            break;
        case OBJ_ACTOR_VE1_PILLAR_2:
        case OBJ_ACTOR_VE1_PILLAR_3:
            Venom1_Ve1Pillar2_3_Init(&gActors[index]);
            break;
        case OBJ_ACTOR_VE1_PILLAR_4:
            Venom1_Ve1Pillar4_Init(&gActors[index]);
            break;
        case OBJ_ACTOR_VE1_MONKEY_STATUE:
            Venom1_Ve1MonkeyStatue_Init(&gActors[index]);
            break;
        case OBJ_ACTOR_AQ_BOULDER:
            Aquas_AqBoulder_Init(&gActors[index]);
            break;
        case OBJ_ACTOR_AQ_JELLYFISH:
            Aquas_AqJellyfish_Init(&gActors[index]);
            break;
    }
}

void Scenery_UpdateTitaniaBones(Scenery* this) {
    f32 sp2C;
    f32 sp28;
    f32 sp24;

    if ((gGroundType == 4) && (this->state == 0)) {
        Ground_801B6E20(this->obj.pos.x, this->obj.pos.z + gPathProgress, &sp2C, &sp24, &sp28);
        this->obj.pos.y = sp24 + 3.0f;
        this->obj.rot.x = RAD_TO_DEG(sp2C);
        this->obj.rot.z = RAD_TO_DEG(sp28);
        this->state++;
    }
}

void func_enmy_80065380(MeMeteor2* this, f32 xPos, f32 yPos, f32 zPos, f32 arg4, f32 arg5, f32 arg6) {
    Actor_Initialize(this);
    this->obj.status = OBJ_ACTIVE;
    this->obj.id = OBJ_ACTOR_ME_METEOR_2;
    this->timer_0BC = RAND_INT(10.0f) + 10;
    this->timer_0C2 = 30;
    this->vel.x = arg4;
    this->vel.y = arg5;
    this->vel.z = arg6;
    this->obj.pos.x = xPos;
    this->obj.pos.y = yPos;
    this->obj.pos.z = zPos;
    this->work_046 = 255;
    this->work_048 = 900;
    this->obj.rot.z = RAND_FLOAT(360.0f);
    this->obj.rot.y = RAND_FLOAT(360.0f);
    Object_SetInfo(&this->info, this->obj.id);
}

void func_enmy_8006546C(f32 xPos, f32 yPos, f32 zPos, f32 arg3, f32 arg4, f32 arg5) {
    s32 i;

    for (i = 0; i < ARRAY_COUNT(gActors); i++) {
        if (gActors[i].obj.status == OBJ_FREE) {
            func_enmy_80065380(&gActors[i], xPos, yPos, zPos, arg3, arg4, arg5);
            break;
        }
    }
}

void func_enmy_800654E4(Object* obj) {
    f32 temp_fs0;
    f32 temp_fs1;
    s32 i;

    func_effect_8007D2C8(obj->pos.x, obj->pos.y, obj->pos.z, 12.0f);

    for (i = 0; i < 4; i++) {
        func_enmy_8006546C(obj->pos.x, obj->pos.y, obj->pos.z, RAND_FLOAT_CENTERED(40.0f), RAND_FLOAT_CENTERED(40.0f),
                           RAND_FLOAT(-20.0f));
    }
}

void func_enmy_800655C8(ActorMissileSeekTeam* this, f32 xPos, f32 yPos, f32 zPos, s32 eventType) {
    Actor_Initialize(this);
    this->obj.status = OBJ_INIT;
    this->obj.id = OBJ_ACTOR_MISSILE_SEEK_TEAM;
    this->obj.pos.x = xPos;
    this->obj.pos.y = yPos;
    this->obj.pos.z = zPos;
    this->eventType = eventType;
    this->timer_0BE = 50;
    if (this->eventType == 1) {
        this->timer_0BE = 30;
    }
    this->fwork[5] = 15.0f;
    Object_SetInfo(&this->info, this->obj.id);
}

void func_enmy_8006566C(f32 xPos, f32 yPos, f32 zPos, s32 arg3) {
    s32 i;

    for (i = 0; i < ARRAY_COUNT(gActors); i++) {
        if (gActors[i].obj.status == OBJ_FREE) {
            func_enmy_800655C8(&gActors[i], xPos, yPos, zPos, arg3);
            break;
        }
    }
}

void ActorMissileSeek_Update(Actor* this) {
    s32 i;
    s32 j;
    f32 spD4;
    f32 spD0;
    f32 spCC;
    f32 var_fv0;
    s32 spC4;
    s32 var_ra;
    s32 spB4[3];
    s32 spA8[3];
    s32 temp_a3_2;
    Vec3f sp98;
    Vec3f sp8C;
    f32 sp88;
#ifdef AVOID_UB
    f32 sp84 = 0;
#else
    f32 sp84;
#endif
    f32 sp80;
    f32 sp7C;
    f32 sp78;

    var_ra = (gLevelMode == LEVELMODE_ALL_RANGE) ? 2 : 0;

    if (this->iwork[2] == 0) {
        if (this->obj.id == OBJ_ACTOR_MISSILE_SEEK_TEAM) {
            for (i = 0; i < 3; i++) {
                spB4[i] = gTeamShields[i + 1];
                spA8[i] = var_ra + i;
            }

            for (i = 0; i < 3; i++) {
                for (j = i + 1; j < 3; j++) {
                    if (spB4[i] < spB4[j]) {
                        temp_a3_2 = spB4[j];
                        spB4[j] = spB4[i];
                        spB4[i] = temp_a3_2;
                        temp_a3_2 = spA8[j];
                        spA8[j] = spA8[i];
                        spA8[i] = temp_a3_2;
                    }
                }
            }

            switch (gGameFrameCount % 6U) {
                case 0:
                case 1:
                case 2:
                    spC4 = spA8[2];
                    break;
                case 3:
                case 4:
                    spC4 = spA8[1];
                    break;
                case 5:
                    spC4 = spA8[0];
                    break;
            }

            if (gLevelMode == LEVELMODE_ALL_RANGE) {
                if (gTeamShields[spC4 - 1] > 0) {
                    this->iwork[1] = spC4;
                    goto label;
                } else {
                    this->iwork[1] = 10000;
                }
            } else {
                if (gTeamShields[spC4 + 1] > 0) {
                    this->iwork[1] = spC4;
                    goto label;
                } else {
                    this->iwork[1] = 10000;
                }
            }
        }
        this->iwork[1] = 10000;
    label:
        this->iwork[2] = 1;
    }

    spC4 = this->iwork[1];
    if ((spC4 == var_ra) || ((var_ra + 1) == spC4) || ((var_ra + 2) == spC4)) {
        this->fwork[29] = gActors[spC4].obj.pos.z;
        this->fwork[28] = gActors[spC4].obj.pos.y;
        this->fwork[27] = gActors[spC4].obj.pos.x;
        if ((fabsf(this->obj.pos.x - gActors[spC4].obj.pos.x) < 400.0f) &&
            (fabsf(this->obj.pos.z - gActors[spC4].obj.pos.z) < 400.0f)) {
            if (RAND_FLOAT(spC4 - 1) < 0.6f) {
                gActors[spC4].iwork[10] = 1;
            }
        }
    } else {
        this->fwork[29] = gPlayer[0].trueZpos;
        this->fwork[28] = gPlayer[0].pos.y;
        this->fwork[27] = gPlayer[0].pos.x;
    }

    if (this->timer_0BC != 0) {
        Math_SmoothStepToAngle(&this->obj.rot.x, 0.0f, 0.3f, 4.0f, 0.001f);
    } else {
        if ((this->iwork[10] == 0) && ((fabsf(this->fwork[27] - this->obj.pos.x) > 300.0f) ||
                                       (fabsf(this->fwork[29] - this->obj.pos.z) > 300.0f))) {
            this->fwork[0] += 5.0f;
            this->fwork[1] += 8.0f;
            sp88 = this->fwork[27] - this->obj.pos.x;
            sp80 = this->fwork[29] - this->obj.pos.z;
            sp80 = sqrtf(SQ(sp88) + SQ(sp80)) * 0.2f;
            if (this->eventType == 1) {
                sp80 = 0.1f;
            }
            spD0 = SIN_DEG(this->fwork[0]) * sp80;
            sp88 = COS_DEG(this->fwork[1]) * sp80;
            spD4 = COS_DEG(this->obj.rot.y) * sp88;
            spCC = -SIN_DEG(this->obj.rot.y) * sp88;

            sp88 = (this->fwork[27] + spD4) - this->obj.pos.x;
            sp84 = (this->fwork[28] + spD0) - this->obj.pos.y;
            sp80 = (this->fwork[29] + spCC) - this->obj.pos.z;
            sp78 = Math_RadToDeg(Math_Atan2F(sp88, sp80));
            sp80 = sqrtf(SQ(sp88) + SQ(sp80));
            sp7C = Math_RadToDeg(-Math_Atan2F(sp84, sp80));
            sp84 = Math_SmoothStepToAngle(&this->obj.rot.y, sp78, 0.3f, 4.0f, 0.001f);
            Math_SmoothStepToAngle(&this->obj.rot.x, sp7C, 0.3f, 4.0f, 0.001f);
        }

        if ((fabsf(this->fwork[27] - this->obj.pos.x) < 60.0f) && (fabsf(this->fwork[28] - this->obj.pos.y) < 60.0f) &&
            (fabsf(this->fwork[29] - this->obj.pos.z) < 60.0f) && ((spC4 == 2) || (spC4 == 3) || (spC4 == 4))) {
            gActors[spC4].dmgType = DMG_BEAM;
            gActors[spC4].damage = 20;
            gActors[spC4].dmgSource = DMG_SRC_2;
            Effect_SpawnTimedSfxAtPos(&this->obj.pos, NA_SE_EN_EXPLOSION_S);
            func_effect_8007D2C8(this->obj.pos.x, this->obj.pos.y, this->obj.pos.z, 5.0f);
            Object_Kill(&this->obj, this->sfxSource);
        }
    }

    Math_Vec3fFromAngles(&sp98, this->obj.rot.x, this->obj.rot.y, this->fwork[5]);

    this->vel.x = sp98.x;
    this->vel.y = sp98.y;
    this->vel.z = sp98.z - gPathVelZ;

    if (this->eventType == 0) {
        this->obj.rot.z += 5.0f;
    }
    if (this->eventType == 1) {
        if (this->timer_0BE == 0) {
            this->timer_0BE = 30;
            Math_Vec3fFromAngles(&sp98, this->obj.rot.x, this->obj.rot.y, 120.0f);
            func_effect_8007F04C(OBJ_EFFECT_ENEMY_LASER_1, this->obj.pos.x + sp98.x, this->obj.pos.y + sp98.y,
                                 this->obj.pos.z + sp98.z, this->obj.rot.x, this->obj.rot.y, this->obj.rot.z, 0.0f,
                                 0.0f, 0.0f, sp98.x, sp98.y, sp98.z, 1.0f);
        }
        var_fv0 = 330.0f;
        if (sp84 < 0.0f) {
            var_fv0 = 30.0f;
        }
        Math_SmoothStepToAngle(&this->obj.rot.z, var_fv0, 0.1f, 3.0f, 0.01f);
    }

    if ((gGroundType == 4) && Ground_801B6AEC(this->obj.pos.x, this->obj.pos.y, this->obj.pos.z + gPathProgress)) {
        func_effect_8007D2C8(this->obj.pos.x, this->obj.pos.y, this->obj.pos.z, 5.0f);
        Object_Kill(&this->obj, this->sfxSource);
    }

    sp8C.x = this->vel.x;
    sp8C.y = this->vel.y;
    sp8C.z = this->vel.z;

    if ((Object_CheckCollision(this->index, &this->obj.pos, &sp8C, 1) != 0) || (this->dmgType != DMG_NONE) ||
        (this->obj.pos.y < (gGroundHeight + 10.0f)) || (gPlayer[0].state == PLAYERSTATE_LEVEL_COMPLETE)) {
        func_effect_8007D2C8(this->obj.pos.x, this->obj.pos.y, this->obj.pos.z, 3.0f);
        Object_Kill(&this->obj, this->sfxSource);
        if (this->dmgType != DMG_NONE) {
            this->itemDrop = DROP_SILVER_RING_50p;
            if ((gCurrentLevel == LEVEL_CORNERIA)) {
                if (gLevelMode == LEVELMODE_ALL_RANGE) {
                    this->itemDrop = DROP_SILVER_RING_50p;
                } else {
                    this->itemDrop = DROP_SILVER_RING_25p;
                }
            }
            if (gCurrentLevel == LEVEL_AREA_6) {
                this->itemDrop = DROP_SILVER_RING_10p;
            }
            Actor_Despawn(this);
        }
        Effect_SpawnTimedSfxAtPos(&this->obj.pos, NA_SE_EN_EXPLOSION_S);
    }

    if (gLevelMode == LEVELMODE_ON_RAILS) {
        if (fabsf(this->obj.pos.z - gPlayer[0].trueZpos) < 100.0f) {
            this->iwork[10] = 1;
        }
        if (gPlayer[0].cam.eye.z < (this->obj.pos.z + gPathProgress)) {
            Object_Kill(&this->obj, this->sfxSource);
        }
    }
}

void func_enmy_800660F0(Actor* this) {
    Item* item;
    s32 i;

    for (item = &gItems[0], i = 0; i < ARRAY_COUNT(gItems); i++, item++) {
        if (item->obj.status == OBJ_FREE) {
            Item_Initialize(item);
            item->obj.status = OBJ_INIT;
            item->obj.id = D_enmy_800CFDF4[this->itemDrop];
            item->obj.pos.x = this->obj.pos.x;
            item->obj.pos.y = this->obj.pos.y;
            item->obj.pos.z = this->obj.pos.z;
            item->timer_4A = 8;

            CALL_CANCELLABLE_EVENT(ItemDropEvent, item) {
                Object_SetInfo(&item->info, item->obj.id);
                if ((item->obj.id == OBJ_ITEM_SILVER_RING) || (item->obj.id == OBJ_ITEM_BOMB) ||
                    (item->obj.id == OBJ_ITEM_LASERS)) {
                    item->unk_50 = 90.0f;
                }

                if ((item->obj.id >= OBJ_ITEM_GOLD_RING) || (item->obj.id == OBJ_ITEM_1UP)) {
                    item->unk_50 = 90.0f;
                    AUDIO_PLAY_SFX(NA_SE_ITEM_APPEAR, gDefaultSfxSource, 4);
                    item->timer_48 = 1000;
                    if (item->obj.id == OBJ_ITEM_WING_REPAIR) {
                        AUDIO_PLAY_SFX(NA_SE_OB_WING, item->sfxSource, 0);
                    }
                }
            }

            break;
        }
    }
}

void Actor_Despawn(Actor* this) {
    Actor* otherActor;
    s32 i;

    if (gVersusMode) {
        if ((this->dmgSource >= 1) && (this->dmgSource < 4 + 1) && !VS_TIME_UP(gVsCountdown)) {
            gPlayerScores[this->dmgSource - 1] += this->info.bonus;
        }
    } else if (!((this->obj.id == OBJ_ACTOR_ALLRANGE) && (this->animFrame == 1))) {
        if ((this->dmgSource == AI360_FOX + 1) && (this->info.bonus != 0)) {
            gHitCount += this->info.bonus;
            D_ctx_80177850 = 15;

            if ((gLevelMode == LEVELMODE_ALL_RANGE) && (gDropHitCountItem != 0)) {
                switch (gDropHitCountItem) {
                    case 9:
                        if (gCurrentLevel == LEVEL_KATINA) {
                            Radio_PlayMessage(gMsg_ID_18031, RCID_BILL);
                        }
                        /* fallthrough */
                    case 4:
                    case 19:
                    case 29:
                        this->itemDrop = DROP_GOLD_RING_1;
                        break;
                    case 14:
                        this->itemDrop = DROP_LASERS;
                        break;
                }
                gDropHitCountItem = 0;
            }

            if (this->obj.id == OBJ_ACTOR_ALLRANGE) {
                if ((this->aiType >= AI360_WOLF) && (this->aiType < AI360_KATT)) {
                    AllRange_GetStarWolfHits(this);
                }
            }
        }

        if (this->itemDrop) {
            if (D_enmy_800CFE5C[this->itemDrop] < 0.0f) {
                otherActor = &gActors[0];
                for (i = 0, otherActor = &gActors[0]; i < ARRAY_COUNT(gActors); i++, otherActor++) {
                    if ((otherActor->obj.status != OBJ_FREE) && (otherActor->index != this->index) &&
                        (otherActor->iwork[15] == this->iwork[15])) {
                        return;
                    }
                }
                func_enmy_800660F0(this);
                AUDIO_PLAY_SFX(NA_SE_ITEM_APPEAR, gDefaultSfxSource, 4);
            } else if (this->itemDrop == DROP_TEAM_MESG) {
                if (gTeamShields[TEAM_ID_PEPPY] > 0) {
                    Radio_PlayMessage(gMsg_ID_20261, RCID_PEPPY);
                } else if (gTeamShields[TEAM_ID_SLIPPY] > 0) {
                    Radio_PlayMessage(gMsg_ID_20263, RCID_SLIPPY);
                } else if (gTeamShields[TEAM_ID_FALCO] > 0) {
                    Radio_PlayMessage(gMsg_ID_20262, RCID_FALCO);
                }
            } else if (Rand_ZeroOne() <= D_enmy_800CFE5C[this->itemDrop]) {
                func_enmy_800660F0(this);
            }
        }
    }
}

void CoSkibot_Update(CoSkibot* this) {
    this->gravity = 0.4f;

    if (this->obj.pos.y <= gGroundHeight + 130.0f) {
        this->obj.pos.y = gGroundHeight + 130.0f;
        this->vel.y = 0.0f;
    }

    this->vel.x = SIN_DEG(this->obj.rot.y) * this->fwork[0];
    this->vel.z = COS_DEG(this->obj.rot.y) * this->fwork[0];

    switch (this->state) {
        case 0:
            if (this->fwork[0] < 20.0f) {
                this->fwork[0] += 0.5f;
            }

            this->animFrame++;
            if (Animation_GetFrameCount(&aCoSkibotAnim) < this->animFrame) {
                this->animFrame = 0;
            }

            if ((this->obj.rot.z < 15.0f) && (this->animFrame < 20)) {
                this->obj.rot.z += 1.0f;
            }
            if ((this->obj.rot.z > -15.0f) && (this->animFrame > 20)) {
                this->obj.rot.z -= 1.0f;
            }

            if ((this->animFrame == 20) || (this->animFrame == 40)) {
                this->state++;
                this->timer_0BC = 20;
            }
            break;

        case 1:
            if (this->obj.rot.z > 0.0f) {
                this->obj.rot.z -= 0.5f;
            }
            if (this->obj.rot.z < 0.0f) {
                this->obj.rot.z += 0.5f;
            }

            if (this->fwork[0] > 0.0f) {
                this->fwork[0] -= 0.3f;
            }

            if (this->timer_0BC == 0) {
                this->state = 0;
            }
            break;
    }

    this->scale = 0.8f;

    if (this->dmgType != DMG_NONE) {
        this->obj.status = OBJ_DYING;
        this->vel.y = RAND_FLOAT(5.0f) + 6.0f;
        if (this->dmgType == DMG_EXPLOSION) {
            this->vel.y = -2.0f;
        }
        this->vel.z = -15.0f;
        this->gravity = 0.5f;
        func_effect_8007D2C8(this->obj.pos.x, this->obj.pos.y + 30.0f, this->obj.pos.z, 13.0f);
        AUDIO_PLAY_SFX(NA_SE_EN_EXPLOSION_S, this->sfxSource, 4);
    }
}

void func_enmy_8006684C(CoSkibot* this) {
    s32 pad;

    if (this->timer_0BE != 0) {
        this->vel.z = 0.0f;
        this->vel.x = 0.0f;
        this->vel.y = 0.0f;
        if (this->timer_0BE == 1) {
            Object_Kill(&this->obj, this->sfxSource);
            this->itemDrop = DROP_SILVER_RING;
            Actor_Despawn(this);
            AUDIO_PLAY_SFX(NA_SE_EN_EXPLOSION_M, this->sfxSource, 4);
            BonusText_Display(this->obj.pos.x, this->obj.pos.y + 250.0f, this->obj.pos.z, 3);
            gHitCount += 3;
            D_ctx_80177850 = 15;
        }
    } else {
        this->obj.rot.x += 11.0f;
        this->obj.rot.y += 7.0f;
        if (this->vel.y < -3.0f) {
            Effect386_Spawn1(this->obj.pos.x, this->obj.pos.y + 30.0f, this->obj.pos.z, 0.0f, 0.0f, 0.0f, 7.0f, 5);
            this->timer_0BE = 3;
        }
    }
}

void CoRadar_Update(CoRadar* this) {
    if (this->timer_0BC != 0) {
        if (this->timer_0BC == 1) {
            Object_Kill(&this->obj, this->sfxSource);
        }
    } else {
        this->obj.rot.y += 5.0f;
        if (this->dmgType != DMG_NONE) {
            func_effect_8007D0E0(this->obj.pos.x, this->obj.pos.y + 130.0f, this->obj.pos.z, 8.0f);
            Effect386_Spawn1(this->obj.pos.x, this->obj.pos.y + 130.0f, this->obj.pos.z, 0.0f, 0.0f, 0.0f, 4.0f, 5);
            this->timer_0BC = 4;
            Effect_SpawnTimedSfxAtPos(&this->obj.pos, NA_SE_OB_EXPLOSION_S);
        }
    }
}

void MeMolarRock_Update(MeMolarRock* this) {
}

void func_enmy_80066A8C(CoBuilding9* this) {
    Vec3f src;
    Vec3f dest;
    f32 yf;

    src.z = 0.0f;
    src.x = -120.0f;

    if (this->obj.rot.y > 90.0f) {
        src.x = 120.0f;
    }

    for (yf = 0.0f; yf < 680.0f; yf += 100.0f) {
        Matrix_RotateY(gCalcMatrix, this->obj.rot.y * M_DTOR, MTXF_NEW);
        Matrix_RotateX(gCalcMatrix, this->obj.rot.x * M_DTOR, MTXF_APPLY);
        src.y = yf;
        Matrix_MultVec3fNoTranslate(gCalcMatrix, &src, &dest);
        func_effect_8007D0E0(this->obj.pos.x + dest.x, this->obj.pos.y + dest.y, this->obj.pos.z + dest.z, 4.0f);
    }
}

void func_enmy_80066C00(CoBuilding9* this) {
    Vec3f src;
    Vec3f dest;
    f32 zf;

    src.y = 30.0f;
    src.x = -100.0f;

    if (this->obj.rot.y > 90.0f) {
        src.x = 100.0f;
    }

    Matrix_RotateY(gCalcMatrix, this->obj.rot.y * M_DTOR, MTXF_NEW);

    for (zf = -180.0f; zf <= 0.0f; zf += 30.0f) {
        src.z = zf;
        Matrix_MultVec3fNoTranslate(gCalcMatrix, &src, &dest);
        func_effect_8007D0E0(this->obj.pos.x + dest.x, this->obj.pos.y + dest.y, this->obj.pos.z + dest.z,
                             RAND_FLOAT(1.0f) + 2.0f);
    }
}

void CoBuilding9_Update(CoBuilding9* this) {
    switch (this->state) {
        case 0:
            break;

        case 1:
            func_enmy_80066C00(this);
            this->state++;
            AUDIO_PLAY_SFX(NA_SE_EN_DAMAGE_L, this->sfxSource, 4);
            break;

        case 2:
            this->vel.x += 0.05f;
            this->obj.rot.x += this->vel.x;
            if (this->obj.rot.x >= 90.0f) {
                this->obj.rot.x = 90.0f;
                this->unk_44 = 40;
                func_enmy_80066A8C(this);
                this->state = 0;
                gCameraShake = 25;
                AUDIO_PLAY_SFX(NA_SE_EN_METAL_BOUND_M, this->sfxSource, 4);
            }
            break;
    }
}

void func_enmy_80066E80(Scenery* this) {
}

void Sprite167_Update(Sprite167* this) {
    this->obj.rot.y += 0.2f;
}

// World-aligned billboarding
void SceneryRotateTowardsCamera(Scenery* this) {
    bool isBuilding = (this->obj.id >= OBJ_SCENERY_CO_BUILDING_5 && this->obj.id <= OBJ_SCENERY_CO_BUILDING_8 ||
                       this->obj.id == OBJ_SCENERY_CO_BUILDING_10);

    if (isBuilding) {
        return;
    }

    this->obj.rot.y = 0.0f;
    if (gPlayer[0].cam.eye.x < this->obj.pos.x) {
        this->obj.rot.y = 271.0f;
    }
}

void func_enmy_80066EE4(Sprite* this) {
}

void Item_CheckBounds(Item* this) {
    f32 var_fa1;

    if ((gPlayer[0].state == PLAYERSTATE_LEVEL_COMPLETE) || (gPlayer[0].state == PLAYERSTATE_STANDBY)) {
        Object_Kill(&this->obj, this->sfxSource);
    }

    if ((gLevelMode == LEVELMODE_ON_RAILS) && (gLastPathChange == 0)) {
        var_fa1 = 900.0f;
        if (gPlayer[0].form != FORM_ARWING) {
            var_fa1 = 600.0f;
        }
        if (this->obj.pos.x > gPlayer[0].xPath + var_fa1) {
            Math_SmoothStepToF(&this->obj.pos.x, gPlayer[0].xPath + var_fa1, 0.1f, 10.0f, 0.01f);
        }
        if (this->obj.pos.x < gPlayer[0].xPath - var_fa1) {
            Math_SmoothStepToF(&this->obj.pos.x, gPlayer[0].xPath - var_fa1, 0.1f, 10.0f, 0.01f);
        }
    }
    if (this->obj.pos.y > 650.0f) {
        Math_SmoothStepToF(&this->obj.pos.y, 650.0f, 0.1f, 10.0f, 0.01f);
    }

    if (gLevelType == LEVELTYPE_PLANET) {
        if (this->obj.pos.y < gGroundHeight + 70.0f) {
            Math_SmoothStepToF(&this->obj.pos.y, gGroundHeight + 70.0f, 0.1f, 5.0f, 0.01f);
        }
        if ((gCurrentLevel == LEVEL_AQUAS) && gBossActive) {
            this->obj.pos.z += 20.0f;
        }
    } else if (this->obj.pos.y < -500.0f) {
        Math_SmoothStepToF(&this->obj.pos.y, -500.0f, 0.1f, 5.0f, 0.01f);
    }
    if (gVersusMode && (this->index == 0) && (gItems[1].obj.status == OBJ_ACTIVE)) {
        if (fabsf(this->obj.pos.x - gItems[1].obj.pos.x) < 200.0f) {
            if (fabsf(this->obj.pos.z - gItems[1].obj.pos.z) < 200.0f) {
                this->obj.pos.x = this->obj.pos.x - 5.0f;
                this->obj.pos.z = this->obj.pos.z - 5.0f;
                gItems[1].obj.pos.x += 5.0f;
                gItems[1].obj.pos.z += 5.0f;
            }
        }
    }
}

void Item_SpinPickup(Item* this) {
    s32 sparkleMask;
    Vec3f src;
    Vec3f dest;

    Math_SmoothStepToF(&this->unk_50, 10.0f, 1.0f, 2.0f, 0.0f);

    if (this->unk_50 > 30.0f) {
        sparkleMask = 1 - 1;
    } else if (this->unk_50 > 20.0f) {
        sparkleMask = 2 - 1;
    } else {
        sparkleMask = 8 - 1;
    }

    if ((sparkleMask & gGameFrameCount) == 0) {
        Matrix_RotateY(gCalcMatrix, gGameFrameCount * 23.0f * M_DTOR, MTXF_NEW);
        src.x = 50.0f;
        src.y = RAND_FLOAT_CENTERED(120.0f);
        src.z = 0.0f;
        Matrix_MultVec3fNoTranslate(gCalcMatrix, &src, &dest);
        Effect_Effect393_Spawn(this->obj.pos.x + dest.x, this->obj.pos.y + dest.y, this->obj.pos.z + dest.z, 3.0f);
    }
    this->obj.rot.y += this->unk_50;
    this->obj.rot.y = Math_ModF(this->obj.rot.y, 360.0f);
}

void Actor_SetupDebris70(ActorDebris* this, f32 xPos, f32 yPos, f32 zPos, f32 xRot, f32 yRot, f32 xVel, f32 yVel,
                         f32 zVel) {
    Actor_Initialize(this);
    this->obj.status = OBJ_ACTIVE;
    this->obj.id = OBJ_ACTOR_DEBRIS;
    this->state = 70;
    this->obj.pos.x = xPos;
    this->obj.pos.y = yPos;
    this->obj.pos.z = zPos;
    this->obj.rot.x = xRot;
    this->obj.rot.y = yRot;
    this->vel.x = xVel;
    this->vel.y = yVel;
    this->vel.z = zVel;
    this->timer_0BC = RAND_INT(15.0f) + 25.0f;
    this->gravity = 0.5f;
    Object_SetInfo(&this->info, this->obj.id);
}

void Actor_SpawnDebris70(f32 xPos, f32 yPos, f32 zPos, f32 xRot, f32 yRot, f32 arg5, f32 arg6, f32 arg7) {
    s32 i;

    for (i = ARRAY_COUNT(gActors) - 1; i >= 50; i--) {
        if (gActors[i].obj.status == OBJ_FREE) {
            Actor_SetupDebris70(&gActors[i], xPos, yPos, zPos, xRot, yRot, arg5, arg6, arg7);
            break;
        }
    }
}

void ActorSupplies_Update(ActorSupplies* this) {
    Player* player = &gPlayer[0];
    s32 i;

    this->obj.rot.y += 1.0f;

    if (gLevelMode == LEVELMODE_ALL_RANGE) {
        if (gCurrentLevel == LEVEL_SECTOR_Z) {
            Math_SmoothStepToF(&this->obj.pos.x, -2000.0f, 0.05f, 60.0f, 0.01f);
            Math_SmoothStepToF(&this->obj.pos.y, -200.0f, 0.05f, 3.0f, 0.01f);
            Math_SmoothStepToF(&this->obj.pos.z, 0.0f, 0.05f, 0.f, 0.01f);
        } else {
            Math_SmoothStepToF(&this->obj.pos.y, 300.0f, 0.05f, 50.0f, 0.01f);
        }
    }
    if (this->dmgType != DMG_NONE) {
        this->dmgType = DMG_NONE;
        this->health -= this->damage;
        if (this->health <= 0) {
            Effect_SpawnTimedSfxAtPos(&this->obj.pos, NA_SE_EN_EXPLOSION_S);
            func_effect_8007D2C8(this->obj.pos.x, this->obj.pos.y, this->obj.pos.z, 5.0f);

            if (((player[0].arwing.rightWingState <= WINGSTATE_BROKEN) ||
                 (player[0].arwing.leftWingState <= WINGSTATE_BROKEN)) &&
                (player[0].form != FORM_LANDMASTER)) {
                this->itemDrop = DROP_WING_REPAIR;
            } else if (gPlayer[0].shields < 128) {
                this->itemDrop = DROP_SILVER_STAR;
            } else if ((gLaserStrength[0] == LASERS_SINGLE) && (player[0].form != FORM_LANDMASTER)) {
                this->itemDrop = DROP_LASERS;
            } else {
                this->itemDrop = DROP_BOMB;
            }

            Actor_Despawn(this);

            for (i = 0; i < 6; i++) {
                Actor_SpawnDebris70(D_enmy_800CFEC4[i].x + this->obj.pos.x, D_enmy_800CFEC4[i].y + this->obj.pos.y,
                                    D_enmy_800CFEC4[i].z + this->obj.pos.z, D_enmy_800CFF0C[i].y + this->obj.rot.y,
                                    D_enmy_800CFF0C[i].x + this->obj.rot.x, RAND_FLOAT_CENTERED(40.0f),
                                    RAND_FLOAT(10.0f) + 10.0f, RAND_FLOAT_CENTERED(40.0f));
                Effect_Effect357_Spawn50(this->obj.pos.x, this->obj.pos.y, this->obj.pos.z, 0.6f);
            }
            Object_Kill(&this->obj, this->sfxSource);
        }
    }
    gRadarMarks[63].enabled = true;
    gRadarMarks[63].type = 103;
    gRadarMarks[63].pos.x = this->obj.pos.x;
    gRadarMarks[63].pos.y = this->obj.pos.y;
    gRadarMarks[63].pos.z = this->obj.pos.z;
    gRadarMarks[63].yRot = 0.0f;
}

void ActorSupplies_Draw(ActorEvent* this) {
    s32 i;

    Lights_SetOneLight(&gMasterDisp, -60, -60, 60, 150, 150, 150, 20, 20, 20);

    for (i = 0; i < 6; i++) {
        Matrix_Push(&gGfxMatrix);
        Matrix_Translate(gGfxMatrix, D_enmy_800CFEC4[i].x, D_enmy_800CFEC4[i].y, D_enmy_800CFEC4[i].z, MTXF_APPLY);
        Matrix_RotateY(gGfxMatrix, D_enmy_800CFF0C[i].y * M_DTOR, MTXF_APPLY);
        Matrix_RotateX(gGfxMatrix, D_enmy_800CFF0C[i].x * M_DTOR, MTXF_APPLY);
        Matrix_SetGfxMtx(&gMasterDisp);
        gSPDisplayList(gMasterDisp++, aActorSuppliesDL);
        Matrix_Pop(&gGfxMatrix);
    }

    Lights_SetOneLight(&gMasterDisp, gLight1x, gLight1y, gLight1z, gLight1R, gLight1G, gLight1B, gAmbientR, gAmbientG,
                       gAmbientB);
}

void func_enmy_80067A40(void) {
    AUDIO_PLAY_SFX(NA_SE_WING_REPAIR, gPlayer[0].sfxSource, 0);

    if (gPlayer[0].arwing.rightWingState <= WINGSTATE_BROKEN) {
        gRightWingFlashTimer[0] = 1050;
        gPlayer[0].arwing.rightWingState = WINGSTATE_INTACT;
    }
    if (gPlayer[0].arwing.leftWingState <= WINGSTATE_BROKEN) {
        gLeftWingFlashTimer[0] = 1050;
        gPlayer[0].arwing.leftWingState = WINGSTATE_INTACT;
    }

    if (gExpertMode) {
        gRightWingHealth[0] = gLeftWingHealth[0] = 10;
    } else {
        gRightWingHealth[0] = gLeftWingHealth[0] = 60;
    }
}

void Item1up_Update(Item1UP* this) {
    Item_CheckBounds(this);
    Item_SpinPickup(this);
    if (this->collected) {
        Object_Kill(&this->obj, this->sfxSource);
        Object_PlayerSfx(gPlayer[this->playerNum].sfxSource, NA_SE_ONE_UP, this->playerNum);
        if (gCurrentLevel != LEVEL_TRAINING) {
            gLifeCount[this->playerNum]++;
        }
    }

    if (this->timer_48 == 1) {
        Object_Kill(&this->obj, this->sfxSource);
    }
}

void ItemPickup_Update(Item* this) {
    Item_CheckBounds(this);
    Item_SpinPickup(this);
    if (this->state == 0) {
        switch (this->obj.id) {
            case OBJ_ITEM_BOMB:
                this->width = 18.0f;
                if (this->collected) {
                    this->timer_4A = 50;
                    this->state = 1;
                    this->timer_48 = 20;
                    this->unk_50 = 60.0f;
                    gBombCount[this->playerNum]++;
                    Object_PlayerSfx(gPlayer[this->playerNum].sfxSource, NA_SE_BOMB_GET, this->playerNum);
                    Object_PlayerSfx(gPlayer[this->playerNum].sfxSource, NA_SE_BOMB_GAUGE_UP, this->playerNum);
                }
                break;

            case OBJ_ITEM_LASERS:
                this->width = 18.0f;
                if (this->collected) {
                    this->timer_4A = 50;
                    this->state = 1;
                    this->timer_48 = 20;
                    this->unk_50 = 60.0f;

                    gLaserStrength[this->playerNum]++;
                    if (gLaserStrength[this->playerNum] > LASERS_HYPER) {
                        gLaserStrength[this->playerNum] = LASERS_HYPER;
                    }

                    Object_PlayerSfx(gPlayer[this->playerNum].sfxSource, NA_SE_TWIN_LASER_GET, this->playerNum);

                    if (gExpertMode) {
                        gRightWingHealth[this->playerNum] = gLeftWingHealth[this->playerNum] = 10;
                    } else {
                        gRightWingHealth[this->playerNum] = gLeftWingHealth[this->playerNum] = 60;
                    }
                    gRightWingFlashTimer[this->playerNum] = 1030;
                    gLeftWingFlashTimer[this->playerNum] = 1030;
                }
                break;
        }
    } else {
        Math_SmoothStepToF(&this->width, 2.5f, 1.0f, 0.5f, 0.0f);
        this->obj.pos.x += (gPlayer[this->playerNum].pos.x - this->obj.pos.x) * 0.5f;

        if (gPlayer[this->playerNum].form == FORM_LANDMASTER) {
            this->obj.pos.y += ((gPlayer[this->playerNum].pos.y + 50.0f) - this->obj.pos.y) * 0.5f;
        } else {
            this->obj.pos.y += (gPlayer[this->playerNum].pos.y - this->obj.pos.y) * 0.5f;
        }

        this->obj.pos.z += (gPlayer[this->playerNum].trueZpos - this->obj.pos.z) * 0.5f;

        if (this->timer_48 == 0) {
            Object_Kill(&this->obj, this->sfxSource);
        }
    }
}

void ItemLasers_Update(ItemLasers* this) {
    if (!gVersusMode && ((gPlayer[0].arwing.leftWingState <= WINGSTATE_BROKEN) ||
                         (gPlayer[0].arwing.rightWingState <= WINGSTATE_BROKEN))) {
        this->obj.id = OBJ_ITEM_WING_REPAIR;
        Object_SetInfo(&this->info, this->obj.id);
        this->timer_48 = 2000;
        AUDIO_PLAY_SFX(NA_SE_OB_WING, this->sfxSource, 0);
    } else {
        ItemPickup_Update(this);
    }
}

void ItemSupplyRing_Update(Item* this) {
    Vec3f src;
    Vec3f dest;

    switch (this->state) {
        case 0:
            Math_SmoothStepToF(&this->width, 0.4f, 1.0f, 0.05f, 0.0f);
            Item_CheckBounds(this);
            Item_SpinPickup(this);
            if (this->collected) {
                this->state = 1;
                this->timer_48 = 50;
                if (this->obj.id == OBJ_ITEM_SILVER_RING) {
                    gPlayer[this->playerNum].heal += 32;
                    Object_PlayerSfx(gPlayer[this->playerNum].sfxSource, NA_SE_SHIELD_RING, this->playerNum);
                } else if (this->obj.id == OBJ_ITEM_GOLD_RING) {
                    gGoldRingCount[0]++;
                    if (gGoldRingCount[0] == 3) {
                        Object_PlayerSfx(gPlayer[this->playerNum].sfxSource, NA_SE_SHIELD_UPGRADE, this->playerNum);
                    } else if (gGoldRingCount[0] == 6) {
                        Object_PlayerSfx(gPlayer[this->playerNum].sfxSource, NA_SE_ONE_UP, this->playerNum);
                        if (gCurrentLevel != LEVEL_TRAINING) {
                            gLifeCount[this->playerNum]++;
                        }
                        gPlayer[this->playerNum].heal += 32;
                        BonusText_Display(gPlayer[this->playerNum].pos.x, gPlayer[this->playerNum].pos.y,
                                          gPlayer[this->playerNum].trueZpos, BONUS_TEXT_1UP);
                    } else {
                        gPlayer[this->playerNum].heal += 32;
                        Object_PlayerSfx(gPlayer[this->playerNum].sfxSource, NA_SE_GOLD_RING, this->playerNum);
                    }
                } else {
                    gPlayer[this->playerNum].heal += 128;
                    Object_PlayerSfx(gPlayer[this->playerNum].sfxSource, NA_SE_SHIELD_RING_M, this->playerNum);
                }
            }

            if ((this->obj.id == OBJ_ITEM_GOLD_RING) && (this->timer_48 == 1)) {
                Object_Kill(&this->obj, this->sfxSource);
            }
            break;

        case 1:
            if (this->timer_48 > 30) {
                Math_SmoothStepToF(&this->width, 1.0f, 1.0f, 0.06f, 0.0f);
            } else {
                Math_SmoothStepToF(&this->width, 0.0f, 1.0f, 0.06f, 0.0f);
            }

            this->obj.pos.x += (gPlayer[this->playerNum].pos.x - this->obj.pos.x) * 0.5f;

            if (gPlayer[this->playerNum].form == FORM_LANDMASTER) {
                this->obj.pos.y += (gPlayer[this->playerNum].pos.y + 50.0f - this->obj.pos.y) * 0.5f;
            } else {
                this->obj.pos.y += (gPlayer[this->playerNum].pos.y - this->obj.pos.y) * 0.5f;
            }
            if (gPlayer[0].alternateView && (gLevelMode == LEVELMODE_ON_RAILS)) {
                this->obj.pos.z += (gPlayer[this->playerNum].trueZpos - 300.0f - this->obj.pos.z) * 0.3f;
            } else {
                this->obj.pos.z += (gPlayer[this->playerNum].trueZpos - this->obj.pos.z) * 0.5f;
            }

            this->obj.rot.z += 22.0f;
            Math_SmoothStepToAngle(&this->obj.rot.y, Math_RadToDeg(-gPlayer[this->playerNum].camYaw), 0.2f, 10.0f,
                                   0.0f);
            if (this->timer_48 == 0) {
                Object_Kill(&this->obj, this->sfxSource);
            }

            if (this->width > 0.3f) {
                Matrix_RotateY(gCalcMatrix, this->obj.rot.y * M_DTOR, MTXF_NEW);
                Matrix_RotateZ(gCalcMatrix, gGameFrameCount * 37.0f * M_DTOR, MTXF_APPLY);
                src.x = 0.0f;
                src.y = this->width * 100.0f;
                src.z = 0.0f;
                Matrix_MultVec3fNoTranslate(gCalcMatrix, &src, &dest);
                Effect_Effect393_Spawn(this->obj.pos.x + dest.x, this->obj.pos.y + dest.y, this->obj.pos.z + dest.z,
                                       3.5f);
            }
            break;
    }
}

void ItemSilverStar_Update(ItemSilverStar* this) {
    ItemSupplyRing_Update(this);
}

void ItemGoldRing_Update(ItemGoldRing* this) {
    ItemSupplyRing_Update(this);
}

void ItemWingRepair_Update(ItemWingRepair* this) {
    Item_CheckBounds(this);
    Item_SpinPickup(this);
    if (this->collected) {
        func_enmy_80067A40();
        Object_Kill(&this->obj, this->sfxSource);
    }

    if (this->timer_48 == 1) {
        Object_Kill(&this->obj, this->sfxSource);
    }
}

void ItemMeteoWarp_Update(ItemMeteoWarp* this) {
    Item_CheckBounds(this);
    if (this->state > 0) {
        if (this->state == 1) {
            this->obj.rot.z -= 10.0f;
            this->obj.pos.x += (gPlayer[this->playerNum].pos.x - this->obj.pos.x) * 0.3f;
            this->obj.pos.y += (gPlayer[this->playerNum].pos.y - this->obj.pos.y) * 0.3f;
            this->obj.pos.z += (gPlayer[this->playerNum].trueZpos - this->obj.pos.z) * 0.3f;
            this->width -= 5.0f;
            if (this->width < 0.0f) {
                this->width = 0.0f;
            }
        }

        this->unk_44 -= 10;
        if (this->unk_44 < 0) {
            Object_Kill(&this->obj, this->sfxSource);
        }
    } else {
        this->width = 100.0f;
        if (gRingPassCount < 0) {
            this->state = 2;
            this->unk_44 = 255;
        } else if (this->collected) {
            this->state = 1;
            this->unk_44 = 255;

            gPlayer[this->playerNum].meteoWarpTimer = 100;

            AUDIO_PLAY_SFX(gWarpRingSfx[gRingPassCount], gPlayer[0].sfxSource, 0);

            if (gRingPassCount == 0) {
                gPlayer[0].boostSpeed = 0.0f;
            }

            gRingPassCount++;
            if (gRingPassCount >= 7) {
                gPlayer[0].state = PLAYERSTATE_ENTER_WARP_ZONE;
                gPlayer[0].csState = 0;
                AUDIO_PLAY_SFX(NA_SE_WARP_HOLE, gDefaultSfxSource, 0);
                gMissionStatus = MISSION_WARP;
                gLeveLClearStatus[gCurrentLevel] = 1;
            }
        }
    }
}

void ItemCheckpoint_Update(ItemCheckpoint* this) {
    TeamId i;

    Item_CheckBounds(this);
    this->unk_58 -= this->unk_44;

    if (this->state > 0) {
        this->unk_44++;
        this->obj.pos.x += (gPlayer[this->playerNum].pos.x - this->obj.pos.x) * 0.3f;
        if (gPlayer[this->playerNum].form == FORM_LANDMASTER) {
            this->obj.pos.y += (gPlayer[this->playerNum].pos.y + 50.0f - this->obj.pos.y) * 0.3f;
        } else {
            this->obj.pos.y += (gPlayer[this->playerNum].pos.y - this->obj.pos.y) * 0.3f;
        }
        if (gPlayer[0].alternateView) {
            this->obj.pos.z += (gPlayer[this->playerNum].trueZpos - 200.0f - this->obj.pos.z) * 0.3f;
        } else {
            this->obj.pos.z += (gPlayer[this->playerNum].trueZpos - 100.0f - this->obj.pos.z) * 0.3f;
        }
        if (this->timer_48 == 0) {
            Math_SmoothStepToF(&this->width, 5.0f, 0.2f, 15.0f, 0.01f);
            Math_SmoothStepToF(&this->unk_50, 0.0f, 0.1f, 0.03f, 0.0f);
            Math_SmoothStepToF(&this->unk_54, 4.0f, 0.1f, 0.2f, 0.01f);
        }
        if (this->width <= 6.5f) {
            Object_Kill(&this->obj, this->sfxSource);
        }
    } else {
        this->unk_44 = 2;
        this->unk_50 = 1.0f;
        this->unk_54 = 1.0f;
        this->width = 100.0f;
        if (this->collected) {
            gPlayer[this->playerNum].heal = 128;
            this->state++;
            this->timer_48 = 15;
            gSavedGroundSurface = gGroundSurface;
            gSavedPathProgress = -this->obj.pos.z;
            gSavedPathProgress -= 250.0f;
            gSavedObjectLoadIndex = gObjectLoadIndex;
            gSavedZoSearchlightStatus = gMissedZoSearchlight;
            gSavedHitCount = gHitCount;
            for (i = TEAM_ID_FALCO; i <= TEAM_ID_PEPPY; i++) {
                gSavedTeamShields[i] = gTeamShields[i];
            }
            AUDIO_PLAY_SFX(NA_SE_CHECKPOINT, gDefaultSfxSource, 4);
        }
    }
}

void ItemRingCheck_Update(Item* this) {
    if (this->collected) {
        Object_Kill(&this->obj, this->sfxSource);
        gRingPassCount++;
    }
}

void ItemPathChange_Update(Item* this) {
    gLastPathChange = this->obj.id;

    if (gPlayer[0].state != PLAYERSTATE_ACTIVE) {
        Object_Kill(&this->obj, this->sfxSource);
    } else if (((gCurrentLevel == LEVEL_METEO) || (gCurrentLevel == LEVEL_SECTOR_X)) && (gLevelPhase == 1)) {
        gPlayer[0].state = PLAYERSTATE_LEVEL_COMPLETE;
        gPlayer[0].csState = 0;
        Object_Kill(&this->obj, this->sfxSource);
    } else if (gCurrentLevel == LEVEL_TRAINING) {
        gPlayer[0].state = PLAYERSTATE_START_360;
        gPlayer[0].csState = 0;
        Object_Kill(&this->obj, this->sfxSource);
    } else if (this->state == 0) {
        if (((this->obj.pos.z + gPathProgress) > -2700.0f) && (fabsf(this->obj.pos.x - gPlayer[0].pos.x) < 1000.0f)) {
            switch (this->obj.id) {
                case OBJ_ITEM_PATH_SPLIT_Y:
                case OBJ_ITEM_PATH_TURN_UP:
                case OBJ_ITEM_PATH_TURN_DOWN:
                    break;
                case OBJ_ITEM_PATH_SPLIT_X:
                    gPlayer[0].flags_228 = PFLAG_228_4 | PFLAG_228_5;
                    break;
                case OBJ_ITEM_PATH_TURN_LEFT:
                    gPlayer[0].flags_228 = PFLAG_228_5;
                    break;
                case OBJ_ITEM_PATH_TURN_RIGHT:
                    gPlayer[0].flags_228 = PFLAG_228_4;
                    break;
            }
        }

        if (this->collected) {
            Object_Kill(&this->obj, this->sfxSource);

            gPlayer[0].pathStep = 0.0f;
            gPlayer[0].pathChangeTimer = this->width * 0.05f;

            switch (this->obj.id) {
                case OBJ_ITEM_PATH_SPLIT_X:
                    if (this->obj.pos.x < gPlayer[0].pos.x) {
                        gPlayer[0].pathChangeYaw = -30.0f;
                        gPlayer[0].xPathTarget = gPlayer[0].xPath + this->width;
                    } else {
                        gPlayer[0].pathChangeYaw = 30.0f;
                        gPlayer[0].xPathTarget = gPlayer[0].xPath - this->width;
                    }
                    break;

                case OBJ_ITEM_PATH_TURN_LEFT:
                    gPlayer[0].pathChangeYaw = 30.0f;
                    gPlayer[0].xPathTarget = gPlayer[0].xPath - this->width;
                    break;

                case OBJ_ITEM_PATH_TURN_RIGHT:
                    gPlayer[0].pathChangeYaw = -30.0f;
                    gPlayer[0].xPathTarget = gPlayer[0].xPath + this->width;
                    break;

                case OBJ_ITEM_PATH_SPLIT_Y:
                    if (this->obj.pos.y < gPlayer[0].pos.y) {
                        gPlayer[0].pathChangePitch = 30.0f;
                        gPlayer[0].yPathTarget = gPlayer[0].yPath + this->width;
                    } else {
                        gPlayer[0].pathChangePitch = -30.0f;
                        gPlayer[0].yPathTarget = gPlayer[0].yPath - this->width;
                    }
                    break;

                case OBJ_ITEM_PATH_TURN_UP:
                    gPlayer[0].pathChangePitch = 30.0f;
                    gPlayer[0].yPathTarget = gPlayer[0].yPath + this->width;
                    break;

                case OBJ_ITEM_PATH_TURN_DOWN:
                    gPlayer[0].pathChangePitch = -30.0f;
                    gPlayer[0].yPathTarget = gPlayer[0].yPath - this->width;
                    break;
            }
        }
    }
}

void Sprite_UpdateDoodad(Sprite* this) {
    this->obj.rot.y =
        (Math_Atan2F(gPlayer[0].cam.eye.x - this->obj.pos.x, gPlayer[0].cam.eye.z - (this->obj.pos.z + gPathProgress)) *
         180.0f) /
        M_PI;
    if (this->destroy) {
        this->obj.status = OBJ_FREE;
        Effect_SpawnTimedSfxAtPos(&this->obj.pos, NA_SE_OB_EXPLOSION_S);
        switch (this->obj.id) {
            case OBJ_SPRITE_CO_POLE:
                func_effect_8007D074(this->obj.pos.x, this->obj.pos.y + 160.0f, this->obj.pos.z, 4.0f);
                break;
            default:
            case OBJ_SPRITE_TI_CACTUS:
                func_effect_8007D074(this->obj.pos.x, this->obj.pos.y + 96.0f, this->obj.pos.z, 5.0f);
                break;
        }
    }
}

void Object_Dying(s32 index, ObjectId objId) {
    switch (objId) {
        case OBJ_ACTOR_EVENT:
            ActorEvent_Dying(&gActors[index]);
            break;

        case OBJ_ACTOR_ALLRANGE:
            if (gCurrentLevel == LEVEL_VENOM_ANDROSS) {
                Andross_801888F4(&gActors[index]);
            } else {
                func_enmy2_800763A4(&gActors[index]);
            }
            break;

        case OBJ_ACTOR_ME_HOPBOT:
            func_enmy2_800763A4(&gActors[index]);
            break;

        case OBJ_ACTOR_ME_MORA:
            MeMora_Dying(&gActors[index]);
            break;

        case OBJ_ACTOR_ME_LASER_CANNON_1:
            Meteo_80187B08(&gActors[index]);
            break;

        case OBJ_ACTOR_ME_METEOR_1:
            Object_Kill(&gActors[index].obj, gActors[index].sfxSource);
            func_effect_8007D2C8(gActors[index].obj.pos.x, gActors[index].obj.pos.y, gActors[index].obj.pos.z, 20.0f);
            break;

        case OBJ_ACTOR_ME_METEOR_2:
            Object_Kill(&gActors[index].obj, gActors[index].sfxSource);
            func_effect_8007D2C8(gActors[index].obj.pos.x, gActors[index].obj.pos.y, gActors[index].obj.pos.z, 10.0f);
            Actor_Despawn(&gActors[index]);
            break;

        case OBJ_ACTOR_CO_SKIBOT:
            func_enmy_8006684C(&gActors[index]);
            break;

        case OBJ_BOSS_TI_GORAS:
            Titania_TiGoras_Dying(&gBosses[index]);
            break;

        case OBJ_ACTOR_TI_RASCO:
            Titania_TiRasco_Dying(&gActors[index]);
            break;

        case OBJ_BOSS_VE1_GOLEMECH:
            Venom1_Ve1Golemech_Dying(&gBosses[index]);
            break;
    }
}

void Actor_Move(Actor* this) {
    f32 var_fv0;

    this->obj.pos.x += this->vel.x;
    this->obj.pos.z += this->vel.z;
    this->obj.pos.y += this->vel.y;
    this->vel.y -= this->gravity;

    if (!gCullObjects || (this->obj.id == OBJ_ACTOR_TEAM_BOSS) ||
        ((gCurrentLevel == LEVEL_MACBETH) && (this->obj.id != OBJ_ACTOR_EVENT))) {
        return;
    }

    var_fv0 = 4000.0f;
    if ((this->obj.id == OBJ_ACTOR_ZO_DODORA) || (gCurrentLevel == LEVEL_MACBETH) ||
        ((this->obj.id == OBJ_ACTOR_EVENT) && (this->eventType == EVID_TI_GREAT_FOX))) {
        var_fv0 = 8000.0f;
    } else if (gPlayer[0].state == PLAYERSTATE_ENTER_WARP_ZONE) {
        var_fv0 = 100000.0f;
    }

    if (((gPlayer[0].cam.eye.z + this->info.cullDistance) < (this->obj.pos.z + gPathProgress)) ||
        ((this->obj.pos.z + gPathProgress) < -15000.0f) || (this->obj.pos.y < (gPlayer[0].yPath - var_fv0)) ||
        ((gPlayer[0].yPath + var_fv0) < this->obj.pos.y) || ((gPlayer[0].xPath + var_fv0) < this->obj.pos.x) ||
        (this->obj.pos.x < (gPlayer[0].xPath - var_fv0))) {
        Object_Kill(&this->obj, this->sfxSource);

        switch (this->obj.id) {
            case OBJ_ACTOR_ZO_DODORA:
                gZoDodoraWaypointCount = 0;
                break;

            case OBJ_ACTOR_TI_DESERT_CRAWLER:
                Titania_8018E3B0(this);
                break;

            case OBJ_ACTOR_ME_MORA:
                gMeMoraStatus[this->work_046] = 0;
                break;

            case OBJ_ACTOR_EVENT:
                if ((this->eventType >= EVID_200) && (this->eventType < EVID_300)) {
                    gMeMoraStatus[this->work_046] = 0;
                } else if ((this->eventType == EVID_SX_WARP_GATE) && (this->work_046 != 2)) {
                    gRingPassCount = -1;
                }
                break;

            case OBJ_ACTOR_ZO_RADARBUOY:
                gMissedZoSearchlight = true;
                break;
        }
    }
}

void Boss_Move(Boss* this) {
    this->obj.pos.x += this->vel.x;
    this->obj.pos.y += this->vel.y;
    this->obj.pos.z += this->vel.z;
    this->vel.y -= this->gravity;

    if (gCullObjects && ((this->obj.pos.z + gPathProgress) > (this->info.cullDistance - gPlayer[0].cam.eye.z))) {
        if (gPlayer[0].cam.eye.z) {} //! FAKE
        Object_Kill(&this->obj, this->sfxSource);
    }
}

void Scenery_Move(Scenery* this) {
    if (gPlayer[0].state == PLAYERSTATE_LEVEL_INTRO) {
        this->obj.pos.z += this->effectVel.z;
        if ((this->info.cullDistance * 1.5f) < this->obj.pos.z) {
            Object_Kill(&this->obj, this->sfxSource);
        }
    } else if ((gLevelMode == LEVELMODE_ON_RAILS) && (gBossActive != 2)) {
        f32 temp_fv0 = fabsf(this->obj.pos.x - gPlayer[0].cam.eye.x);
        f32 var_fa0 = 500.0f;

        if ((this->obj.id == OBJ_SCENERY_CO_HIGHWAY_1) || (this->obj.id == OBJ_SCENERY_CO_HIGHWAY_2)) {
            var_fa0 = 1000.0f;
        }
        temp_fv0 = ((temp_fv0 - var_fa0) < 0.0f) ? 0.0f : (temp_fv0 - var_fa0) * 1.7f;
        if ((fabsf(gPlayer[0].yRot_114) > 1.0f) || (gCurrentLevel == LEVEL_MACBETH)) {
            temp_fv0 = 0.0f;
        }
        temp_fv0 -= gPlayer[0].cam.eye.z;

        // @port: increase cullDistance by 50%.
        f32 portCulldistance = 1.5f;

        if (gCurrentLevel == LEVEL_TITANIA) {
            portCulldistance = 1.0f;
        }

        if (((this->info.cullDistance * portCulldistance) - temp_fv0) < (this->obj.pos.z + gPathProgress)) {
            Object_Kill(&this->obj, this->sfxSource);
        }
    }
}

void Sprite_Move(Sprite* this) {
    if (gCullObjects) {
        f32 temp_fv0 = fabsf(this->obj.pos.x - gPlayer[0].cam.eye.x);
        f32 var_fa0 = 500.0f;

        if (((this->obj.id == OBJ_SPRITE_FOG_SHADOW) &&
             ((this->sceneryId == OBJ_SCENERY_CO_HIGHWAY_1) || (this->sceneryId == OBJ_SCENERY_CO_HIGHWAY_2))) ||
            (this->obj.id == OBJ_SCENERY_CO_HIGHWAY_3)) {
            var_fa0 = 1000.0f;
        }

        temp_fv0 = ((temp_fv0 - var_fa0) < 0.0f) ? 0.0f * 1.7f : (temp_fv0 - var_fa0) * 1.7f;
        temp_fv0 -= gPlayer[0].cam.eye.z;

        if ((this->info.cullDistance - temp_fv0) < (this->obj.pos.z + gPathProgress)) {
            this->obj.status = OBJ_FREE;
        }
    }
}

void Effect_Move(Effect* this) {
    this->obj.pos.x += this->vel.x;
    this->obj.pos.y += this->vel.y;
    this->obj.pos.z += this->vel.z;

    if (gCullObjects) {
        if ((gPlayer[0].cam.eye.z + this->info.cullDistance) < (this->obj.pos.z + gPathProgress)) {
            Object_Kill(&this->obj, this->sfxSource);
        } else if ((fabsf(this->obj.pos.y - gPlayer[0].cam.eye.y) > 25000.0f) ||
                   (fabsf(this->obj.pos.x - gPlayer[0].cam.eye.x) > 25000.0f)) {
            Object_Kill(&this->obj, this->sfxSource);
        }
    }
}

void Item_Move(Item* this) {
    if (gCullObjects) {
        f32 temp = (0.0f - gPlayer[0].cam.eye.z);

        if ((this->info.cullDistance - temp) < (this->obj.pos.z + gPathProgress)) {
            Object_Kill(&this->obj, this->sfxSource);
            if ((this->obj.id == OBJ_ITEM_METEO_WARP) && (this->state == 0)) {
                gRingPassCount = -1;
            }
        }
    }
}

void Actor_Update(Actor* this) {
    s32 i;

    if (this->timer_0BC != 0) {
        this->timer_0BC--;
    }
    if (this->timer_0BE != 0) {
        this->timer_0BE--;
    }
    if (this->timer_0C0 != 0) {
        this->timer_0C0--;
    }
    if (this->timer_0C2 != 0) {
        this->timer_0C2--;
    }
    if (this->timer_0C6 != 0) {
        this->timer_0C6--;
    }

    if (gVersusMode) {
        for (i = 0; i < gCamCount; i++) {
            if (this->lockOnTimers[i] != 0) {
                if (!(gControllerHold[i].button & A_BUTTON)) {
                    this->lockOnTimers[i]--;
                }
                gChargeTimers[i] = 0;
            }
        }
    } else if (this->lockOnTimers[TEAM_ID_FOX] != 0) {
        bool rapidFire = CVarGetInteger("gRapidFire", 0) == 1;
        if (!(gControllerHold[gMainController].button & A_BUTTON) ||
            (rapidFire && (gControllerHold[gMainController].button & A_BUTTON))) {
            this->lockOnTimers[TEAM_ID_FOX]--;
        }
        gChargeTimers[0] = 0;
    }

    if (this->timer_0C4 != 0) {
        this->timer_0C4--;
    }

    switch (this->obj.status) {
        case OBJ_INIT: {
            CALL_CANCELLABLE_EVENT(ObjectInitEvent, OBJECT_TYPE_ACTOR, this) {
                this->obj.status = OBJ_ACTIVE;
                Object_Init(this->index, this->obj.id);
                if (this->obj.id != OBJ_ACTOR_ZO_RADARBUOY) {
                    Actor_Move(this);
                }
            }
            break;
        }

        case OBJ_ACTIVE: {
            CALL_CANCELLABLE_EVENT(ObjectUpdateEvent, OBJECT_TYPE_ACTOR, this) {
                Actor_Move(this);
                if ((this->obj.status != OBJ_FREE) && (this->info.action != NULL)) {
                    this->info.action(&this->obj);
                }
            }
            break;
        }

        case OBJ_DYING: {
            CALL_CANCELLABLE_EVENT(ObjectDestroyEvent, OBJECT_TYPE_ACTOR, this) {
                Actor_Move(this);
                if (this->obj.status != OBJ_FREE) {
                    Object_Dying(this->index, this->obj.id);
                }
                break;
            }
            break;
        }
    }
}

void Boss_Update(Boss* this) {
    if (this->timer_050 != 0) {
        this->timer_050--;
    }
    if (this->timer_052 != 0) {
        this->timer_052--;
    }
    if (this->timer_054 != 0) {
        this->timer_054--;
    }
    if (this->timer_056 != 0) {
        this->timer_056--;
    }
    if (this->timer_058 != 0) {
        this->timer_058--;
    }
    if (this->timer_05A != 0) {
        this->timer_05A--;
    }
    if (this->timer_05C != 0) {
        this->timer_05C--;
    }

    switch (this->obj.status) {
        case OBJ_INIT: {
            CALL_CANCELLABLE_EVENT(ObjectInitEvent, OBJECT_TYPE_BOSS, this) {
                this->obj.status = OBJ_ACTIVE;
                Object_Init(this->index, this->obj.id);
                Boss_Move(this);
            }
            break;
        }

        case OBJ_ACTIVE: {
            CALL_CANCELLABLE_EVENT(ObjectUpdateEvent, OBJECT_TYPE_BOSS, this) {
                Boss_Move(this);
                if ((this->obj.status != OBJ_FREE) && (this->info.action != NULL)) {
                    this->info.action(&this->obj);
                }
            }
            break;
        }

        case OBJ_DYING: {
            CALL_CANCELLABLE_EVENT(ObjectDestroyEvent, OBJECT_TYPE_BOSS, this) {
                Boss_Move(this);
                if (this->obj.status != OBJ_FREE) {
                    Object_Dying(this->index, this->obj.id);
                }
            }
            break;
        }
    }
}

void Scenery_Update(Scenery* this) {
    if (this->timer_4C != 0) {
        this->timer_4C--;
    }

    switch (this->obj.status) {
        case OBJ_INIT: {
            CALL_CANCELLABLE_EVENT(ObjectInitEvent, OBJECT_TYPE_SCENERY, this) {
                this->obj.status = OBJ_ACTIVE;
                Object_Init(this->index, this->obj.id);
                Scenery_Move(this);
            }
            break;
        }

        case OBJ_ACTIVE: {
            CALL_CANCELLABLE_EVENT(ObjectUpdateEvent, OBJECT_TYPE_SCENERY, this) {
                Scenery_Move(this);
                if (this->info.action != NULL) {
                    this->info.action(&this->obj);
                }
            }
            break;
        }
    }
}

void Sprite_Update(Sprite* this) {
    switch (this->obj.status) {
        case OBJ_INIT: {
            CALL_CANCELLABLE_EVENT(ObjectInitEvent, OBJECT_TYPE_SPRITE, this) {
                this->obj.status = OBJ_ACTIVE;
                Object_Init(this->index, this->obj.id);
                Sprite_Move(this);
            }
            break;
        }
        case OBJ_ACTIVE: {
            CALL_CANCELLABLE_EVENT(ObjectUpdateEvent, OBJECT_TYPE_SPRITE, this) {
                Sprite_Move(this);
                if (this->info.action != NULL) {
                    this->info.action(&this->obj);
                }
            }
            break;
        }

        case OBJ_DYING: {
            CALL_CANCELLABLE_EVENT(ObjectDestroyEvent, OBJECT_TYPE_SPRITE, this) {
                Sprite_Move(this);
                Object_Dying(this->index, this->obj.id);
            }
            break;
        }
    }
}

void Item_Update(Item* this) {
    if (this->timer_48 != 0) {
        this->timer_48--;
    }
    if (this->timer_4A != 0) {
        this->timer_4A--;
    }

    switch (this->obj.status) {
        case OBJ_INIT: {
            CALL_CANCELLABLE_EVENT(ObjectInitEvent, OBJECT_TYPE_ITEM, this) {
                this->obj.status = OBJ_ACTIVE;
                Object_Init(this->index, this->obj.id);
                Item_Move(this);
            }
            break;
        }

        case OBJ_ACTIVE: {
            CALL_CANCELLABLE_EVENT(ObjectUpdateEvent, OBJECT_TYPE_ITEM, this) {
                Item_Move(this);
                if (this->info.action != NULL) {
                    this->info.action(&this->obj);
                }
            }
            break;
        }
    }
}

void Effect_Update(Effect* this) {
    if (this->timer_50 != 0) {
        this->timer_50--;
    }

    switch (this->obj.status) {
        case OBJ_INIT: {
            CALL_CANCELLABLE_EVENT(ObjectInitEvent, OBJECT_TYPE_EFFECT, this) {
                this->obj.status = OBJ_ACTIVE;
                Object_Init(this->index, this->obj.id);
                Effect_Move(this);
            }
            /* fallthrough */
        }
        case OBJ_ACTIVE: {
            CALL_CANCELLABLE_EVENT(ObjectUpdateEvent, OBJECT_TYPE_EFFECT, this) {
                Effect_Move(this);
                if ((this->obj.status != OBJ_FREE) && (this->info.action != NULL)) {
                    this->info.action(&this->obj);
                }
            }
            break;
        }
    }
}

void TexturedLine_Update(TexturedLine* this) {
    Vec3f sp44;
    Vec3f sp38;
    f32 dx;
    f32 dy;
    f32 dz;

    if (this->timer != 0) {
        this->timer--;
    }

    dx = this->posAA.x - this->posBB.x;
    dy = this->posAA.y - this->posBB.y;
    dz = this->posAA.z - this->posBB.z;

    this->yRot = Math_Atan2F(dx, dz);
    this->xRot = -Math_Atan2F(dy, sqrtf(SQ(dx) + SQ(dz)));

    if (this->mode != 4) {
        this->zScale = sqrtf(SQ(dx) + SQ(dy) + SQ(dz));
    }

    if (gGameState == GSTATE_PLAY) {
        if (((this->mode == 1) || (this->mode == 101) || (this->mode == 50)) &&
            (gPlayer[0].state == PLAYERSTATE_ACTIVE) && (gPlayer[0].hitTimer == 0)) {
            Matrix_RotateX(gCalcMatrix, -this->xRot, MTXF_NEW);
            Matrix_RotateY(gCalcMatrix, -this->yRot, MTXF_APPLY);

            sp44.x = gPlayer[gPlayerNum].pos.x - this->posAA.x;
            sp44.y = gPlayer[gPlayerNum].pos.y - this->posAA.y;
            sp44.z = gPlayer[gPlayerNum].trueZpos - this->posAA.z;

            Matrix_MultVec3fNoTranslate(gCalcMatrix, &sp44, &sp38);

            sp38.x += this->posAA.x;
            sp38.y += this->posAA.y;
            sp38.z += this->posAA.z;

            if ((fabsf(sp38.x - this->posAA.x) < 30.0f) && (fabsf(sp38.y - this->posAA.y) < 30.0f) &&
                (sp38.z < this->posAA.z) && ((this->posAA.z - this->zScale) < sp38.z)) {
                if (gCurrentLevel == LEVEL_AQUAS) {
                    Player_ApplyDamage(&gPlayer[0], 0, 30);
                } else {
                    Player_ApplyDamage(&gPlayer[0], 0, 20);
                }
                if (this->mode < 100) {
                    this->mode = 0;
                }
            }
        }
        if (((this->posAA.z + gPathProgress) > 1000.0f) && (gLevelMode != LEVELMODE_ALL_RANGE)) {
            this->mode = 0;
        }

        if (((this->mode == 3) || (this->mode == 50)) && (this->timer == 0)) {
            this->mode = 0;
        }
    }
}

void TexturedLine_UpdateAll(void) {
    TexturedLine* texLine;
    s32 i;

    for (i = 0, texLine = gTexturedLines; i < ARRAY_COUNT(gTexturedLines); i++, texLine++) {
        if (texLine->mode != 0) {
            TexturedLine_Update(texLine);
        }
    }
}

void Object_Update(void) {
    s32 i;
    s32 pad;
    Scenery360* scenery360;
    Actor* actor;
    Boss* boss;
    Sprite* sprite;
    Scenery* scenery;
    Item* item;
    Effect* effect;

    gCullObjects = false;
    if ((gLevelMode == LEVELMODE_ON_RAILS) &&
        ((gPlayer[0].state == PLAYERSTATE_INIT) || (gPlayer[0].state == PLAYERSTATE_ACTIVE) ||
         (gPlayer[0].state == PLAYERSTATE_DOWN) || (gPlayer[0].state == PLAYERSTATE_ENTER_WARP_ZONE) ||
         (gPlayer[0].state == PLAYERSTATE_START_360) || (gPlayer[0].state == PLAYERSTATE_NEXT))) {
        gCullObjects = true;
    }
    if (gLevelMode != LEVELMODE_ALL_RANGE) {
        if ((gLoadLevelObjects != 0) && (gPlayer[0].state != PLAYERSTATE_LEVEL_INTRO)) {
            Object_LoadLevelObjects();
        }
        for (i = 0, scenery = gScenery; i < ARRAY_COUNT(gScenery); i++, scenery++) {
            if (scenery->obj.status != OBJ_FREE) {
                scenery->index = i;
                Scenery_Update(scenery);
            }
        }
    } else if (gVersusMode) {
        for (i = 0, scenery360 = gScenery360; i < 200; i++, scenery360++) {
            if ((scenery360->obj.status != OBJ_FREE) && (scenery360->obj.id == OBJ_SCENERY_VS_SPACE_JUNK_3)) {
                if ((i % 2) != 0) {
                    scenery360->obj.rot.y += 0.5f;
                } else {
                    scenery360->obj.rot.y -= 0.5f;
                }
            }
        }
    }

    for (i = 0, sprite = &gSprites[0]; i < ARRAY_COUNT(gSprites); i++, sprite++) {
        if (sprite->obj.status != OBJ_FREE) {
            sprite->index = i;
            Sprite_Update(sprite);
        }
    }

    for (i = 0, boss = &gBosses[0]; i < ARRAY_COUNT(gBosses); i++, boss++) {
        if (boss->obj.status != OBJ_FREE) {
            boss->index = i;
            Boss_Update(boss);
        }
    }

    for (i = 0, actor = &gActors[0]; i < ARRAY_COUNT(gActors); i++, actor++) {
        if (actor->obj.status != OBJ_FREE) {
            actor->index = i;
            Actor_Update(actor);
        }
    }

    for (i = 0, item = &gItems[0]; i < ARRAY_COUNT(gItems); i++, item++) {
        if (item->obj.status != OBJ_FREE) {
            item->index = i;
            Item_Update(item);
        }
    }

    for (i = 0, effect = &gEffects[0]; i < ARRAY_COUNT(gEffects); i++, effect++) {
        if (effect->obj.status != OBJ_FREE) {
            effect->index = i;
            Effect_Update(effect);
        }
    }

    TexturedLine_UpdateAll();

    for (i = 0; i < ARRAY_COUNT(D_enmy_Timer_80161670); i++) {
        if (D_enmy_Timer_80161670[i] != 0) {
            D_enmy_Timer_80161670[i]--;
        }
    }
}
