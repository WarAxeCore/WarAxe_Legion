/*
░██╗░░░░░░░██╗░█████╗░██████╗░░█████╗░██╗░░██╗███████╗░█████╗░░█████╗░██████╗░███████╗
░██║░░██╗░░██║██╔══██╗██╔══██╗██╔══██╗╚██╗██╔╝██╔════╝██╔══██╗██╔══██╗██╔══██╗██╔════╝
░╚██╗████╗██╔╝███████║██████╔╝███████║░╚███╔╝░█████╗░░██║░░╚═╝██║░░██║██████╔╝█████╗░░
░░████╔═████║░██╔══██║██╔══██╗██╔══██║░██╔██╗░██╔══╝░░██║░░██╗██║░░██║██╔══██╗██╔══╝░░
░░╚██╔╝░╚██╔╝░██║░░██║██║░░██║██║░░██║██╔╝╚██╗███████╗╚█████╔╝╚█████╔╝██║░░██║███████╗
░░░╚═╝░░░╚═╝░░╚═╝░░╚═╝╚═╝░░╚═╝╚═╝░░╚═╝╚═╝░░╚═╝╚══════╝░╚════╝░░╚════╝░╚═╝░░╚═╝╚══════╝

(c) Copyright WarAxeCore_Legion 2020
*/

#include "ScriptedCreature.h"
#include "LFGMgr.h"

//Npc Information: https://wow.gamepedia.com/Lorewalker_Fu

class npc_lorewalker_fu : public CreatureScript
{
public:
	npc_lorewalker_fu() : CreatureScript("npc_lorewalker_fu") { }

	bool OnGossipHello(Player* player, Creature* creature) override
	{
		player->ADD_GOSSIP_ITEM(0, "Tell me of Greenstone Village.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        player->ADD_GOSSIP_ITEM(0, "Tell me of Unga Ingoo.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
        player->ADD_GOSSIP_ITEM(0, "Tell me of Crypt of Forgotten Kings.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
		player->ADD_GOSSIP_ITEM(0, "Tell me of the Arena of Annihilation", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
        player->ADD_GOSSIP_ITEM(0, "Tell me the tale of a Brewing Storm.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
        player->ADD_GOSSIP_ITEM(0, "Tell me of the Brewmoon Festival.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6);
        player->ADD_GOSSIP_ITEM(0, "Tell me of Theramore's Fall.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 7);
        player->ADD_GOSSIP_ITEM(0, "Tell me of the Assault on Zan'vess.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 8);
        player->ADD_GOSSIP_ITEM(0, "Tell me of the Dagger in the Dark that felled Vol'jin", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 9);
        player->ADD_GOSSIP_ITEM(0, "Tell me of how Varian and Tyrande learned a Little Patience.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 10);
        player->ADD_GOSSIP_ITEM(0, "Tell me of the defense of Domination Point", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 11);
        player->ADD_GOSSIP_ITEM(0, "Tell me of the Battle on the High Seas.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 12);
        player->ADD_GOSSIP_ITEM(0, "Tell me of the unearthing of the Dark Heart of Pandaria", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 13);
        player->ADD_GOSSIP_ITEM(0, "Tell me of the Secrets of Ragefire.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 14);
        player->ADD_GOSSIP_ITEM(0, "Tell me the tale of Blood in the Snow.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 15);

		player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());

		return true;
	}

	bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
	{
		if (!player)
			return false;

		if (action == GOSSIP_ACTION_INFO_DEF + 4)
		{
			std::set<uint32> Slot_s;
			Slot_s.insert(511);
			sLFGMgr->JoinLfg(player, player->GetSpecializationRoleMaskForGroup(), Slot_s);
		}

		player->CLOSE_GOSSIP_MENU();

		return true;
	}
};

void AddSC_npc_lorewalker_fu()
{
	new npc_lorewalker_fu();
}