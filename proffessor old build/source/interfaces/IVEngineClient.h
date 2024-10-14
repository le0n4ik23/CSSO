#pragma once
#include "../classes/CEntity.h"
class Vector3;


#define SIGNED_GUID_LEN 32
#define MAX_PLAYER_NAME_LENGTH 32
#define MAX_CUSTOM_FILES		4

#define vec_t float

typedef struct player_info_s
{
    // scoreboard information
    char			name[128];
    // local server user ID, unique while server is running
    int				userID;
    // global unique player identifer
    char			guid[SIGNED_GUID_LEN + 1];
    // friends identification number
    int			friendsID;
    // friends name
    char			friendsName[MAX_PLAYER_NAME_LENGTH];
    // true, if player is a bot controlled by game.dll
    bool			fakeplayer;
    // true if player is the HLTV proxy
    bool			ishltv;
#if defined( REPLAY_ENABLED )
    // true if player is the Replay proxy
    bool			isreplay;
#endif
    // custom files CRC for this player
    int			customFiles[MAX_CUSTOM_FILES];
    // this counter increases each time the server downloaded a new file
    unsigned char	filesDownloaded;

} player_info_t;





class IVEngineClient
{
public:

    /*void GetScreenSize(int& width, int& height)
    {
        util::get_vfunc<5, void>(this, width, height);
    }*/
    virtual int					GetIntersectingSurfaces(
        const void* model,
        const Vector3& vCenter,
        const float radius,
        const bool bOnlyVisibleSurfaces,	// Only return surfaces visible to vCenter.
        void* pInfos,
        const int nMaxInfos) = 0;
 
    // Get the lighting intensivty for a specified point
    // If bClamp is specified, the resulting Vector is restricted to the 0.0 to 1.0 for each element
    virtual Vector3				GetLightForPoint(const Vector3& pos, bool bClamp) = 0;

    // Traces the line and reports the material impacted as well as the lighting information for the impact point
    virtual void* TraceLineMaterialAndLighting(const Vector3& start, const Vector3& end,
        Vector3& diffuseLightColor, Vector3& baseColor) = 0;

    // Given an input text buffer data pointer, parses a single token into the variable token and returns the new
    //  reading position
    virtual const char* ParseFile(const char* data, char* token, int maxlen) = 0;
    virtual bool				CopyLocalFile(const char* source, const char* destination) = 0;

    // Gets the dimensions of the game window
    virtual void				GetScreenSize(int& x, int& y) = 0;

    // Forwards szCmdString to the server, sent reliably if bReliable is set
    virtual void				ServerCmd(const char* szCmdString, bool bReliable = true) = 0;
    // Inserts szCmdString into the command buffer as if it was typed by the client to his/her console.
    // Note: Calls to this are checked against FCVAR_CLIENTCMD_CAN_EXECUTE (if that bit is not set, then this function can't change it).
    //       Call ClientCmd_Unrestricted to have access to FCVAR_CLIENTCMD_CAN_EXECUTE vars.
    virtual void				ClientCmd(const char* szCmdString) = 0;

    // Fill in the player info structure for the specified player index (name, model, etc.)
    virtual bool				GetPlayerInfo(int ent_num, player_info_t* pinfo) = 0;

    // Retrieve the player entity number for a specified userID
    virtual int					GetPlayerForUserID(int userID) = 0;

    // Retrieves text message system information for the specified message by name
    virtual void* TextMessageGet(const char* pName) = 0;

    // Returns true if the console is visible
    virtual bool				Con_IsVisible(void) = 0;

    // Get the entity index of the local player
    virtual int					GetLocalPlayer(void) = 0;

    // Client DLL is hooking a model, loads the model into memory and returns  pointer to the model_t
    virtual const void* LoadModel(const char* pName, bool bProp = false) = 0;

    // Get the exact server timesstamp ( server time ) from the last message received from the server
    virtual float				GetLastTimeStamp(void) = 0;

    // Given a CAudioSource (opaque pointer), retrieve the underlying CSentence object ( stores the words, phonemes, and close
    //  captioning data )
    virtual void* GetSentence(void* pAudioSource) = 0;
    // Given a CAudioSource, determines the length of the underlying audio file (.wav, .mp3, etc.)
    virtual float				GetSentenceLength(void* pAudioSource) = 0;
    // Returns true if the sound is streaming off of the hard disk (instead of being memory resident)
    virtual bool				IsStreaming(void* pAudioSource) const = 0;

    // Copy current view orientation into va
    virtual void				placeHolder() = 0;
    // Set current view orientation from va

    virtual void				GetViewAngles(Vector3& va) = 0;

    virtual void				SetViewAngles(const Vector3& va) = 0;


    // Retrieve the current game's maxclients setting
    virtual int					GetMaxClients(void) = 0;

    // Given the string pBinding which may be bound to a key, 
    //  returns the string name of the key to which this string is bound. Returns NULL if no such binding exists
    virtual	const char* Key_LookupBinding(const char* pBinding) = 0;

    // Given the name of the key "mouse1", "e", "tab", etc., return the string it is bound to "+jump", "impulse 50", etc.
    virtual const char* Key_BindingForKey(int code) = 0;

    // Bind a key
    virtual	void				Key_SetBinding(int code, const char* pBinding) = 0;

    // key trapping (for binding keys)
    virtual void				StartKeyTrapMode(void) = 0;
    virtual bool				CheckDoneKeyTrapping(int& code) = 0;

