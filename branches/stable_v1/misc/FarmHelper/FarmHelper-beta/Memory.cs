using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Magic;
using FarmHelper_beta;
using System.Runtime.InteropServices;
using System.Threading;

namespace Memory
{
    public class Memory
    {
        public Memory()
        {
            InitMemory();
        }
        public void InitMemory()
        {
            try
            {
                wow = new BlackMagic(); //Create new function to open wow process
                if (PId == 0)
                    wow.OpenProcessAndThread(SProcess.GetProcessFromWindowTitle("World of Warcraft"));
                else wow.OpenProcessAndThread(PId);
                if (wow.ProcessId != 0)
                {
                    WowControl.hWnd = (uint)wow.WindowHandle;
                    GoTo(0, 0, 0, 0, 0);
                }
                else
                {
					
                    playerbase = 0;
                    BgStatus = 0;
                    IsIndoors = 0; IsMounted = 0;
                    BattlefieldInstanceExpiration = 0;
                    Location = ""; SubLocation = ""; LastError = "";
                    Speed = 0; ;
                    X = 0; Y = 0; Z = 0;
                    LastRead = new DateTime();
                    PlayerForm = 0;
                    Objects = new List<ObjectInfo>();
                    PlayerBuffs = new List<AURA>();
                }
            }
            catch (Exception) { }
        }
		public enum Lua
		{

			Lua_DoString = 0x008193B0,                  // 3.3.5
			Lua_GetLocalizedText = 0x007229C0,          // 3.3.5

		}

		public enum Direct3D9
		{

			pDevicePtr_1 = 0x00C5DF80,                  // 3.3.5
			pDevicePtr_2 = 0x397C,                      // 3.3.5
			oEndScene = 0xA8,                           // 3.3.5

		}

		public enum ObjectManager
		{

			CurMgrPointer = 0xC79CE0,                 // 3.3.5 +
			CurMgrOffset = 0x2ED0,                      // 3.3.5 +
			NextObject = 0x3C,                          // 3.3.5
			FirstObject = 0xAC,                         // 3.3.5
			LocalGUID = 0xC0                            // 3.3.5

		}

		public enum Globals
		{

			s_PlayerName = 0x00C79D18,                  // 3.3.5 +
			s_PlayerBase = 0x00CD87A8,                  // 3.3.5 +
			s_RealmName = 0x00C79B9E,                  // 3.3.5 +
			s_CurrentTarget = 0x00BD07B0,               // 3.3.5 +
			s_CurrentAccount = 0x00B6AA40,              // 3.3.5 +
			s_LastTarget = 0x00BD07B8,                  // 3.3.5 +
			s_MouseOver = 0x00BD07A0,                   // 3.3.5 +
			s_ComboPoint = 0x00BD084D,                  // 3.3.5 +
			s_LootWindow = 0x00BFA8D8,                  // 3.3.5 +
			s_KnownSpell = 0x00BE5D88,                  // 3.3.5 +
			s_IsLoadingOrConnecting = 0x00B6AA38,       // 3.3.5 +
			Movement_Field = 0xD8,                      // 3.3.5
			LocationName = 0x00BD0780,					// 3.3.5 +
			GameState = 0x00BD078A, //1 = in game 0 = loadingscreen
			CharCount = 0xB6B23C,					// 3.3.5 +
			CharNo = 0xAC436C,					// 3.3.5 +

			LastError = 0xBCFB90,					// 3.3.5 +
			PlayerGUID = 0x00CA1238,					// 3.3.5 +
			PlayerID = 0x00CA123C,					// 3.3.5 +
			TargetGUID = 0x00BD07B0,					// 3.3.5 +
			BGStatus = 0x00BEA4D0,					// 3.3.5 +
			IsIndoors = 0x00B4AA94,					// 3.3.5 +
			PlayerNameCache = 0xC5D938,					// 3.3.5 +

			s_SpellCooldown_Pointer = 0x00D3F5AC,       // 3.3.5 +

			s_Timestamp = 0x00B1D618,                   // 3.3.5
			s_LastHardwareAction = 0x00B4999C,          // 3.3.5

			ClntObjMgrObjectPtr = 0x004D5548,           // 3.3.5 +
			ClntObjMgrGetActivePlayerObj = 0x4038F8,    // 3.3.5 +
			HandleTerrainClick = 0x0080C458,            // 3.3.5 +
			CGGameUI_Target = 0x005253F0,               // 3.3.5
			Spell_C_CastSpell = 0x0080DB50,             // 3.3.5
			CGUnit_C__GetCreatureType = 0x0071F6E0,     // 3.3.5
			CGUnit_C__GetCreatureRank = 0x00718DE0,     // 3.3.5

