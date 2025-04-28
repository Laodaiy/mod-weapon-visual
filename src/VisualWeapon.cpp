/*
** Made by Rochet2(Eluna)
** Rewritten by Poszer & Talamortis https://github.com/poszer/ & https://github.com/talamortis/
** AzerothCore 2019 http://www.azerothcore.org/
** Cleaned and made into a module by Micrah https://github.com/milestorme/
*/

#include "ScriptMgr.h"
#include "Player.h"
#include "Chat.h"
#include "ScriptedGossip.h"
#include "Config.h"
#include "Chat.h"

using namespace std;

enum VisualWeaponsGossip
{
    VIS_DEFAULT_MESSAGE         = 907,
    VIS_GOSSIP_MAIN_MENU_ACTION = 100,
    VIS_GOSSIP_MAIN_HAND_ACTION = 200,
    VIS_GOSSIP_OFF_HAND_ACTION  = 300,
    VIS_GOSSIP_CLOSE_ACTION     = 400
};

struct VisualData
{
    uint32 Menu;
    uint32 Submenu;
    uint32 Icon;
    uint32 Id;
    string Name;
};

VisualData vData[] =
{
    { 1, VIS_GOSSIP_MAIN_MENU_ACTION, GOSSIP_ICON_TALK, 0, "返回" },
    { 1, 2, GOSSIP_ICON_INTERACT_1, 0, "下一页" },
    { 1, 0, GOSSIP_ICON_BATTLE, 3789, "狂暴" },
    { 1, 0, GOSSIP_ICON_BATTLE, 3854, "法术强度 Ⅳ" },
    { 1, 0, GOSSIP_ICON_BATTLE, 3273, "死亡霜冻" },
    { 1, 0, GOSSIP_ICON_BATTLE, 3225, "斩杀" },
    { 1, 0, GOSSIP_ICON_BATTLE, 3870, "吸血" },
    { 1, 0, GOSSIP_ICON_BATTLE, 1899, "邪恶武器" },
    { 1, 0, GOSSIP_ICON_BATTLE, 2674, "魔法激荡" },
    { 1, 0, GOSSIP_ICON_BATTLE, 2675, "作战专家" },
    { 1, 0, GOSSIP_ICON_BATTLE, 2671, "奥火法强" },
    { 1, 0, GOSSIP_ICON_BATTLE, 2672, "暗冰法强" },
    { 1, 0, GOSSIP_ICON_BATTLE, 3365, "裂刃符文" },
    { 1, 0, GOSSIP_ICON_BATTLE, 2673, "猫鼬" },
    { 1, 0, GOSSIP_ICON_BATTLE, 2343, "法术强度 I" },

    { 2, VIS_GOSSIP_MAIN_MENU_ACTION, GOSSIP_ICON_TALK, 0, "返回" },
    { 2, 3, GOSSIP_ICON_INTERACT_1, 0, "下一页" },
    { 2, 1, GOSSIP_ICON_INTERACT_1, 0, "上一页" },
    { 2, 0, GOSSIP_ICON_BATTLE, 425, "黑庙傀儡" },
    { 2, 0, GOSSIP_ICON_BATTLE, 3855, "法术强度 III" },
    { 2, 0, GOSSIP_ICON_BATTLE, 1894, "冰冷武器" },
    { 2, 0, GOSSIP_ICON_BATTLE, 1103, "敏捷" },
    { 2, 0, GOSSIP_ICON_BATTLE, 1898, "生命偷取" },
    { 2, 0, GOSSIP_ICON_BATTLE, 3345, "大地生命" },
    { 2, 0, GOSSIP_ICON_BATTLE, 1743, "MHTest02" },
    { 2, 0, GOSSIP_ICON_BATTLE, 3093, "亡灵攻强" },
    { 2, 0, GOSSIP_ICON_BATTLE, 1900, "十字军" },
    { 2, 0, GOSSIP_ICON_BATTLE, 3846, "法术强度 II" },
    { 2, 0, GOSSIP_ICON_BATTLE, 1606, "攻强" },
    { 2, 0, GOSSIP_ICON_BATTLE, 283, "风怒" },
    { 2, 0, GOSSIP_ICON_BATTLE, 1, "石化" },

    { 3, VIS_GOSSIP_MAIN_MENU_ACTION, GOSSIP_ICON_TALK, 0, "返回" },
    { 3, 2, GOSSIP_ICON_INTERACT_1, 0, "上一页" },
    { 3, 0, GOSSIP_ICON_BATTLE, 3265, "神圣武器" },
    { 3, 0, GOSSIP_ICON_BATTLE, 2, "冰封" },
    { 3, 0, GOSSIP_ICON_BATTLE, 3, "火舌" },
    { 3, 0, GOSSIP_ICON_BATTLE, 3266, "正义武器" },
    { 3, 0, GOSSIP_ICON_BATTLE, 1903, "精神" },
    { 3, 0, GOSSIP_ICON_BATTLE, 13, "磨快" },
    { 3, 0, GOSSIP_ICON_BATTLE, 26, "冰霜之油" },
    { 3, 0, GOSSIP_ICON_BATTLE, 7, "致命毒药" },
    { 3, 0, GOSSIP_ICON_BATTLE, 803, "灼热武器" },
    { 3, 0, GOSSIP_ICON_BATTLE, 1896, "武器伤害" },
    { 3, 0, GOSSIP_ICON_BATTLE, 2666, "智力" },
    { 3, 0, GOSSIP_ICON_BATTLE, 25, "暗影之油" },
};

