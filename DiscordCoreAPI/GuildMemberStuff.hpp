// GuildMemberStuff.hpp - Header for the guild member related stuff.
// May 13, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _GUILD_MEMBER_STUFF_
#define _GUILD_MEMBER_STUFF_

#include "pch.h"
#include "DiscordDataStructs.hpp"

namespace DiscordCoreAPI {

	class GuildMemberManager;

	class Guild;

	class GuildMember{
	public:
		DiscordCoreInternal::GuildMemberData data;
		Guild* guild{ nullptr };
		com_ptr<GuildMemberManager> guildMembers{ nullptr };
		GuildMember() {}
		GuildMember(DiscordCoreInternal::GuildMemberData guildMemberData, Guild* guildNew, com_ptr<GuildMemberManager> guildMembersNew) {
			this->data = guildMemberData;
			this->guild = guildNew;
			this->guildMembers = guildMembersNew;
		}
		
	protected:
		friend class GuildMemberManager;

	};

	class GuildMemberManager: map<string,GuildMember>, public implements<GuildMemberManager, winrt::Windows::Foundation::IInspectable>{
	public:
		Guild* guild{ nullptr };
		GuildMemberManager() {}
		GuildMemberManager(Guild* guildNew, DiscordCoreInternal::HttpAgentPointers pointers) {
			this->guild = guildNew;
			this->pointers = pointers;
		}

		task<GuildMember> fetchAsync(string guildId, string guildMemberId) {
			DiscordCoreInternal::HttpWorkload workload;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::GET;
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::GET_CHANNEL;
			workload.relativePath = "/guilds/" + guildId + "/members/" + guildMemberId;
			send(&pointers.pGETAgent->workSubmissionBuffer, workload);
			json jsonValue = receive(pointers.pGETAgent->workReturnBuffer);
			DiscordCoreInternal::GuildMemberData guildMemberData;
			com_ptr<GuildMemberManager> pGuildMemberManager;
			pGuildMemberManager.attach(this);
			GuildMember guildMember(guildMemberData, this->guild, pGuildMemberManager);
			try {
				guildMember = this->at(guildMemberId);
				this->erase(guildMemberId);
			}
			catch (exception error) {
				cout << "fetchAsync() Error: " << error.what() << endl;
			}
			DiscordCoreInternal::parseObject(jsonValue, &guildMember.data);
			this->insert(make_pair(guildMemberId, guildMember));
			co_return guildMember;
		}

		task<GuildMember> getGuildMemberAsync(string guildMemberID) {
			try {
				GuildMember guildMember = this->at(guildMemberID);
				co_return guildMember;
			}
			catch (exception error) {
				cout << "getGuildMemberAsync() Error: " << error.what() << endl;
				DiscordCoreInternal::GuildMemberData guildMemberData;
				com_ptr<GuildMemberManager> pGuildMemberManager;
				pGuildMemberManager.attach(this);
				GuildMember guildMember(guildMemberData, this->guild, pGuildMemberManager);
				co_return guildMember;
			}
		}
		~GuildMemberManager() {}

	protected:
		DiscordCoreInternal::HttpAgentPointers pointers;
		friend class Guild;
	};
}
#endif
