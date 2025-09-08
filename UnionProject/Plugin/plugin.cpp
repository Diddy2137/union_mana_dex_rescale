#define gouthic2 //gouthic1
#include "plugin.h"
#include "UnionAfx.h"
#include <../UnionSDK/console/Console.h>
#include <sstream>

using namespace Common;
using namespace UnionCore;
#ifdef gouthic1
using namespace Gothic_I_Classic;
#include <../Gothic_I_Classic/API/oNpc.h>
#include <../Gothic_I_Classic/API/oItem.h>
#include <../Gothic_I_Classic/API/zOption.h>
#endif
#ifdef gouthic2
using namespace Gothic_II_Addon;
#include <../Gothic_II_Addon/API/oNpc.h>
#include <../Gothic_II_Addon/API/oItem.h>
#include <../Gothic_II_Addon/API/zOption.h>
#endif

#ifndef NPC_ATR_DEXTERITY
#define NPC_ATR_DEXTERITY 5
#endif
#ifndef NPC_ATR_MANA
#define NPC_ATR_MANA 3
#endif
#ifndef NPC_WEAPON_BOW
#define NPC_WEAPON_BOW 6
#endif
#ifndef NPC_WEAPON_CBOW
#define NPC_WEAPON_CBOW 5
#endif
#ifndef NPC_WEAPON_MAGIC
#define NPC_WEAPON_MAGIC 7
#endif

// override trough menu
static float g_DexScale = 2.0f;
static float g_ManaScale = 2.0f;
static bool spierdalaj = false;
static bool SneakAmbush = false; //sneak mode for ranged
static bool DontKill = false; //dont kill npcs

static void LoadConfig() {
    if (!zoptions) return;

    static const float scaleSteps[] = {
        2.00f,1.75f,1.50f,1.25f,
        1.0f,0.75f,0.50f,0.25f,0.00f
    };

    int dexIdx = zoptions->ReadInt("DamageScaling", "DexScale", 1);
    int manaIdx = zoptions->ReadInt("DamageScaling", "ManaScale", 1);

    if (dexIdx < 0) dexIdx = 0;
    if (dexIdx > 8) dexIdx = 8;
    if (manaIdx < 0) manaIdx = 0;
    if (manaIdx > 8) manaIdx = 8;

    g_DexScale = scaleSteps[dexIdx];
    g_ManaScale = scaleSteps[manaIdx];

    spierdalaj = zoptions->ReadBool("DamageScaling", "DebugOutput", false);
	SneakAmbush = zoptions->ReadBool("DamageScaling", "SneakAmbush", false);
	DontKill = zoptions->ReadBool("DamageScaling", "DontKill", false);

    cmd << CMD_CYAN_INT << "[Union:Config] DexScale=" << g_DexScale
        << ", ManaScale=" << g_ManaScale << endl;
}




static inline oCNpc* GetHeroG1() {
    return oCNpc::player;
}
// weapon mode
static inline int GetHeroWeaponMode(const oCNpc* hero) {
    return const_cast<oCNpc*>(hero)->GetWeaponMode();
}



static inline int CalcDexBonus(const oCNpc* npc) {
    if (!npc) return 0;
    int dex = const_cast<oCNpc*>(npc)->GetAttribute(NPC_ATR_DEXTERITY);
    if (dex < 0) dex = 0;
    int bonus = static_cast<int>(dex * g_DexScale);
    //dex debug
    if (spierdalaj == 1) {
        cmd << CMD_YELLOW_INT << "[Union:Ranged] Hero DEX=" << dex << endl;
        cmd << CMD_GREEN_INT << "[Union:Ranged] Bonus=" << bonus << endl;
        return bonus;
    }
    else
        return bonus;
}

static inline int CalcManaBonus(const oCNpc* npc) {
    if (!npc) return 0;
    int mana = const_cast<oCNpc*>(npc)->GetAttribute(NPC_ATR_MANA);
    if (mana < 0) mana = 0;
    int bonus = static_cast<int>(mana * g_ManaScale);
    //mana debug
    if (spierdalaj == 1) {
        cmd << CMD_YELLOW_INT << "[Union:Magic] Hero MANA=" << mana << endl;
        cmd << CMD_GREEN_INT << "[Union:Magic] Bonus=" << bonus << endl;
        return bonus;
    }
    else
		return bonus;
}

// -------------------------------------------------
// Signatures
// -------------------------------------------------
using TOnDamage = void(__fastcall*)(oCNpc* _this, void* edx, oCNpc::oSDamageDescriptor&);
using TOnDamageAnim = void(__fastcall*)(oCNpc* _this, void* edx, oCNpc::oSDamageDescriptor&);