			nbItemsSellByMerchant = 0x00BFA3E8,         // 3.3.5

			CInputControl = 0x00C2494C,                 // 3.3.5

		}

		public enum ClickToMove
		{

			CGPlayer_C__ClickToMove = 0x007278B8,       // 3.3.5 +

			CTM_Activate_Pointer = 0xBD08F4,            // 3.3.5 +
			CTM_Activate_Offset = 0x30,                 // 3.3.5 +

			CTM_Base = 0x00CA11D8,                      // 3.3.5 +

		}

		public enum Party
		{

			s_LeaderGUID = 0x00BD1960,                  // 3.3.5
			s_Member1GUID = 0x00BD1940,                 // 3.3.5
			s_Member2GUID = s_Member1GUID + 0x8,        // 3.3.5
			s_Member3GUID = s_Member2GUID + 0x8,        // 3.3.5
			s_Member4GUID = s_Member3GUID + 0x8,        // 3.3.5

		}

		public enum Corpse
		{

			X = 0x00BD0A58,                              // 3.3.5 +
			Y = X + 0x4,                                 // 3.3.5
			Z = X + 0x8,                                 // 3.3.5

		}

		public enum ShapeshiftForm
		{

			CGUnit_C__GetShapeshiftFormId = 0x0071B358, // 3.3.5   +

			BaseAddress_Offset1 = 0xD0,                 // 3.3.5 
			BaseAddress_Offset2 = 0x1D3,                // 3.3.5 

		}

		public enum UnitBaseGetUnitAura
		{

			CGUnit_Aura = 0x005576A8,                   // 3.3.5 +
			AURA_COUNT_1 = 0xDD0,                       // 3.3.5
			AURA_COUNT_2 = 0xC54,                       // 3.3.5
			AURA_TABLE_1 = 0xC50,                       // 3.3.5
			AURA_TABLE_2 = 0xC58,                       // 3.3.5
			AURA_SIZE = 0x18,                           // 3.3.5
			AURA_SPELL_ID = 0x8                         // 3.3.5

		}

		public enum Movements
		{

			MoveForwardStart = 0x005FC5B0,              // 3.3.5
			MoveForwardStop = 0x005FC600,               // 3.3.5
			MoveBackwardStart = 0x005FC640,             // 3.3.5
			MoveBackwardStop = 0x005FC690,              // 3.3.5
			TurnLeftStart = 0x005FC6D0,                 // 3.3.5
			TurnLeftStop = 0x005FC710,                  // 3.3.5
			TurnRightStart = 0x005FC760,                // 3.3.5
			TurnRightStop = 0x005FC7A0,                 // 3.3.5
			JumpOrAscendStart = 0x005FC330,             // 3.3.5
			AscendStop = 0x005FC450,                    // 3.3.5

		}

		public enum IsFlying
		{

			// Reversed from Lua_IsFlying

			IsFlyingOffset = 0x44,                      // 3.3.5
			IsFlying_Mask = 0x2000000,                  // 3.3.5

		}

		public enum IsSwimming
		{

			// Reversed from Lua_IsSwimming

			IsSwimmingOffset = 0xA30,                   // 3.3.5
			IsSwimming_Mask = 0x200000,                 // 3.3.5

		}

		public enum AutoLoot
		{

			AutoLoot_Activate_Pointer = 0x00D090C,      // 3.3.5
			AutoLoot_Activate_Offset = 0x30,            // 3.3.5

		}

		public enum AutoSelfCast
		{

			AutoSelfCast_Activate_Pointer = 0xBD0918,   // 3.3.5
			AutoSelfCast_Activate_Offset = 0x30,        // 3.3.5

		}

		public enum WoWChat
		{

