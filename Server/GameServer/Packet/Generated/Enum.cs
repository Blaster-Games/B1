// <auto-generated>
//     Generated by the protocol buffer compiler.  DO NOT EDIT!
//     source: Enum.proto
// </auto-generated>
#pragma warning disable 1591, 0612, 3021, 8981
#region Designer generated code

using pb = global::Google.Protobuf;
using pbc = global::Google.Protobuf.Collections;
using pbr = global::Google.Protobuf.Reflection;
using scg = global::System.Collections.Generic;
namespace Google.Protobuf.Protocol {

  /// <summary>Holder for reflection information generated from Enum.proto</summary>
  public static partial class EnumReflection {

    #region Descriptor
    /// <summary>File descriptor for Enum.proto</summary>
    public static pbr::FileDescriptor Descriptor {
      get { return descriptor; }
    }
    private static pbr::FileDescriptor descriptor;

    static EnumReflection() {
      byte[] descriptorData = global::System.Convert.FromBase64String(
          string.Concat(
            "CgpFbnVtLnByb3RvEghQcm90b2NvbCo+CglFR2FtZU1vZGUSDQoJTU9ERV9O",
            "T05FEAASEwoPTU9ERV9ERUFUSE1BVENIEAESDQoJTU9ERV9URUFNEAIqNwoJ",
            "RVRlYW1UeXBlEg0KCVRFQU1fTk9ORRAAEgwKCFRFQU1fUkVEEAESDQoJVEVB",
            "TV9CTFVFEAIqQgoKRVJvb21TdGF0ZRIOCgpTVEFURV9OT05FEAASEQoNU1RB",
            "VEVfV0FJVElORxABEhEKDVNUQVRFX1BMQVlJTkcQAkIbqgIYR29vZ2xlLlBy",
            "b3RvYnVmLlByb3RvY29sYgZwcm90bzM="));
      descriptor = pbr::FileDescriptor.FromGeneratedCode(descriptorData,
          new pbr::FileDescriptor[] { },
          new pbr::GeneratedClrTypeInfo(new[] {typeof(global::Google.Protobuf.Protocol.EGameMode), typeof(global::Google.Protobuf.Protocol.ETeamType), typeof(global::Google.Protobuf.Protocol.ERoomState), }, null, null));
    }
    #endregion

  }
  #region Enums
  public enum EGameMode {
    [pbr::OriginalName("MODE_NONE")] ModeNone = 0,
    [pbr::OriginalName("MODE_DEATHMATCH")] ModeDeathmatch = 1,
    [pbr::OriginalName("MODE_TEAM")] ModeTeam = 2,
  }

  public enum ETeamType {
    [pbr::OriginalName("TEAM_NONE")] TeamNone = 0,
    [pbr::OriginalName("TEAM_RED")] TeamRed = 1,
    [pbr::OriginalName("TEAM_BLUE")] TeamBlue = 2,
  }

  public enum ERoomState {
    [pbr::OriginalName("STATE_NONE")] StateNone = 0,
    [pbr::OriginalName("STATE_WAITING")] StateWaiting = 1,
    [pbr::OriginalName("STATE_PLAYING")] StatePlaying = 2,
  }

  #endregion

}

#endregion Designer generated code