static TOnDamage     Ivk_oCNpc_OnDamage = nullptr;
static TOnDamageAnim Ivk_oCNpc_OnDamage_Anim = nullptr;

// -------------------------------------------------
// Detect which attribute a weapon scales with
// -------------------------------------------------
static inline bool WeaponRequiresAttr(const oCItem* w, int atr) {
    if (!w) return false;
    // oItem/oCItem  cond_atr[] + cond_value[]
    for (int i = 0; i < 3; ++i) {
        if (w->cond_atr[i] == atr && w->cond_value[i] > 0)
            return true;
    }
    return false;
}

static inline int GetWeaponScalingAttr(const oCItem* w) {
    if (!w) return NPC_ATR_STRENGTH;               // fallback
    if (WeaponRequiresAttr(w, NPC_ATR_DEXTERITY)) return NPC_ATR_DEXTERITY;
    if (WeaponRequiresAttr(w, NPC_ATR_MANA))      return NPC_ATR_MANA;
    return NPC_ATR_STRENGTH;                      // STR → no bonus
}


// -------------------------------------------------
// Core bonus application (shared by both hooks)
// -------------------------------------------------
static void ApplyStatBonus(oCNpc* victim, oCNpc::oSDamageDescriptor& desc) {
    oCNpc* attacker = desc.pNpcAttacker;
    oCNpc* hero = GetHeroG1();
    if (!(attacker && attacker == hero)) return;

    int mode = GetHeroWeaponMode(hero);
    oCItem* weapon = hero->GetWeapon();
    int bonus = 0;

    if (mode == NPC_WEAPON_BOW || mode == NPC_WEAPON_CBOW) {
        bonus = CalcDexBonus(hero);
    }
    else if (mode == NPC_WEAPON_MAGIC) {
        bonus = CalcManaBonus(hero);
    }
    else if (weapon) {
        // Melee (or other) → decide by the weapon’s actual requirement
        const int scalingAtr = GetWeaponScalingAttr(weapon);
        if (scalingAtr == NPC_ATR_DEXTERITY)
            bonus = CalcDexBonus(hero);
        else if (scalingAtr == NPC_ATR_MANA)
            bonus = CalcManaBonus(hero);
        // STR → no scaling
        if (spierdalaj == 1) {
            cmd << CMD_CYAN_INT << "[Union] Weapon req atr="
                << scalingAtr << " (4=STR,5=DEX,2=MANA)" << endl;
        }
    }

    if (bonus <= 0) return;

    // Inject bonus
    int idxCount = static_cast<int>(sizeof(desc.aryDamage) / sizeof(desc.aryDamage[0]));
    int maxIdx = 0;
    for (int i = 1; i < idxCount; ++i)
        if (desc.aryDamage[i] > desc.aryDamage[maxIdx]) maxIdx = i;

    int old = desc.aryDamage[maxIdx];
    desc.aryDamage[maxIdx] += bonus;
    desc.fDamageTotal += bonus;
    desc.fDamageReal += bonus;
    desc.fDamageEffective += bonus;

    if (spierdalaj == 1) {
        cmd << CMD_CYAN_INT << "[Union] Mode=" << mode
            << " old=" << old
            << " new=" << desc.aryDamage[maxIdx]
            << " (+ " << bonus << ")" << endl;
    }

    auto funnelDamageTo = [&](int idxTarget) {
        float total = desc.fDamageTotal;
        for (int i = 0; i < oEDamageIndex_MAX; ++i) desc.aryDamage[i] = 0;
        desc.aryDamage[idxTarget] = static_cast<unsigned long>(total);
        desc.fDamageTotal = total;
        };

    const bool isRanged = (mode == NPC_WEAPON_BOW || mode == NPC_WEAPON_CBOW);
    const bool isMelee = (mode == NPC_WEAPON_1HS || mode == NPC_WEAPON_2HS || mode == NPC_WEAPON_DAG);
    const bool isMagic = (mode == NPC_WEAPON_MAG);
	//sneak mode deal 2x damage when sneaking if npc can't see you
    if (SneakAmbush && isRanged) {
        const bool isSneaking = (hero->GetBodyState() == BS_SNEAK);
        const bool targetSeesHero = victim && victim->CanSee(hero, 0) != 0;
        if (isSneaking && !targetSeesHero) {
            for (int i = 0; i < oEDamageIndex_MAX; ++i) desc.aryDamage[i] *= 2;
            desc.fDamageTotal *= 2.0f;
            desc.fDamageReal *= 2.0f;
            desc.fDamageEffective *= 2.0f;
        }
    }
	//don't kill npcs
    if (DontKill) {
#ifdef gouthic2
        desc.bDamageDontKill = 1;
#endif //g1 dont have this
        funnelDamageTo(oEDamageIndex_Blunt);
        desc.enuModeDamage = oEDamageType_Blunt;
    }


    
}


