/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2006-2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "greenstone_village.h"
#include "ScriptedCreature.h"
#include "Scenario.h"
#include "ScenarioMgr.h"
#include "AchievementMgr.h"

enum scenario_information
{
    // Stage 1 of 5
    Scenario_Progress_La_Liupo = 19204, //20981
    Scenario_Progress_Mayor_Lin = 19199,
    Scenario_Progress_Meila = 19200,
    Scenario_Progress_Portly_Shung = 19201,
    Scenario_Progress_Scribe_Rinji = 19202,
    Scenario_Progress_Swan = 19203,
};

class mob_trigger_la_liupo : public CreatureScript
{
public:
    mob_trigger_la_liupo() : CreatureScript("mob_trigger_la_liupo") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->GetEntry() == 61343 || creature->GetEntry() == 61342)
        {
			for (Map::PlayerList::const_iterator itr = instance->GetPlayers().begin(); itr != instance->GetPlayers().end(); ++itr)
			{
				Player* player = itr->getSource();
				if (!player)
					continue;

				player->UpdateAchievementCriteria(CRITERIA_TYPE_SCRIPT_EVENT_3, 42260);

			}

           player->SEND_GOSSIP_MENU(15073, creature->GetGUID());
        }
        return true;
    }
};


void AddSC_greenstone_village()
{
    new mob_trigger_la_liupo();
}
