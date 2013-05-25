using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using System.Diagnostics;
using System.IO;
using Memory;
using System.Windows.Forms;

namespace FarmHelper_beta
{
    public static class WowControl
    {
        public static uint hWnd;
        [DllImport("USER32.DLL")]
        private static extern void PostMessage(uint a, int b, int c, int d);
        [DllImport("Kernel32.dll")]
        public static extern void Sleep(int s);
        public static MainForm.WayPoints[] WP;
        public static DateTime BotStartTime, StuckStart, WaitTime, ChatBugTime, ChatBugFix, LocationChangeTime;
        public static double LastSpeed;
        public static List<ulong> BadObjects = new List<ulong>();
        public static List<uint> PlayerBuffs = new List<uint>();
        public static List<uint> TargetBuffs = new List<uint>();
        public static String FarmStatus = "Not started", Account, Password;
		public static String Status = "Ready", LoadedWP = "", ErrorState = "";
        public static List<String> Loot = new List<String>();
        public static List<String> CombatLog = new List<string>();
        public static String[] SayChat = new String[60];
        public static String[] WisperChat = new String[60];
        public static String[] AllChat = new String[60];
        public static List<String> Config = new List<string>();
        public static List<String> AllChatParsed = new List<string>();
        public static List<String> WisperChatParsed = new List<string>();
        public static List<String> SayChatParsed = new List<string>();
        public static List<String> Log = new List<string>();
        public static List<String> Goods = new List<string>();
		public static String[] AllSpells = new String[0];
        public static bool Debugging, ChatBug, SkiningKillAll, FullBag, SkinAll,FarmHonor, FarmSkinning, FarmMining, LootMobs, StayWithGroup;
        public static int ClassType, InDoors, InWater, SkiningRange, FarmMount;
        public static int FriendCheckRange, EnemyCheckRange, HealDDPriority;
        public static int GroupRange;
        public static int RangeFromPlayerToGroup;
        public static int MinPlayersInGroup;
        public struct LootObject
        {
            public String Name;
            public String Time;
            public ulong Guid;
        }
		public static int GetUnitIndexByGuid(Memory.Memory wow, UInt64 GUID)
		{
			for (int i = 0; i < wow.Objects.Count; i++)
				if (wow.Objects[i].GUID == GUID)
					return i;
			return -1;
		}
        public static void UpdateStatus(String Status)
        {
            try
            {
                if (Log.ToArray().Length == 0)
                    Log.Add(GetTimeStr() + " " + Status);
                else
                    if (Log[Log.ToArray().Length - 1].Substring(11, Log[Log.ToArray().Length - 1].Length - 11) != Status)
                        Log.Add(GetTimeStr() + " " + Status);
                File.WriteAllLines(Application.StartupPath + "\\Logs\\Log " + DateTime.Now.Date.ToShortDateString()+ ".txt", Log.ToArray());
            }
            catch (Exception) { }
        }
        public static void UpdateCombatLog(String Status)
        {
            if (CombatLog.ToArray().Length == 0)
            {
                CombatLog.Add(GetTimeStr() + " " + Status);
                return;
            }
            else
            {
                if (CombatLog[CombatLog.ToArray().Length - 1].Substring(11, CombatLog[CombatLog.ToArray().Length - 1].Length - 11) != Status)
                    CombatLog.Add(GetTimeStr() + " " + Status);
            }
        }
        public static void SaveVariable(bool Var, String VarString)
        {
            Config.Add(VarString + "=" + Var.ToString() + ";");
        }
        public static void SaveVariable(int Var, String VarString)
        {
            Config.Add(VarString + "=" + Var.ToString() + ";");
        }
        public static void SaveVariable(String Var, String VarString)
        {
            if (Var != null) Config.Add(VarString + "=" + Var.ToString() + ";");
            else Config.Add(VarString + "=null;");
        }
        public static bool LoadVariableBool(String VarName)
        {
            String FindedVarName = "";
            String FindedVar = "";
            for (int i = 0; i < Config.Count; i++)
            {
                for (int n = 0; n < Config[i].Length; n++)
                    if (Config[i].Substring(n, 1) == "=")
                    {
                        FindedVarName = Config[i].Substring(0, n);
                        if (VarName == FindedVarName)
                        {
                            FindedVar = Config[i].Substring(n + 1, Config[i].Length - n - 2);
                            return Convert.ToBoolean(FindedVar);
                        }
                    }
            }
            return false;
        }
        public static int LoadVariableInt(String VarName)
        {
            String FindedVarName = "";
            String FindedVar = "";
            for (int i = 0; i < Config.Count; i++)
            {
                for (int n = 0; n < Config[i].Length; n++)
                    if (Config[i].Substring(n, 1) == "=")
                    {
                        FindedVarName = Config[i].Substring(0, n);
                        if (VarName == FindedVarName)
                        {
                            FindedVar = Config[i].Substring(n + 1, Config[i].Length - n - 2);
                            return Convert.ToInt32(FindedVar);
                        }
                    }
            }
            return 0;
        }
        public static String LoadVariableString(String VarName)
        {
            String FindedVarName = "";
            String FindedVar = "";
            for (int i = 0; i < Config.Count; i++)
            {
                for (int n = 0; n < Config[i].Length; n++)
                    if (Config[i].Substring(n, 1) == "=")
                    {
                        FindedVarName = Config[i].Substring(0, n);
                        if (VarName == FindedVarName)
                        {
                            FindedVar = Config[i].Substring(n + 1, Config[i].Length - n - 2);
                            return FindedVar;
                        }
                    }
            }
            return "";
        }
        public static void LoadCfgFromFile(String File)
        {
            int BindStart = 0, BindEnd = 0;
            Config.Clear();
            String[] Temp = System.IO.File.ReadAllLines(File);
            for (int i = 0; i < Temp.Length; i++)
            {
                Config.Add(Temp[i]);
                if (Temp[i] == "[Bindings]")
                    BindStart = i + 1;
                if (Temp[i] == "[End]")
                    BindEnd = i;
            }
            LoadBinds(BindStart, BindEnd, Temp);
        }
        public static void SaveGoods()
        {
            File.WriteAllLines(Application.StartupPath + "\\Data\\Goods.fh", Goods.ToArray());
        }
        public static int LoadBinds(int ConfigStart, int ConfigEnd, String[] Config)
        {
            Abilities.Ability Temp = new Abilities.Ability();
            Abilities.BindingsList.Clear();
            int[] Slashes = new int[11];
            bool FoundedAbility = false;
            int AbilityIndex = 0;
            for (int i = ConfigStart; i < ConfigEnd; i++)
            {
                int Start = 0;
                int End = 0;
                int SlashCount = 0;
                FoundedAbility = false;
                for (int n = 0; n < Config[i].Length; n++)
                {
                    String TempStr = Config[i].Substring(n, 1);
                    if (TempStr == "%")
                    {
                        Start = n;
                        FoundedAbility = true;
                    }
                    if (TempStr == ";")
                        End = n;
                    if (TempStr == "\\")
                    {
                        Slashes[SlashCount] = n;
                        SlashCount++;
                    }
                }
                if (FoundedAbility == true)
                {
                    try
                    {
                        Temp.AbilityName = Config[i].Substring(0, Start);
                        Temp.AbilityTxt = Config[i].Substring(Start + 1, (Slashes[0] - 1 - Start));
                        Temp.Key = Convert.ToInt32(Config[i].Substring(Slashes[0] + 1, (Slashes[1] - 1 - Slashes[0])));
                        Temp.NotEditable = Convert.ToBoolean(Config[i].Substring(Slashes[1] + 1, (Slashes[2] - 1 - Slashes[1])));
                        Temp.Modifiers = Convert.ToString(Config[i].Substring(Slashes[2] + 1, (Slashes[3] - 1 - Slashes[2])));
                        Temp.CastTime = (float)Convert.ToDouble(Config[i].Substring(Slashes[3] + 1, (Slashes[4] - 1 - Slashes[3])));
                        Temp.CoolDown = (float)Convert.ToDouble(Config[i].Substring(Slashes[4] + 1, (Slashes[5] - 1 - Slashes[4])));
                        Temp.InCombat = Convert.ToInt32(Config[i].Substring(Slashes[5] + 1, (Slashes[6] - 1 - Slashes[5])));
                        Temp.AbilityType = Convert.ToInt32(Config[i].Substring(Slashes[6] + 1, (End - 1 - Slashes[6])));
                    } catch (Exception) { WowControl.Log.Add(WowControl.GetTimeStr() + "Got error while loading binds. Maybe old version cfg."); }
                    Abilities.BindingsList.Add(Temp);
                    int OptionsCount = GetOptionsListCount(Config, i, Temp.AbilityName);
                    if (OptionsCount > 0)
                        LoadOptionsList(Config, i + 1, OptionsCount, AbilityIndex);
                    AbilityIndex++;
                }
            }
            return 0;
        }
        public static int GetOptionsListCount(String[] Config, int Start, String AbilityName)
        {
            int result = 0;
            for (int i = Start; i < Config.Length - 1; i++)
            {
                String Temp=Config[i].Substring(0, AbilityName.Length + 1);
                if (Temp == AbilityName + " ")
                    result++;
            }
            return result;
        }
        public static void LoadOptionsList(String[] Config, int OptionsStart, int OptionsCount, int AbilityIndex)
        {
            Abilities.Ability[] TempBinds = Abilities.BindingsList.ToArray();
            TempBinds[AbilityIndex].OptionsList = new List<Abilities.AbilityOptions>();
            int[] Spaces = new int[11];
            for (int i = OptionsStart; i < OptionsStart + OptionsCount; i++)
            {
                int Start = 0;
                int End = 0;
                int SpacesCount = 0;
                for (int n = 0; n < Config[i].Length; n++)
                {
                    String TempStr = Config[i].Substring(n, 1);
                    if (TempStr == ":")
                    {
                        Start = n + 1;
                    }
                    if (TempStr == ";")
                        End = n;
                    if (TempStr == "\\")
                    {
                        Spaces[SpacesCount] = n;
                        SpacesCount++;
                    }
                }
                Abilities.AbilityOptions Temp = new Abilities.AbilityOptions();
                Temp.Unit = Config[i].Substring(Start + 1, (Spaces[0] - 1 - Start));
                Temp.Parameter = Config[i].Substring(Spaces[0] + 1, (Spaces[1] - 1 - Spaces[0]));
                Temp.Condition = Config[i].Substring(Spaces[1] + 1, (Spaces[2] - 1 - Spaces[1]));
                Temp.Value = Config[i].Substring(Spaces[2] + 1, (Spaces[3] - 1 - Spaces[2]));
                Temp.AndOr = Config[i].Substring(Spaces[3] + 1, (End - 1 - Spaces[3]));
                TempBinds[AbilityIndex].OptionsList.Add(Temp);
            }
            Abilities.BindingsList.Clear();
            for (int i = 0; i < TempBinds.Length; i++)
                Abilities.BindingsList.Add(TempBinds[i]);
        }
        public static void Command(String s)
        {
            if ((DateTime.Now - WowControl.LocationChangeTime).TotalSeconds < 60)
                return;
            /*if ((ChatBug == true) & (WowControl.ChatBugTime > WowControl.ChatBugFix))
            {
                WowControl.ChatBugFix = DateTime.Now;
                btn(0xd);//Enter
                ChatBug = false;
            }*/
            btn(0xd);//Enter
            Sleep(100);
            PostMessage(hWnd, 0x0102, 47, 1);//WM_Char (Slash)
            for (int i = 0; i < (s.Length); i++)
                PostMessage(hWnd, 0x0102, s.ToCharArray()[i], 1);//WM_Char
            Sleep(100);
            btn(0xd);//Enter
        }
        public static void KeyToWnd(int key, bool KeyUpDown)
        {
            if (KeyUpDown == true)
                PostMessage(hWnd, 0x0101, key, 1);//WM_KEYUP}
            else
                PostMessage(hWnd, 0x0100, key, 1);//WM_KEYDOWN
        }
        public static void Test()
        {
            String t = "Анбеливыбыл";
            for (int i = 0; i < t.Length; i++)
                PostMessage(hWnd, 0x0102, t.ToCharArray()[i], 1);//WM_Char
        }
        public static void btn(int B)
        {
            PostMessage(hWnd, 0x0100, B, 1);//WM_KEYDOWN
            PostMessage(hWnd, 0x0101, B, 1);//WM_KEYUP
        }
        public static void LoadGoods()
        {
            Goods.Clear();
            String[] ReadedGoods = File.ReadAllLines(Application.StartupPath + "\\Data\\Goods.fh");
            for (int i = 0; i < ReadedGoods.Length; i++)
                Goods.Add(ReadedGoods[i]);
        }
        public static MainForm.WayPoints[] LoadWaypoints(String Location)
        {
            String[] WPTxt = File.ReadAllLines(Application.StartupPath + "\\Data\\Waypoints.fh");
            List<MainForm.WayPoints> Result = new List<MainForm.WayPoints>();
            MainForm.WayPoints Temp = new MainForm.WayPoints();
            for (int i = 0; i < WPTxt.Length; i++)
            {
                int Count = 0;
                int S1 = 0, S2 = 0, S3 = 0, S4 = 0, S5 = 0, S6 = 0, S7 = 0;
                for (int n = 0; n < WPTxt[i].Length; n++)
                {
                    if (WPTxt[i].Substring(n, 1) == " ")
                    {
                        switch (Count)
                        {
                            case 0:
                                S1 = n;
                                Count++;
                                break;
                            case 1:
                                S2 = n;
                                Count++;
                                break;
                            case 2:
                                S3 = n;
                                Count++;
                                break;
                            case 3:
                                S4 = n;
                                Count++;
                                break;
                            case 4:
                                S5 = n;
                                Count++;
                                break;
                        }
                    }
                    if (WPTxt[i].Substring(n, 1) == ".")
                        S6 = n;
                    if (WPTxt[i].Substring(n, 1) == ";")
                        S7 = n;
                }
                Temp.X = (float)Convert.ToDouble(WPTxt[i].Substring(0, S1));
                Temp.Y = (float)Convert.ToDouble(WPTxt[i].Substring(S1, S2 - S1));
                Temp.Z = (float)Convert.ToDouble(WPTxt[i].Substring(S2, S3 - S2));
                Temp.NextWaypoint = (int)Convert.ToDouble(WPTxt[i].Substring(S3, S4 - S3));
                String Loc = WPTxt[i].Substring(S6 + 1, S7 - S6 - 1);
                if (Loc == Location)
                    Result.Add(Temp);
            }
            LoadedWP = Location;
            if (Result.ToArray().Length > 0)
                UpdateStatus("Waypoints [" + Result.ToArray().Length.ToString() + "] loaded for " + LoadedWP + ".");
            else
                UpdateStatus("Waypoints missing for " + Location + ".");
            return Result.ToArray();
        }
        public static double CheckPoint(float X1, float Y1, float Z1, float X2, float Y2, float Z2)
        {
            double RangeXY = Math.Sqrt(Math.Pow(X2 - X1, 2) + Math.Pow(Y2 - Y1, 2));
            double Range = Math.Sqrt(Math.Pow(RangeXY, 2) + Math.Pow(Z2 - Z1, 2));
            return Range;
        }
        public static int GetClosestTarget(Memory.Memory.ObjectInfo[] Objects, String Faction, int IsAlive, int Range, int Type)
        {
            double MinRange = 10000;
            int ClosestTargetIndex = 0;
            for (int i = 0; i < Objects.Length; i++)
            {
				if (WowControl.CheckForBad(Objects[i].GUID) == false)
				{

					if ((Objects[i].Type == Type) & (Objects[i].Faction == Faction) & (Objects[i].Health >= IsAlive) & (Objects[i].Level > 1))
					{
						if (CheckPoint(Objects[i].X, Objects[i].Y, Objects[i].Z, Objects[0].X, Objects[0].Y, Objects[0].Z) < MinRange)
						{
							ClosestTargetIndex = i;
							MinRange = CheckPoint(Objects[i].X, Objects[i].Y, Objects[i].Z, Objects[0].X, Objects[0].Y, Objects[0].Z);
						}
					}
				}
            }
            if ((ClosestTargetIndex != 0) & (MinRange < Range))
            {

                return ClosestTargetIndex;
            }
            else return 0;
        }
        public static String GetTimeStr()
        {
            DateTime DT = DateTime.Now;
            String ResultTime;
            if (DT.Hour > 9)
                ResultTime = "[" + DT.ToLongTimeString() + "]";
            else
                ResultTime = "[0" + DT.ToLongTimeString() + "]";
            return ResultTime;
        }
        public static string RandomString(int size)
        {
            StringBuilder builder = new StringBuilder();
            Random random = new Random();
            char ch;
            int lu;
            int r;
            for (int i = 0; i < size; i++)
            {
                ch = Convert.ToChar(Convert.ToInt32(Math.Floor(26 * random.NextDouble() + 65)));
                lu = Convert.ToInt32(Math.Floor(26 * random.NextDouble() + 65));
                Math.DivRem(lu, 2, out r);
                String Out;
                if (r == 1)
                    Out = ch.ToString().ToUpper();
                else
                    Out = ch.ToString().ToLower();
                builder.Append(Out);
            }
            return builder.ToString();
        }
        public static bool Stuck(double Speed)
        {
            if ((LastSpeed > 0) & (Speed < 0.006))
            {
                StuckStart = DateTime.Now;
                LastSpeed = Speed;
            }
            if (Speed > 0.006)
            {
                StuckStart = DateTime.Now;
                LastSpeed = Speed;
            }
            if ((DateTime.Now - StuckStart).TotalSeconds > 5)
                return true;
            else return false;
        }
        public static void GetOnMount(Memory.Memory wow, int FlyOrGround)
        {
            if (Abilities.AbilityStatus != "Casting")
                if ((wow.IsMounted == 0) & (wow.IsIndoors == 0))
                {
                    wow.GoTo(0, 0, 0, 0, 3);//Stop
                    WowControl.Sleep(500);
                    WowControl.FarmStatus = "Mounting";
                    if (FlyOrGround == 0)
                        Abilities.Button("Fly mount");
                    else Abilities.Button("Ground mount");
                }
            if (wow.LastError == "Нельзя использовать в воде.")
            {
                InWater = 1;
                WowControl.FarmStatus = "In water";
            }
            else InWater = 0;
            if (wow.LastError == "Здесь нельзя ездить верхом.")
                InDoors = 1;
            else InDoors = 0;
        }
        public static bool FindTextInString(String SourceStr, String FindStr)
        {
            bool result = false;
            for (int i = 0; i < SourceStr.Length - FindStr.Length; i++)
                if (SourceStr.Substring(i, FindStr.Length) == FindStr)
                    result = true;
            return result;
        }
        public static void Login()
        {
            btn(0x2e);//Delete
            Sleep(1000);
            for (int i = 0; i < Account.Length; i++)
            {
                Char key = Account.ToUpper().ToCharArray()[i];
                if (key == '.')
                    PostMessage(hWnd, 0x0102, 0x2e, 1);//WM_Char
                else if (key == '@')
                    PostMessage(hWnd, 0x0102, 0x40, 1);//WM_Char
                else
                    PostMessage(hWnd, 0x0102, key, 1);//WM_Char
            }
            Sleep(100);
            btn(0x9);//Tab
            for (int i = 0; i < Password.Length; i++)
                PostMessage(hWnd, 0x0102, Password.ToUpper().ToCharArray()[i], 1);//WM_Char
            btn(0xd);//Enter
            WowControl.WaitTime = DateTime.Now.AddSeconds(10);
            Sleep(5000);
            return;
        }
        public static void CharSelect()
        {
            if (WowControl.WaitTime > DateTime.Now)
                return;
            btn(0xd);//Enter
            WaitTime = DateTime.Now.AddSeconds(20);
        }
		public static void CheckLastError(String ErrString)
		{
			if (ErrString == "Нельзя применить к этой цели.")
				ErrorState = "Bad target";
			/*else if (ErrString == "Цель вне поля зрения.")
				ErrorState = "Bad target";*/
			else ErrorState = "";
		}
		public static bool CheckForBad(UInt64 GUID)
		{
			bool result = false;
			for (int i = 0; i < BadObjects.Count; i++)
			{
				if (BadObjects[i] == GUID)
					result = true;
			}
			return result;
		}
		public static void Stop()
		{
			Memory.Memory wow = new Memory.Memory();
			wow.GoTo(0, 0, 0, 0, 3);
		}
		public static void SelectTarget(UInt64 GUID)
		{
			Memory.Memory wow = new Memory.Memory();
			wow.SelectTarget(GUID);
		}
    }
}
