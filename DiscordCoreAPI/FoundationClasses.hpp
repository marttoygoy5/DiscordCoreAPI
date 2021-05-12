// FoundationClasses.hpp - Header for "shared classes".
// Apr 30, 2021
// Chris M.
// https://github/com/RealTimeChris

#pragma once

#ifndef _FOUNDATION_CLASSES
#define _FOUNDATION_CLASSES

#include "ClientDataTypes.hpp"

namespace CommanderNS {

	struct HTTPData {
		json data;
	};

	enum class RateLimitType{
		UNSET =0, 
		GET_MESSAGE = 1,
		POST_MESSAGE = 2, 
		DELETE_MESSAGE = 3,
		GET_USER = 4, 
		GET_USER_SELF = 5, 
		GET_GUILD = 6, 
		GET_CHANNEL = 7, 
		GET_REACTION = 8,
		PUT_REACTION = 9,
		DELETE_REACTION = 10,
		DELETE_ALL_REACTION = 11,
		GET_GUILD_MEMBER = 12,
		GET_GUILD_MEMBERS = 13,
		GET_ROLES = 14,
		GET_USER_GUILDS = 15
	};

	struct RateLimitData {
		int getsRemaining = 1;
		float msRemain = 0;
		float timeStartedAt = 0;
		RateLimitType rateLimitType;
		string bucket;
	};

	class PermissionsConverter {
	public:
		static bool checkForPresence(ClientDataTypes::Permissions permission, string permissionString) {
			int permissionsInteger = stoll(permissionString);
			if ((permissionsInteger & (int)permission) == (int)permission) {
				return true;
			}
			else {
				return false;
			}
		}
		static string addPermissionsToString(string originalPermissionString, ClientDataTypes::Permissions permissionsToAdd[], int quantityOfPermsToAdd) {
			int permissionsInteger = stoll(originalPermissionString);
			for (unsigned int x = 0; x < quantityOfPermsToAdd; x += 1) {
				permissionsInteger = permissionsInteger | (int)permissionsToAdd[x];
			}
			stringstream sstream;
			sstream << permissionsInteger;
			return sstream.str();
		}
		static string removePermissionsFromString(string originalPermissionString, ClientDataTypes::Permissions permissionsToRemove[], int quantityOfPermsToRemove) {
			int permissionsInteger = stoll(originalPermissionString);
			for (unsigned int x = 0; x < quantityOfPermsToRemove; x += 1) {
				permissionsInteger = permissionsInteger & ~(int)permissionsToRemove[x];
			}
			stringstream sstream;
			sstream << permissionsInteger;
			return sstream.str();
		}
		static void displayPermissions(string permissionString) {
			int permissionsInteger = stoll(permissionString);
			stringstream sstream;
			if (permissionsInteger & (1 << 0)) {
				sstream << "CREATE_INSTANT_INVITE" << endl;
			}
			if (permissionsInteger & (1 << 1)) {
				sstream << "KICK_MEMBERS" << endl;
			}
			if (permissionsInteger & (1 << 2)) {
				sstream << "BAN_MEMBERS" << endl;
			}
			if (permissionsInteger & (1 << 3)) {
				sstream << "ADMINISTRATOR" << endl;
			}
			if (permissionsInteger & (1 << 4)) {
				sstream << "MANAGE_CHANNELS" << endl;
			}
			if (permissionsInteger & (1 << 5)) {
				sstream << "MANAGE_GUILD" << endl;
			}
			if (permissionsInteger & (1 << 6)) {
				sstream << "ADD_REACTIONS" << endl;
			}
			if (permissionsInteger & (1 << 7)) {
				sstream << "VIEW_AUDIT_LOG" << endl;
			}
			if (permissionsInteger & (1 << 8)) {
				sstream << "PRIORITY_SPEAKER" << endl;
			}
			if (permissionsInteger & (1 << 9)) {
				sstream << "STREAM" << endl;
			}
			if (permissionsInteger & (1 << 10)) {
				sstream << "VIEW_CHANNEL" << endl;
			}
			if (permissionsInteger & (1 << 11)) {
				sstream << "SEND_MESSAGES" << endl;
			}
			if (permissionsInteger & (1 << 12)) {
				sstream << "SEND_TTS_MESSAGES" << endl;
			}
			if (permissionsInteger & (1 << 13)) {
				sstream << "MANAGE_MESSAGES" << endl;
			}
			if (permissionsInteger & (1 << 14)) {
				sstream << "EMBED_LINKS" << endl;
			}
			if (permissionsInteger & (1 << 15)) {
				sstream << "ATTACH_FILES" << endl;
			}
			if (permissionsInteger & (1 << 16)) {
				sstream << "READ_MESSAGE_HISTORY" << endl;
			}
			if (permissionsInteger & (1 << 17)) {
				sstream << "MENTION_EVERYONE" << endl;
			}
			if (permissionsInteger & (1 << 18)) {
				sstream << "USE_EXTERNAL_EMOJI" << endl;
			}
			if (permissionsInteger & (1 << 19)) {
				sstream << "VIEW_GUILD_INSIGHTS" << endl;
			}
			if (permissionsInteger & (1 << 20)) {
				sstream << "CONNECT" << endl;
			}
			if (permissionsInteger & (1 << 21)) {
				sstream << "SPEAK" << endl;
			}
			if (permissionsInteger & (1 << 22)) {
				sstream << "MUTE_MEMBERS" << endl;
			}
			if (permissionsInteger & (1 << 23)) {
				sstream << "DEAFEN_MEMBERS" << endl;
			}
			if (permissionsInteger & (1 << 24)) {
				sstream << "MOVE_MEMBERS" << endl;
			}
			if (permissionsInteger & (1 << 25)) {
				sstream << "USE_VAD" << endl;
			}
			if (permissionsInteger & (1 << 26)) {
				sstream << "CHANGE_NICKNAME" << endl;
			}
			if (permissionsInteger & (1 << 27)) {
				sstream << "MANAGE_NICKNAMES" << endl;
			}
			if (permissionsInteger & (1 << 28)) {
				sstream << "MANAGE_ROLES" << endl;
			}
			if (permissionsInteger & (1 << 29)) {
				sstream << "MANAGE_WEBHOOKS" << endl;
			}
			if (permissionsInteger & (1 << 30)) {
				sstream << "MAANGE_EMOJIS" << endl;
			}
			if (permissionsInteger & (1 << 31)) {
				sstream << "USE_SLASH_COMMANDS" << endl;
			}
			if (permissionsInteger & (1 << 32)) {
				sstream << "REQUEST_TO_SPEAK" << endl;
			}
			if (permissionsInteger & (1 << 34)) {
				sstream << "MANAGE_THREADS" << endl;
			}
			if (permissionsInteger & (1 << 35)) {
				sstream << "USE_PUBLIC_THREADS" << endl;
			}
			if (permissionsInteger & (1 << 36)) {
				sstream << "USE_PRIVATE_THREADS" << endl;
			}
			cout << "PERMISSIONS: " << endl << sstream.str() << endl;
		}
	};
};
#endif
