#include "discord_rpc.h"
#include "/Users/Goshan/Desktop/source cssoupd/source/gui.h"
#include "/Users/Goshan/Desktop/source cssoupd/source/interfaces/interfaces.h"

void c_discord::initialize() {
    DiscordEventHandlers Handle;
    memset(&Handle, 0, sizeof(Handle));

    Discord_Initialize("1046869883264192532", &Handle, 1, NULL);
}

void c_discord::update() {
    static bool init = false;
    if (gui::settings::misc::discord) {
    if (!init){
        initialize();
    init = true;
}    
    DiscordRichPresence discordPresence;
    memset(&discordPresence, 0, sizeof(discordPresence));
    static auto elapsed = std::time ( nullptr );

    std::string current_status = { "In main menu" };
    if (engine->IsConnected()) {
		// get hostname from server
        current_status = "In game |  ";
		current_status += engine->get_level_name();
    }

    discordPresence.largeImageText = "777";
    discordPresence.state = current_status.c_str();
    discordPresence.largeImageKey = "https://cdn.discordapp.com/attachments/1190046675843170394/1226586413659717733/Professor-Money-Heist-Wallpapers-16.webp?ex=66254e9d&is=6612d99d&hm=c95a687267ae818db16e9493ed9063987e6456621dc2570f8fa68bfba55c91eb&";
    discordPresence.startTimestamp = elapsed;
    Discord_UpdatePresence(&discordPresence);
    } else {
        shutdown();
        init = false;
    }
}

void c_discord::shutdown() {
    Discord_ClearPresence();
    Discord_Shutdown();
}
