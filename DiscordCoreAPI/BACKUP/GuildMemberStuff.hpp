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
		shared_ptr<GuildMemberManager> guildMembers{ nullptr };
		GuildMember() {}
		GuildMember(DiscordCoreInternal::GuildMemberData guildMemberData, Guild* guildNew, shared_ptr<GuildMemberManager> guildMembersNew) {
			this->data = guildMemberData;
			this->guild = guildNew;
			this->guildMembers = guildMembersNew;
		}
		
	protected:
		friend class GuildMemberManager;

	};

	class GuildMemberManager: concurrent_unordered_map<string,GuildMember>, public implements<GuildMemberManager, winrt::Windows::Foundation::IInspectable>{
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
			try {
				guildMemberData = this->at(guildMemberId).data;
				this->unsafe_erase(guildMemberId);
			}
			catch (exception error) {
				cout << "fetchAsync() Error: " << error.what() << endl;
			}
			DiscordCoreInternal::parseObject(jsonValue, &guildMemberData);
			shared_ptr<GuildMemberManager> pGuildMemberManager;
			pGuildMemberManager.reset(this);
			GuildMember guildMember(guildMemberData, this->guild, pGuildMemberManager);
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
				shared_ptr<GuildMemberManager> pGuildMemberManager;
				pGuildMemberManager.reset(this);
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