    // Returns true if the player is fully connected and active in game (i.e, not still loading)
    virtual bool				IsInGame(void) = 0;
    // Returns true if the player is connected, but not necessarily active in game (could still be loading)
    virtual bool				IsConnected(void) = 0;
    // Returns true if the loading plaque should be drawn
    virtual bool				IsDrawingLoadingImage(void) = 0;
    virtual void				HideLoadingPlaque(void) = 0;

    // Prints the formatted string to the notification area of the screen ( down the right hand edge
    //  numbered lines starting at position 0
    virtual void				Con_NPrintf(int pos...) = 0;
    // Similar to Con_NPrintf, but allows specifying custom text color and duration information
    virtual void				Con_NXPrintf(const struct con_nprint_s* info...) = 0;

    // Is the specified world-space bounding box inside the view frustum?
    virtual int					IsBoxVisible(const Vector3& mins, const Vector3& maxs) = 0;

    // Is the specified world-space boudning box in the same PVS cluster as the view origin?
    virtual int					IsBoxInViewCluster(const Vector3& mins, const Vector3& maxs) = 0;

    // Returns true if the specified box is outside of the view frustum and should be culled
    virtual bool				CullBox(const Vector3& mins, const Vector3& maxs) = 0;

    // Allow the sound system to paint additional data (during lengthy rendering operations) to prevent stuttering sound.
    virtual void				Sound_ExtraUpdate(void) = 0;
#if defined(_PS3)
    // Allow sound system updates from engine, ensure only done if sv.IsActive() is true 
    virtual void				Sound_ServerUpdateSoundsPS3(void) = 0;
#endif
#define FLOW_INCOMING	1
#define FLOW_OUTGOING	0
    // Get the current game directory ( e.g., hl2, tf2, cstrike, hl1 )
    virtual const char* GetGameDirectory(void) = 0;

    // Get access to the world to screen transformation matrix
    virtual const int& WorldToScreenMatrix() = 0;
    void execute_cmd(const char* cmd, bool from_console_or_keybind = false) {
        using original_fn = void(__thiscall*)(IVEngineClient*, const char*, bool);
        return (*(original_fn**)this)[114](this, cmd, from_console_or_keybind);
    }
    void get_player_info(int index, player_info_t* info) {
        using fn = void(__thiscall*)(IVEngineClient*, int, player_info_t*);
        return (*(fn**)this)[8](this, index, info);
    }
    const char* get_level_name() {
        using fn = const char* (__thiscall*)(IVEngineClient*);
        return (*(fn**)this)[53](this);
    }
};


class INetChannelInfo
{
public:

    enum {
        GENERIC = 0,	// must be first and is default group
        LOCALPLAYER,	// bytes for local player entity update
        OTHERPLAYERS,	// bytes for other players update
        ENTITIES,		// all other entity bytes
        SOUNDS,			// game sounds
        EVENTS,			// event messages
        USERMESSAGES,	// user messages
        ENTMESSAGES,	// entity messages
        VOICE,			// voice data
        STRINGTABLE,	// a stringtable update
        MOVE,			// client move cmds
        STRINGCMD,		// string command
        SIGNON,			// various signondata
        TOTAL,			// must be last and is not a real group
    };

    virtual const char* GetName(void) const = 0;	// get channel name
    virtual const char* GetAddress(void) const = 0; // get channel IP address as string
    virtual float		GetTime(void) const = 0;	// current net time
    virtual float		GetTimeConnected(void) const = 0;	// get connection time in seconds
    virtual int			GetBufferSize(void) const = 0;	// netchannel packet history size
    virtual int			GetDataRate(void) const = 0; // send data rate in byte/sec

    virtual bool		IsLoopback(void) const = 0;	// true if loopback channel
    virtual bool		IsTimingOut(void) const = 0;	// true if timing out
    virtual bool		IsPlayback(void) const = 0;	// true if demo playback

    virtual float		GetLatency(int flow) const = 0;	 // current latency (RTT), more accurate but jittering
    virtual float		GetAvgLatency(int flow) const = 0; // average packet latency in seconds
    virtual float		GetAvgLoss(int flow) const = 0;	 // avg packet loss[0..1]
    virtual float		GetAvgChoke(int flow) const = 0;	 // avg packet choke[0..1]
    virtual float		GetAvgData(int flow) const = 0;	 // data flow in bytes/sec
    virtual float		GetAvgPackets(int flow) const = 0; // avg packets/sec
    virtual int			GetTotalData(int flow) const = 0;	 // total flow in/out in bytes
    virtual int			GetSequenceNr(int flow) const = 0;	// last send seq number
    virtual bool		IsValidPacket(int flow, int frame_number) const = 0; // true if packet was not lost/dropped/chocked/flushed
    virtual float		GetPacketTime(int flow, int frame_number) const = 0; // time when packet was send
    virtual int			GetPacketBytes(int flow, int frame_number, int group) const = 0; // group size of this packet
    virtual bool		GetStreamProgress(int flow, int* received, int* total) const = 0;  // TCP progress if transmitting
    virtual float		GetTimeSinceLastReceived(void) const = 0;	// get time since last recieved packet in seconds
    virtual	float		GetCommandInterpolationAmount(int flow, int frame_number) const = 0;
    virtual void		GetPacketResponseLatency(int flow, int frame_number, int* pnLatencyMsecs, int* pnChoke) const = 0;
    virtual void		GetRemoteFramerate(float* pflFrameTime, float* pflFrameTimeStdDeviation) const = 0;
    virtual INetChannelInfo* GetNetChannelInfo(void) = 0;
    virtual float		GetTimeoutSeconds() const = 0;
};