class VisualWeaponNPC : public CreatureScript
{
public:
    VisualWeaponNPC() : CreatureScript("npc_visualweapon") { }

    bool MainHand;

    void SetVisual(Player* player, uint32 visual_id)
    {
        uint8 slot = MainHand ? EQUIPMENT_SLOT_MAINHAND : EQUIPMENT_SLOT_OFFHAND;

        Item* item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);

        if (!item)
        {
            ChatHandler(player->GetSession()).PSendSysMessage("没有可以改变视觉效果的装备。");
            return;
        }

        const ItemTemplate* itemTemplate = item->GetTemplate();
        if (itemTemplate->Class != ITEM_CLASS_WEAPON)
            return;

        if (itemTemplate->SubClass == ITEM_SUBCLASS_WEAPON_BOW ||
            itemTemplate->SubClass == ITEM_SUBCLASS_WEAPON_GUN ||
            itemTemplate->SubClass == ITEM_SUBCLASS_WEAPON_obsolete ||
            itemTemplate->SubClass == ITEM_SUBCLASS_WEAPON_THROWN ||
            itemTemplate->SubClass == ITEM_SUBCLASS_WEAPON_SPEAR ||
            itemTemplate->SubClass == ITEM_SUBCLASS_WEAPON_CROSSBOW ||
            itemTemplate->SubClass == ITEM_SUBCLASS_WEAPON_WAND ||
            itemTemplate->SubClass == ITEM_SUBCLASS_WEAPON_FISHING_POLE)
            return;