			ChatBufferStart = 0x00B75A60,               // 3.3.5 +
			NextMessage = 0x17C0,                       // 3.3.5 +
			ChatBufferCount = 0x00BCEFF4,               // 3.3.5 +

		}
        public struct AURA
        {
            public UInt64 creatorGuid;	// 0x00 - 0x07
            public uint auraId;	// 0x08 - 0x0B
            public ulong unk1;	// 0x0C - 0x0F
            public ulong unk2;	// 0x10 - 0x13
            public ulong endTime;	// 0x14 - 0x17
        }      
        public struct ObjectInfo
        {
            public uint Type;
            public float X;
            public float Y;
            public float Z;
            public float R;
            public String Name;
            public bool IsTarget;
            public bool IsPlayer;
            public uint Health;
            public uint MaxHealth;
            public uint Level;
            public UInt64 GUID;
            public uint IsInCombat;
            public String Faction;
            public uint IsLootable;
            public List<AURA> Auras;
            public int Power;
        }
        public uint playerbase, BgStatus, IsIndoors, IsMounted, BattlefieldInstanceExpiration, TestParam;
        public String Location, SubLocation, LastError;
        public double Speed;
        private float X, Y, Z;
        public String RealmName, LoginState;
        private DateTime LastRead;
		public int PlayerForm, PId, Combopoints, LoadingState, CharCount, CharNo;
        public bool Online;
        public ObjectInfo Temp;
        public BlackMagic wow;
        public List<ObjectInfo> Objects = new List<ObjectInfo>();
        public List<AURA> PlayerBuffs = new List<AURA>();
        public List<int> PowerList = new List<int>();

		
        protected const uint AURA_COUNT_1 = 0xdd0;
        protected const uint AURA_COUNT_2 = 0xc54;
        protected const uint AURA_SIZE = 0x18;
        protected const uint AURA_SPELL_ID = 8;
        protected const uint AURA_TABLE_1 = 0xc50;
        protected const uint AURA_TABLE_2 = 0xc58;