// -------------------------------------------------------
// Hook: oCNpc::OnDamage(oSDamageDescriptor&) — preferred//
// -------------------------------------------------------
void __fastcall Hook_oCNpc_OnDamage(oCNpc* _this, void* edx, oCNpc::oSDamageDescriptor& desc) {
    if (!_this) return;
    ApplyStatBonus(_this, desc);
    if (Ivk_oCNpc_OnDamage)
        Ivk_oCNpc_OnDamage(_this, edx, desc);
}

// -----------------------------------------------------------
// Hook: oCNpc::OnDamage_Anim(oSDamageDescriptor&) — fallback broken //
// -----------------------------------------------------------
void __fastcall Hook_oCNpc_OnDamage_Anim(oCNpc* _this, void* edx, oCNpc::oSDamageDescriptor& desc) {
    if (!_this) return;
    ApplyStatBonus(_this, desc);
    if (Ivk_oCNpc_OnDamage_Anim)
        Ivk_oCNpc_OnDamage_Anim(_this, edx, desc);
}

// -------------------------------------------------
// Hook                                            |
// -------------------------------------------------
struct InitHooks_DamagePipeline {
    InitHooks_DamagePipeline() {
        bool hooked = false;
        cmd << CMD_CYAN << "[HOOK Start]";
        if (!hooked) {
            auto* pHook = &UnionCore::CreateHookByName(
                oCNpc::OnDamage,
                (TOnDamage)&Hook_oCNpc_OnDamage,
                Hook_CallPatch
            );
            if (pHook && pHook->GetSourceAddress()) {
                Ivk_oCNpc_OnDamage = reinterpret_cast<TOnDamage>(pHook->GetSourceAddress());
                hooked = true;
				cmd << CMD_GREEN << "[HOOK OK]" << endl;
            }
        }

        if (!hooked) {
            auto* pHookAnim = &UnionCore::CreateHookByName(
                oCNpc::OnDamage_Anim,
                (TOnDamageAnim)&Hook_oCNpc_OnDamage_Anim,
                Hook_CallPatch
            );
            if (pHookAnim && pHookAnim->GetSourceAddress()) {
                Ivk_oCNpc_OnDamage_Anim = reinterpret_cast<TOnDamageAnim>(pHookAnim->GetSourceAddress());
                hooked = true;
                cmd << CMD_YELLOW << "[HOOK Fallback] oCNpc::OnDamage_Anim" << endl;
            }
        }
    }
} g_InitHooks_DamagePipeline;

// -------------------------------------------------
// entry points
// -------------------------------------------------
cexport void Game_Init() { LoadConfig(); }
cexport void Game_Entry() {}
cexport void Game_Exit() {}
cexport void Game_PreLoop() {}
cexport void Game_Loop() {}
cexport void Game_PostLoop() {}
cexport void Game_MenuLoop() {}
cexport void Game_SaveBegin() {}
cexport void Game_SaveEnd() {}
void LoadBegin() {}
void LoadEnd() {}
cexport void Game_LoadBegin_NewGame() { LoadBegin(); }
cexport void Game_LoadEnd_NewGame() { LoadEnd(); }
cexport void Game_LoadBegin_SaveGame() { LoadBegin(); }
cexport void Game_LoadEnd_SaveGame() { LoadEnd(); }
cexport void Game_LoadBegin_ChangeLevel() { LoadBegin(); }
cexport void Game_LoadEnd_ChangeLevel() { LoadEnd(); }
cexport void Game_LoadBegin_Trigger() {}
cexport void Game_LoadEnd_Trigger() {}
cexport void Game_Pause() {}
cexport void Game_Unpause() {}
cexport void Game_DefineExternals() {}
cexport void Game_ApplyOptions() {
    LoadConfig();
    // Force re-read of options when menu changes are made
    if (zoptions) {
        g_DexScale = zoptions->ReadReal("DamageScaling", "DexScale", 0.25f);
        g_ManaScale = zoptions->ReadReal("DamageScaling", "ManaScale", 0.25f);
        spierdalaj = zoptions->ReadInt("DamageScaling", "DebugOutput", false);
		SneakAmbush = zoptions->ReadBool("DamageScaling", "SneakAmbush", false);
		DontKill = zoptions->ReadBool("DamageScaling", "DontKill", false);
    }
}