        player->SetUInt16Value(PLAYER_VISIBLE_ITEM_1_ENCHANTMENT + (item->GetSlot() * 2), 0, visual_id);
        CharacterDatabase.Execute("REPLACE into `mod_weapon_visual_effect` (`item_guid`, `enchant_visual_id`) VALUES ('{}', '{}')", item->GetGUID().GetCounter(), visual_id);
    }

    void GetMenu(Player* player, Creature* creature, uint32 menuId)
    {
        for (uint8 i = 0; i < (sizeof(vData) / sizeof(*vData)); i++)
        {
            if (vData[i].Menu == menuId)
                AddGossipItemFor(player, vData[i].Icon, vData[i].Name, GOSSIP_SENDER_MAIN, i);
        }

        player->PlayerTalkClass->SendGossipMenu(DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
    }

    void GetMainMenu(Player* player, Creature* creature)
    {
        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|TInterface/PaperDoll/UI-PaperDoll-Slot-MainHand:40:40:-18|t主手", GOSSIP_SENDER_MAIN, VIS_GOSSIP_MAIN_HAND_ACTION);
        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|TInterface/PaperDoll/UI-PaperDoll-Slot-SecondaryHand:40:40:-18|t副手", GOSSIP_SENDER_MAIN, VIS_GOSSIP_OFF_HAND_ACTION);
        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|TInterface/PaperDollInfoFrame/UI-GearManager-Undo:40:40:-18|t没事了", GOSSIP_SENDER_MAIN, VIS_GOSSIP_CLOSE_ACTION);

        player->PlayerTalkClass->SendGossipMenu(VIS_DEFAULT_MESSAGE, creature->GetGUID());
    }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        GetMainMenu(player, creature);
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
    {
        player->PlayerTalkClass->ClearMenus();

        switch (action)
        {
            case VIS_GOSSIP_MAIN_HAND_ACTION:
                MainHand = true;
                GetMenu(player, creature, 1);
                return true;

            case VIS_GOSSIP_OFF_HAND_ACTION:
                MainHand = false;
                GetMenu(player, creature, 1);
                return true;

            case VIS_GOSSIP_CLOSE_ACTION:
                CloseGossipMenuFor(player);
                return false;
        }

        uint32 menuData = vData[action].Submenu;

        if (menuData == VIS_GOSSIP_MAIN_MENU_ACTION)
        {
            GetMainMenu(player, creature);
            return true;
        }
        else if (menuData == 0)
        {
            SetVisual(player, vData[action].Id);
            menuData = vData[action].Menu;
        }

        GetMenu(player, creature, menuData);
        return true;
    }
};

class VisualWeaponPlayer : public PlayerScript
{
public:
    VisualWeaponPlayer() : PlayerScript("VisualWeaponPlayer")
    {
        // Delete unused rows from DB table
        CharacterDatabase.Execute("DELETE FROM `mod_weapon_visual_effect` WHERE NOT EXISTS(SELECT 1 FROM item_instance WHERE `mod_weapon_visual_effect`.item_guid = item_instance.guid)");
    }

    void GetVisual(Player* player)
    {
        if (!player)
            return;

        Item* pItem;

        // We need to query the DB to get item
        QueryResult result = CharacterDatabase.Query("SELECT item_guid, enchant_visual_id FROM `mod_weapon_visual_effect` WHERE item_guid IN(SELECT guid FROM item_instance WHERE owner_guid = '{}')", player->GetGUID().GetCounter());

        if (!result)
            return;

        // Now we have query the DB we need to get the fields.
        do
        {
            Field* fields = result->Fetch();
            uint32 item_guid = fields[0].Get<uint32>();
            uint32 visual = fields[1].Get<uint32>();

            // Lets loop to check item by pos
            for (int i = EQUIPMENT_SLOT_MAINHAND; i <= EQUIPMENT_SLOT_OFFHAND; ++i)
            {
                pItem = player->GetItemByPos(255, i);

                if (pItem && pItem->GetGUID().GetCounter() == item_guid)
                {
                    player->SetUInt16Value(PLAYER_VISIBLE_ITEM_1_ENCHANTMENT + (pItem->GetSlot() * 2), 0, visual);
                }
            }
        } while (result->NextRow());
    }

    // if Player has item in bag and re-equip it lets check for enchant
    void OnPlayerEquip(Player* player, Item* /*item*/, uint8 /*bag*/, uint8 /*slot*/, bool /*update*/) override
    {
        GetVisual(player);
    }

    void OnPlayerLogin(Player* player) override
    {
        GetVisual(player);

        if(sConfigMgr->GetOption<bool>("VisualWeapon.AnnounceEnable", true))
            ChatHandler(player->GetSession()).SendSysMessage("服务器已开启 |cff4CFF00武器光效|r 模块.");
    }
};

class VisualWeaponWorld : public WorldScript
{
public:
    VisualWeaponWorld() : WorldScript("VisualWeaponWorld") {}

    void OnStartup() override
    {
        // Delete unused rows from DB table
        CharacterDatabase.DirectExecute("DELETE FROM `mod_weapon_visual_effect` WHERE NOT EXISTS(SELECT 1 FROM item_instance WHERE `mod_weapon_visual_effect`.item_guid = item_instance.guid)");
    }

};

void AddVisualWeaponScripts()
{
    new VisualWeaponPlayer();
    new VisualWeaponWorld();
    new VisualWeaponNPC();
}
