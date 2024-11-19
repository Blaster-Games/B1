// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Blaster : ModuleRules
{
	public Blaster(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDefinitions.AddRange(new string[]
       {
           "WITH_DEBUGGING=1",
           "DO_CHECK=0",
           "DO_GUARD_SLOW=0"
       });

        PublicIncludePaths.AddRange(new string[]
        {
            "Blaster"
        });

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "Niagara", "MultiplayerSessions", "OnlineSubsystem", "OnlineSubsystemSteam", 
            "Sockets",
            "Networking",
            "HTTP",
            "Json",
            "JsonUtilities",
            "SlateCore"
        });
        
        PrivateDependencyModuleNames.AddRange(new string[] { "ProtobufCore" });

        PrivateIncludePaths.AddRange(new string[]
        {
	        "Blaster/Network/",
        });
	}
}
