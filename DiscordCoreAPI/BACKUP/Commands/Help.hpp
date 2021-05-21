// Help.hpp - Header for the +ACI-help+ACI- command.
// May 20, 2021
// Chris M.
// https://github.com/RealTimeChris

+ACM-pragma once

+ACM-ifndef +AF8-HELP+AF8-
+ACM-define +AF8-HELP+AF8-

+ACM-include +ACI-../pch.h+ACI-
+ACM-include +ACI-../DiscordCoreClient.hpp+ACI-
+ACM-include +ACI-Commands.hpp+ACI-

namespace DiscordCoreAPI +AHs-
	extern shared+AF8-ptr+ADw-DiscordCoreClient+AD4- pDiscordCoreAPI+ADs-

	class HelpCommand : public BaseFunction+AHs-
	public:
		virtual  void execute(DiscordCoreAPI::DiscordCoreFunctionBaseArguments args) +AHs-
			Guild guild +AD0- args.pClient-+AD4-guilds-+AD4-fetchAsync(+AHs- args.message.data.guildId +AH0-).get()+ADs-
			Channel channel +AD0- guild.channels-+AD4-fetchAsync(+AHs- args.message.data.channelId +AH0-).get()+ADs-
			cout +ADwAPA- guild.data.name +ADwAPA- endl+ADs-
			cout +ADwAPA- channel.data.name +ADwAPA- endl+ADs-
			for (unsigned int x +AD0- 0+ADs- x +ADw- args.argumentsArray.size()+ADs- x +-+AD0- 1) +AHs-
				cout +ADwAPA- args.argumentsArray.at(x) +ADwAPA- endl+ADs-
			+AH0-
		+AH0-
	+AH0AOw-

	HelpCommand helpCommand+AHsAfQA7-
+AH0-
+ACM-endif.