        public List<AURA> getAuras(uint unit)
        {
            List<AURA> result = new List<AURA>();
            AURA A = new AURA();
            uint auraTable = unit + AURA_TABLE_1; //aura list & count has 2 possible locations
            uint auraCount = wow.ReadUInt(unit + AURA_COUNT_1);
            if (auraCount > 80)
            {
                auraTable = wow.ReadUInt(unit + AURA_TABLE_2); //and the second one
                auraCount = wow.ReadUInt(unit + AURA_COUNT_2);
            }
            for (uint i = 0; i < auraCount; i++)
            {
                uint spellId = wow.ReadUInt(auraTable + AURA_SIZE * i + AURA_SPELL_ID);
                if (spellId > 0)
                {
                    UInt64 creatorGUID = wow.ReadUInt64(auraTable + AURA_SIZE * i);
                    A.auraId = spellId;
                    A.creatorGuid = creatorGUID;
                    result.Add(A);
                }
            }
            return result;
        }
     /*   public void DoString(string pszString)
        {
            uint s_curMgr = wow.ReadUInt(wow.ReadUInt(CurrMgr_Ptr) + CurrMgr_Offs);
            uint pScript = wow.AllocateMemory(0x1024);
            wow.WriteASCIIString(pScript + 0x1024, pszString);

            uint codeCave = wow.AllocateMemory(0x1024);

            wow.Asm.Clear();
            wow.Asm.AddLine("FS mov EAX, [0x2C]");
            wow.Asm.AddLine("mov eax, [eax]");
            wow.Asm.AddLine("add eax, 8");
            wow.Asm.AddLine("mov edx, {0}", s_curMgr);
            wow.Asm.AddLine("mov [eax], edx");

            wow.Asm.AddLine("mov ecx, {0}", pScript + pszString.Length - 1);
            wow.Asm.AddLine("mov eax, " + pScript);

            wow.Asm.AddLine("push ecx");
            wow.Asm.AddLine("push eax");
            wow.Asm.AddLine("push eax");

            wow.Asm.AddLine("mov eax, 0x004B32B0");
            wow.Asm.AddLine("call eax");
            wow.Asm.AddLine("add esp, 0xC");
            wow.Asm.AddLine("retn");

            wow.Asm.InjectAndExecute(codeCave);
            wow.FreeMemory(codeCave);

            return;
        }*/
        public void Lua_DoString(string luaString)
        {
            uint cave = 0;
            try
            {
                wow.SuspendThread(wow.ThreadHandle);
                
                cave = wow.AllocateMemory(0x2048);
                wow.WriteASCIIString(cave + 0x1024, luaString);

                wow.Asm.Clear();

                wow.Asm.AddLine("mov EDX, [0x00BB43F0]");
                wow.Asm.AddLine("mov EDX, [EDX+0x00002EB0]");

                wow.Asm.AddLine("FS mov EAX, [0x2C]");
                wow.Asm.AddLine("mov EAX, [EAX]");
                wow.Asm.AddLine("add EAX, 10");
                wow.Asm.AddLine("mov [EAX], edx");

                wow.Asm.AddLine("push 0");
                wow.Asm.AddLine("mov eax, " + (cave + 0x1024));
                wow.Asm.AddLine("push eax");
                wow.Asm.AddLine("push eax");
                wow.Asm.AddLine("call 0x004B32B0");
                wow.Asm.AddLine("add esp, 0xC");
                wow.Asm.AddLine("retn");

                wow.Asm.InjectAndExecute(cave);
                wow.ResumeThread(wow.ThreadHandle);

                wow.FreeMemory(cave);
                wow.Asm.Clear();
            }
            catch
            {
                wow.ResumeThread(wow.ThreadHandle);
                wow.FreeMemory(cave);
                wow.Asm.Clear();
            }
        }
		public void Test(String pszString)
		{

			uint curMgr = wow.ReadUInt(wow.ReadUInt((uint)ObjectManager.CurMgrPointer) + (uint)ObjectManager.CurMgrOffset);
			//String pszString="DoEmote(\"dance\")";
			uint pScript = wow.AllocateMemory(0x1024);
			wow.WriteASCIIString(pScript, pszString);

			uint codeCave = wow.AllocateMemory(0x1024);

			wow.Asm.Clear();
			wow.Asm.AddLine("FS mov EAX, [0x2C]");
			wow.Asm.AddLine("mov eax, [eax]");
			wow.Asm.AddLine("add eax, 8");
			wow.Asm.AddLine("mov edx, {0}", curMgr);
			wow.Asm.AddLine("mov [eax], edx");

			wow.Asm.AddLine("mov ecx, {0}", pScript + pszString.Length - 1);
			wow.Asm.AddLine("mov eax, " + pScript);

			wow.Asm.AddLine("push ecx");
			wow.Asm.AddLine("push eax");
			wow.Asm.AddLine("push eax");

			wow.Asm.AddLine("mov eax, 0x0077DEF0");
			wow.Asm.AddLine("call eax");
			wow.Asm.AddLine("add esp, 0xC");
			wow.Asm.AddLine("retn");

			wow.Asm.InjectAndExecute(codeCave);
			wow.FreeMemory(codeCave);

			return;
		}
        public int GetUnitPower(uint Object)
        {
            uint Power; uint MaxPower;
            for (uint i = 0; i < 7; i++)
            {
                Power = wow.ReadUInt(wow.ReadUInt(playerbase + 0x8) + ((i + 0x19) * 4)); // Reads players health
                MaxPower = wow.ReadUInt(wow.ReadUInt(playerbase + 0x8) + ((i + 0x21) * 4)); // Reads players health
                if ((MaxPower > 10) & (MaxPower > 10))
                {
                    int P = Convert.ToInt32(Convert.ToDouble(Convert.ToDouble(Power) / Convert.ToDouble(MaxPower)) * 100);
                    if (P != 0)
                        PowerList.Add(P);
                }
            }
            if (PowerList.Count > 0)
                return PowerList[0];
            else return 0;

        }
        public void Read()
        {
            try
            {
                try
                {
					playerbase = wow.ReadUInt(wow.ReadUInt(wow.ReadUInt((uint)Globals.s_PlayerBase) + 0x34) + 0x24);
					if (playerbase != 0)
                        Online = true;
                    else Online = false;
                } //this is the player base
                catch (Exception) { Online = false; }
				BgStatus = wow.ReadUInt((uint)Globals.BGStatus);
				IsIndoors = wow.ReadUInt((uint)Globals.IsIndoors);
				LastError = ReadRealName(wow.ReadBytes((uint)Globals.LastError, 120));
				BattlefieldInstanceExpiration = 0;//wow.ReadUInt(0xC55A24) & 1;
				Combopoints = Convert.ToInt16(wow.ReadByte((uint)Globals.s_ComboPoint));
				CharCount = Convert.ToInt16(wow.ReadByte((uint)Globals.CharCount));
				CharNo = Convert.ToInt16(wow.ReadByte((uint)Globals.CharNo));
				String WOWBuild = ReadRealName(wow.ReadBytes(0x00A30BE6, 10));
				byte[] TT = wow.ReadBytes(0x00A30BE6, 10);
				LoginState = wow.ReadASCIIString(0xB6A9E0, 40);
				RealmName = ReadRealName(wow.ReadBytes((uint)Globals.s_RealmName, 120));
				LoadingState = (int)wow.ReadUInt((uint)Globals.s_IsLoadingOrConnecting);
				WowControl.CheckLastError(LastError);
                ObjectInfo Target = new ObjectInfo();
                byte[] UnitFields = wow.ReadBytes(wow.ReadUInt(playerbase + 0x8) + (0x17 * 4), 4);
                PlayerForm = Convert.ToInt32(UnitFields[3]);
                Objects.Clear();
                IsMounted = wow.ReadUInt(playerbase + 0x9C0);// &1;
                Temp = new ObjectInfo();
                Temp.Auras = getAuras(playerbase);
                WowControl.PlayerBuffs.Clear();
                for (int i = 0; i < Temp.Auras.Count; i++)
                    WowControl.PlayerBuffs.Add(Temp.Auras[i].auraId);
				Temp.Name = ReadRealName(wow.ReadBytes((uint)Globals.s_PlayerName, 120));
                Temp.Level = wow.ReadUInt(wow.ReadUInt(playerbase + 0x8) + (0x36 * 4)); // Reads players level
                Temp.Health = wow.ReadUInt(wow.ReadUInt(playerbase + 0x8) + (0x18 * 4)); // Reads players health
                PowerList.Clear();
                Temp.Power = GetUnitPower(playerbase);
                Temp.MaxHealth = wow.ReadUInt(wow.ReadUInt(playerbase + 0x8) + (0x20 * 4)); // Reads players maxhealth
                if ((Location != null) & (Location != ""))
                {
					if (Location != ReadRealName(wow.ReadBytes(wow.ReadUInt((uint)Globals.LocationName), 60)))
                    {
                        WowControl.LocationChangeTime = DateTime.Now;
						Location = ReadRealName(wow.ReadBytes(wow.ReadUInt((uint)Globals.LocationName), 60));
                        WowControl.BadObjects.Clear();
                        if (BgStatus == 3)
                            WowControl.WaitTime = DateTime.Now.AddSeconds(20);
                        else WowControl.WaitTime = DateTime.Now.AddSeconds(1);
                        return;
                    }
                }
                else
					Location = ReadRealName(wow.ReadBytes(wow.ReadUInt((uint)Globals.LocationName), 60));
                Temp.X = wow.ReadFloat(playerbase + 0x798); // Read players xlocation
                Temp.Y = wow.ReadFloat(playerbase + 0x79C); // Read players ylocation
                Temp.Z = wow.ReadFloat(playerbase + 0x7A0); // Read players zlocation
                double Time = (DateTime.Now - LastRead).TotalMilliseconds;
                Speed = WowControl.CheckPoint(Temp.X, Temp.Y, Temp.Z, X, Y, Z) / Time;
                X = Temp.X;
                Y = Temp.Y;
                Z = Temp.Z;
                Temp.R = wow.ReadFloat(playerbase + 0x7A8); // Read players rotation
                Temp.IsInCombat = (wow.ReadUInt(wow.ReadUInt(playerbase + 208) + 212) >> 19) & 1;
                Temp.Faction = GetFaction(wow.ReadUInt(wow.ReadUInt(playerbase + 0x8) + (0x37 * 4))); //Faction               
				Temp.Type = 4;
                Temp.GUID = wow.ReadUInt64(playerbase + 0x30);
                Objects.Add(Temp);
                LastRead = DateTime.Now;
				uint s_curMgr = wow.ReadUInt(wow.ReadUInt((uint)ObjectManager.CurMgrPointer) + (uint)ObjectManager.CurMgrOffset);
                uint curObj = wow.ReadUInt(s_curMgr + 0xAC);
                uint nextObj = curObj;
                UInt64 playerGUID = wow.ReadUInt64((uint)Globals.PlayerGUID);
				UInt64 targetGUID = wow.ReadUInt64((uint)Globals.TargetGUID);
                UInt64 localGUID = wow.ReadUInt64(s_curMgr + 0xC0);
                while (curObj != 0 && (curObj & 1) == 0)
                {
                    UInt64 type = wow.ReadUInt(curObj + 0x14);
                    UInt64 cGUID = wow.ReadUInt64(curObj + 0x30);
                    Temp.Type = Convert.ToUInt32(type);
                    if (Temp.Type == 7)
                        Temp.Type = 7;
                    Temp.GUID = cGUID;
                    switch (type)
                    {
                        case 3:
                            Temp.IsInCombat = (wow.ReadUInt(wow.ReadUInt(curObj + 208) + 212) >> 19) & 1;
                            Temp.IsPlayer = false;
                            Temp.IsTarget = false;
                            Temp.Name = "Unit";
                            if (cGUID == targetGUID)
                                Temp.IsTarget = true;
                            Temp.X = wow.ReadFloat(curObj + 0x7D4);
                            Temp.Y = wow.ReadFloat(curObj + 0x7D8);
                            Temp.Z = wow.ReadFloat(curObj + 0x7DC);
                            Temp.R = wow.ReadFloat(curObj + 0x7A8);
                            Temp.Health = wow.ReadUInt(wow.ReadUInt(curObj + 0x8) + (0x18 * 4)); // Reads health
                            Temp.MaxHealth = wow.ReadUInt(wow.ReadUInt(curObj + 0x8) + (0x20 * 4)); // Reads maxhealth
                            Temp.Level = wow.ReadUInt(wow.ReadUInt(curObj + 0x8) + (0x36 * 4)); // Reads level
                            Temp.Name = ReadRealName(wow.ReadBytes(wow.ReadUInt(wow.ReadUInt(curObj + 0x964) + 0x5c), 60));
                            Temp.Faction = "Mob";
                            Temp.IsLootable = wow.ReadUInt(wow.ReadUInt(curObj + 0x8) + (0x4F * 4));
                            Temp.Auras = getAuras(curObj);
                            Temp.Power = GetUnitPower(curObj);
                            if (cGUID == targetGUID)
                            {
                                Temp.IsTarget = true;
                                Target = Temp;
                                Target.Auras = getAuras(curObj);
                            }
                            Objects.Add(Temp);
                            break;
                        case 4:
                            Temp.IsInCombat = (wow.ReadUInt(wow.ReadUInt(curObj + 208) + 212) >> 19) & 1;
                            Temp.IsTarget = false;
                            Temp.IsPlayer = false;
                            Temp.Name = GetPlayerNameByGuid(cGUID);
                            Temp.X = wow.ReadFloat(curObj + 0x7D4);
                            Temp.Y = wow.ReadFloat(curObj + 0x7D8);
                            Temp.Z = wow.ReadFloat(curObj + 0x7DC);
                            Temp.R = wow.ReadFloat(curObj + 0x7A8);
                            Temp.Health = wow.ReadUInt(wow.ReadUInt(curObj + 0x8) + (0x18 * 4)); // Reads health
                            Temp.MaxHealth = wow.ReadUInt(wow.ReadUInt(curObj + 0x8) + (0x20 * 4)); // Reads maxhealth
                            Temp.Level = wow.ReadUInt(wow.ReadUInt(curObj + 0x8) + (0x36 * 4)); // Reads level
                            Temp.Faction = GetFaction(wow.ReadUInt(wow.ReadUInt(curObj + 0x8) + (0x37 * 4))); //Faction
                            Temp.Auras = getAuras(curObj);
                            Temp.Power = GetUnitPower(curObj);
                            if (cGUID == targetGUID)
                            {
                                Temp.IsTarget = true;
                                Target = Temp;
                                Target.Auras = getAuras(curObj);
                            }
                            else if (cGUID == playerGUID)
                                break;
							if (Objects[0].GUID != Temp.GUID)
                            Objects.Add(Temp);
                            break;
                        case 5:
                            Temp.Faction = "Object";
                            Temp.IsInCombat = 0;
                            Temp.Name = "";
                            Temp.IsPlayer = false;
                            Temp.IsTarget = false;
                            if (cGUID == targetGUID)
                                Temp.IsTarget = true;
                            Temp.Level = 0;
                            Temp.Health = 0;
                            Temp.MaxHealth = 0;
                            Temp.X = wow.ReadFloat(curObj + 0xE8);
                            Temp.Y = wow.ReadFloat(curObj + 0xEC);
                            Temp.Z = wow.ReadFloat(curObj + 0xF0);
                            Temp.R = wow.ReadFloat(curObj + 0x7A8);
                            Temp.Name = ReadRealName(wow.ReadBytes(wow.ReadUInt(wow.ReadUInt(curObj + 0x1A4) + 0x90), 60));
                            Temp.Power = GetUnitPower(curObj);
                            Objects.Add(Temp);
                            break;
                    }
                    nextObj = wow.ReadUInt(curObj + 0x3C);
                    if (nextObj == curObj)
                        break;
                    else
                        curObj = nextObj;
                }
                Objects.Add(Target);
                WowControl.TargetBuffs.Clear();
                if (Target.Auras != null)
                    for (int i = 0; i < Target.Auras.Count; i++)
                        WowControl.TargetBuffs.Add(Target.Auras[i].auraId);
                if (LastError == "Нет места.")
                    WowControl.FullBag = true;
            }
            catch (Exception)
            {
                //WowControl.UpdateStatus("Error. " + E.Message);
                InitMemory();
            }

        }
        public void SelectTarget(UInt64 GUID)
        {
            wow.WriteUInt64(0x00C4EB48, GUID);
        }
        public void SetLoot()
        {
            wow.WriteInt(wow.ReadUInt(playerbase + 0xD8) + 0x1020, 1);
        }
        public void ReadChat()
        {
            String NewString;
            for (uint i = 0; i < 59; i++)
            {
				String str = ReadRealName(wow.ReadBytes(((uint)WoWChat.ChatBufferStart + (i * (uint)WoWChat.NextMessage)) + 0x3c, 1500));
                if (str == "") return;
                int Type = Convert.ToInt32(GetTextFromChatString(str, "Type"));
                String Channel = GetTextFromChatString(str, "Channel");
                String PlayerName = GetTextFromChatString(str, "Player Name");
                String Text = GetTextFromChatString(str, "Text");
                if (Type == 1)//Say
                {
                    if (Objects[0].Name == PlayerName)
                    {
                        WowControl.ChatBug = true;
                        WowControl.ChatBugTime = DateTime.Now;
                    }
                    NewString = " [" + PlayerName + "] " + Text;
                    if (WowControl.SayChat[i] != NewString)
                    {
                        WowControl.SayChat[i] = NewString;
                        WowControl.SayChatParsed.Add(WowControl.GetTimeStr() + NewString);
                    }
                }
                else if ((Type == 7) || (Type == 23) || (Type == 9))//Whisper
                {
                    NewString = " [" + PlayerName + "] " + Text;
                    if (WowControl.WisperChat[i] != NewString)
                    {
                        WowControl.WisperChat[i] = NewString;
                        switch (Type)
                        {
                            case 7:
                                NewString = " [" + PlayerName + "] <== " + Text;
                                WowControl.WisperChatParsed.Add(WowControl.GetTimeStr() + NewString);
                                break;
                            case 23:
                                NewString = " [" + PlayerName + "] <" + Text + ">";
                                WowControl.WisperChatParsed.Add(WowControl.GetTimeStr() + NewString);
                                break;
                            case 9:
                                NewString = " [" + PlayerName + "] ==> " + Text;
                                WowControl.WisperChatParsed.Add(WowControl.GetTimeStr() + NewString);
                                break;
                        }
                    }
                }
                else//All
                {
                    NewString = " [" + PlayerName + "] " + Text;
                    if (WowControl.AllChat[i] != NewString)
                    {
                        WowControl.AllChat[i] = NewString;
                        WowControl.AllChatParsed.Add(WowControl.GetTimeStr() + NewString);
                    }
                }
            }
        }
        public String GetTextFromChatString(String InptStr, String Pattern)
        {
            int Start = 0;
            for (int i = 0; i < InptStr.Length; i++)
            {
                if (Start==0)
                if (InptStr.Substring(i, Pattern.Length) == Pattern)
                {
                    Start = i + Pattern.Length + 3;
                }
                String T = InptStr.Substring(i, 1);
                if ((Start != 0) & (InptStr.Substring(i, 1) == "]"))
                    return InptStr.Substring(Start, i - Start);
            }
            return "";
        }
        public String ReadRealName(byte[] InputName)
        {
            if (InputName == null)
                return "";
            string playernam = Encoding.UTF8.GetString(InputName);
            for (int i = 0; i < playernam.Length; i++)
            {
                string t = playernam.Substring(i, 1);
                if (t == "\0")
                {
                    playernam = playernam.Substring(0, i);
                    return playernam;
                }
            }
            return playernam;
        }
        /// <summary>
        /// Going to coords/guid 
        ///7 - подойти и полутать(Move to + Interact (OBJECT ONLY))
        ///11 - Лететь и ебашить(Attack GUID pos)
        ///4 - лететь в координату(Walk to Assigned POS)
        ///6 -	Loot
        /// </summary>
        /// <param name="X"></param>
        /// <param name="Y"></param>
        /// <param name="Z"></param>
        /// <param name="GUID"></param>
        /// <param name="ActionType"></param>
        public void GoTo(float X, float Y, float Z, UInt64 GUID, int ActionType)
        {
            uint ctmBase = (uint)ClickToMove.CTM_Base;
            uint ctmTurnScale = ctmBase + 0x04;
            uint ctmDistance = ctmBase + 0x0c;
            uint ctmAction = ctmBase + 0x1c;
            uint ctmTarget = ctmBase + 0x20;
            uint ctmDestinationX = ctmBase + 0x8C;
            uint ctmDestinationY = ctmDestinationX + 0x4;
            uint ctmDestinationZ = ctmDestinationY + 0x4;
            switch (ActionType)
            {
                case 0:
                    wow.WriteUInt64(ctmTarget, 0);
                    wow.WriteFloat(ctmDistance, 0f); // ctm distance
                    wow.WriteFloat(ctmDestinationX, 0);//ctm x
                    wow.WriteFloat(ctmDestinationY, 0);//ctm y
                    wow.WriteFloat(ctmDestinationZ, 0);//ctm z
                    break;
                case 1:
                    wow.WriteUInt64(ctmTarget, GUID);
                    wow.WriteFloat(ctmDistance, 30.5f); // ctm distance
                    break;
                case 3:
                    wow.WriteUInt64(ctmTarget, 0);
                    wow.WriteFloat(ctmDistance, 30.5f); // ctm distance
                    break;
                case 4:
                    if (Abilities.AbilityStatus == "Casting")
                        return;
                    wow.WriteFloat(ctmDestinationX, X);//ctm x
                    wow.WriteFloat(ctmDestinationY, Y);//ctm y
                    wow.WriteFloat(ctmDestinationZ, Z);//ctm z
                    wow.WriteFloat(ctmDistance, 0.5f); // ctm distance
                    break;
                case 6:
                    wow.WriteUInt64(ctmTarget, GUID);
                    wow.WriteFloat(ctmDistance, 3.66666650772095f); // ctm distance
                    break;
                case 7:
                    wow.WriteUInt64(ctmTarget, GUID);
                    wow.WriteFloat(ctmDistance, 3.66666650772095f); // ctm distance
                    break;
                case 9:
                    wow.WriteUInt64(ctmTarget, GUID);
                    wow.WriteFloat(ctmDistance, 3.66666650772095f); // ctm distance
                    break;
                case 11:
                    if ((Abilities.AbilityStatus == "Casting") & (WowControl.ClassType == 0))
                        return;
                    wow.WriteUInt64(ctmTarget, GUID);
                    wow.WriteFloat(ctmDistance, 3.66666650772095f); // ctm distance
                    break;
            }
            wow.WriteInt(ctmAction, ActionType); // ctm action
        }
        public string GetPlayerNameByGuid(ulong guid)
        {
            const uint nameStorePtr = (uint)Globals.PlayerNameCache + 0x8; // Player name cache
            const uint nameMaskOffset = 0x24;
            const uint nameBaseOffset = 0x1C;
            const uint nameStringOffset = 0x20;
            uint base_addr, offset, current, testGUID;
            uint mask = wow.ReadUInt(nameStorePtr + nameMaskOffset);
            base_addr = wow.ReadUInt(nameStorePtr + nameBaseOffset);
            uint shortGUID = (uint)guid & 0xffffffff;
            uint x = (mask & shortGUID);
            offset = 12 * x;
            current = wow.ReadUInt((base_addr + offset + 8));
            offset = wow.ReadUInt((base_addr + offset));
            if ((current == 0) || (current & 0x1) == 0x1)
                return "Unknown";
            testGUID = wow.ReadUInt((current));
            string testName = ReadRealName(wow.ReadBytes((current + nameStringOffset), 40));
            while (testGUID != shortGUID)
            {
                current = wow.ReadUInt((current + offset + 4));
                if ((current == 0) || (current & 0x1) == 0x1)
                    return "Unknown";
                testGUID = wow.ReadUInt(current);
            }
            // Found the guid in the name list...
            string str = ReadRealName(wow.ReadBytes((current + nameStringOffset), 40));
            return str;
        }
        private String GetFaction(uint FactionIndex)
        {
            switch (FactionIndex)
            {
                case 1:
                    return "Alliance";
                case 2:
                    return "Horde";
                case 3:
                    return "Alliance";
                case 4:
                    return "Alliance";
                case 5:
                    return "Horde";
                case 6:
                    return "Horde";
                case 115:
                    return "Alliance";
                case 116:
                    return "Horde";
                case 1610:
                    return "Horde";
                case 1629:
                    return "Alliance";
            }
            return "Unknown";
        }
        public void SetPlayerToCombat()
        {
            ObjectInfo[] temp = Objects.ToArray();
            temp[0].IsInCombat = 1;
            Objects.Clear();
            for (int i = 0; i < temp.Length; i++)
                Objects.Add(temp[i]);
        }
    }
}
