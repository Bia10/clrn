using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

namespace FarmHelper_beta
{
    public static class Abilities
    {
        public static List<Ability> BindingsList = new List<Ability>();
		public static AbilityPrioritet[,] Prioritets;
        public static String AbilityStatus;
        public static DateTime StartCast, EndCast;
		public static int Target;
        public struct AbilityOptions
        {
            public String Unit;
            public String Parameter;
            public String Condition;
            public String Value;
            public String AndOr;
        }
        public struct Ability
        {
            public String AbilityName;
            public String AbilityTxt;
            public String Modifiers;
            public int Key;
            public float CoolDown;
            public bool NotEditable;
            public int InCombat;
            public int Try;
            public float CastTime;
            public int AbilityType;
            public DateTime LastUse;
            public List<AbilityOptions> OptionsList;
        }
		public struct AbilityPrioritet
		{
			public UInt64 GUID;
			public int Prioritet;
		}
        public enum Buffs : uint
        {
            Cat = 768,
            Bear = 9634,
            Travel = 783,
            Tree = 33891,
            CatStealth = 5215,
            RogueStealth = 1784,
            Fly = 40120
        }
        public static String GetBuff(List<Memory.Memory.AURA> AllBuffs)
        {
            for (int i = 0; i < AllBuffs.Count; i++)
            {
                switch (AllBuffs[i].auraId)
                {
                    case (uint)Buffs.Bear:
                        return "Bear";
                    case (uint)Buffs.Cat:
                        return "Cat";
                    case (uint)Buffs.CatStealth:
                        return "CatStealth";
                    case (uint)Buffs.Fly:
                        return "Fly";
                    case (uint)Buffs.RogueStealth:
                        return "RogueStealth";
                    case (uint)Buffs.Travel:
                        return "Travel";
                    case (uint)Buffs.Tree:
                        return "Tree";
                }
            }
            return "";
        }
        public static void InitBinds()
        {
            BindingsList.Clear();
            Ability Temp = new Ability();
            Temp.NotEditable = true;
            Temp.CoolDown = 0;
            Temp.AbilityName = "Jump";
            Temp.AbilityTxt = "";
            Temp.InCombat = 3;
            Temp.AbilityType = 0;
            BindingsList.Add(Temp);
            Temp.AbilityName = "Fly mount";
            Temp.AbilityTxt = "";
            Temp.InCombat = 2;
            Temp.CastTime = 2.5f;
            Temp.InCombat = 3;
            Temp.AbilityType = 1;
            BindingsList.Add(Temp);
            Temp.CastTime = 2.5f;
            Temp.AbilityName = "Ground mount";
            Temp.AbilityTxt = "";
            Temp.InCombat = 3;
            Temp.AbilityType = 1;
            BindingsList.Add(Temp);
            Temp.CastTime = 4.0f;
            Temp.AbilityName = "Mote xtrtr";
            Temp.AbilityTxt = "";
            Temp.InCombat = 4;
            Temp.AbilityType = 1;
            BindingsList.Add(Temp);
        }
        public static void MakeBinds()
        {
            WowControl.Config.Add("[Bindings]");
            for (int i = 0; i < BindingsList.Count; i++)
            {
                WowControl.Config.Add(BindingsList[i].AbilityName + "%" + BindingsList[i].AbilityTxt + "\\" + BindingsList[i].Key.ToString() + "\\" + BindingsList[i].NotEditable.ToString() + "\\" + BindingsList[i].Modifiers + "\\" + (BindingsList[i].CastTime * 1.0f).ToString() + "\\" + (BindingsList[i].CoolDown * 1.0f).ToString() + "\\" + BindingsList[i].InCombat.ToString() + "\\" + BindingsList[i].AbilityType.ToString() + ";");
                try
                {
                    for (int r = 0; r < BindingsList[i].OptionsList.Count; r++)
                    {
                        WowControl.Config.Add(BindingsList[i].AbilityName + " options" + r.ToString() + ": " + BindingsList[i].OptionsList[r].Unit + "\\" + BindingsList[i].OptionsList[r].Parameter + "\\" + BindingsList[i].OptionsList[r].Condition + "\\" + BindingsList[i].OptionsList[r].Value + "\\" + BindingsList[i].OptionsList[r].AndOr + ";");
                    }
                }
                catch (Exception) { }
                //WowControl.Config.Add(BindingsList[i].AbilityName + "=" + BindingsList[i].AbilityTxt + "\\" + BindingsList[i].Key.ToString() + "\\" + BindingsList[i].NotEditable.ToString() + "\\" + BindingsList[i].Modifiers + "\\" + (BindingsList[i].CastTime * 1.0f).ToString() + "\\" + (BindingsList[i].CoolDown * 1.0f).ToString() + "\\" + BindingsList[i].Options.ToString() + "\\" + BindingsList[i].InCombat.ToString() + "\\" + BindingsList[i].TargetPlayer.ToString() + "\\" + BindingsList[i].HpManaCp.ToString() + "\\" + BindingsList[i].MoreLessEqual.ToString() + "\\" + BindingsList[i].Value.ToString() + ";");
            }
        }
        public static bool CheckString(String Source, String Pattern)
        {
            for (int i = 0; i <= Source.Length - Pattern.Length; i++)
                if (Source.Substring(i, Pattern.Length) == Pattern)
                    return true;
            return false;
        }
        public static void Button(String AbilityName)
        {
            bool Founded = false;
            for (int i = 0; i < BindingsList.Count; i++)
                if (BindingsList[i].AbilityName == AbilityName)
                {//Shift, Control, Alt
                    if (BindingsList[i].Key != 0)
                        Founded = true;
                    Ability[] TempArray = BindingsList.ToArray();
                    TempArray[i].LastUse = DateTime.Now;
                    BindingsList.Clear();
                    for (int n = 0; n < TempArray.Length; n++)
                        BindingsList.Add(TempArray[n]);
					if (BindingsList[i].AbilityType == 0)//Instant
						AbilityStatus = "Instant";
					else
					{
						AbilityStatus = "Casting";
						WowControl.Stop();
					}
                    StartCast = DateTime.Now;
                    EndCast = DateTime.Now.AddMilliseconds(BindingsList[i].CastTime * 1000);
                    if (CheckString(BindingsList[i].Modifiers, "Alt") == true)
                        WowControl.KeyToWnd(0x12, false);
                    if (CheckString(BindingsList[i].Modifiers, "Control") == true)
                        WowControl.KeyToWnd(0x11, false);
                    if (CheckString(BindingsList[i].Modifiers, "Shift") == true)
                        WowControl.KeyToWnd(0x10, false);
                    WowControl.KeyToWnd(BindingsList[i].Key, false);
                    WowControl.KeyToWnd(BindingsList[i].Key, true);
                    if (CheckString(BindingsList[i].Modifiers, "Alt") == true)
                        WowControl.KeyToWnd(0x12, true);
                    if (CheckString(BindingsList[i].Modifiers, "Control") == true)
                        WowControl.KeyToWnd(0x11, true);
                    if (CheckString(BindingsList[i].Modifiers, "Shift") == true)
                        WowControl.KeyToWnd(0x10, true);
                }
            if ((Founded == false) & (AbilityName != "Mote xtrtr"))
                WowControl.UpdateStatus("Not found bind for ability: " + AbilityName + ".");
        }
        public static void Button(String AbilityName, int Duration)
        {
            for (int i = 0; i < BindingsList.Count; i++)
                if (BindingsList[i].AbilityName == AbilityName)
                {//Shift, Control, Alt
                    Ability[] TempArray = BindingsList.ToArray();
                    TempArray[i].LastUse = DateTime.Now;
                    BindingsList.Clear();
                    for (int n = 0; n < TempArray.Length; n++)
                        Abilities.BindingsList.Add(TempArray[n]);
					if (BindingsList[i].AbilityType == 0)//Instant
						AbilityStatus = "Instant";
					else
					{
						AbilityStatus = "Casting";
						WowControl.Stop();
					}
                    StartCast = DateTime.Now;
                    EndCast = DateTime.Now.AddMilliseconds(BindingsList[i].CastTime * 1000);
                    if (CheckString(BindingsList[i].Modifiers, "Alt") == true)
                        WowControl.KeyToWnd(0x12, false);
                    if (CheckString(BindingsList[i].Modifiers, "Control") == true)
                        WowControl.KeyToWnd(0x11, false);
                    if (CheckString(BindingsList[i].Modifiers, "Shift") == true)
                        WowControl.KeyToWnd(0x10, false);
                    WowControl.KeyToWnd(BindingsList[i].Key, false);
                    WowControl.Sleep(Duration);
                    WowControl.KeyToWnd(BindingsList[i].Key, true);
                    if (CheckString(BindingsList[i].Modifiers, "Alt") == true)
                        WowControl.KeyToWnd(0x12, true);
                    if (CheckString(BindingsList[i].Modifiers, "Control") == true)
                        WowControl.KeyToWnd(0x11, true);
                    if (CheckString(BindingsList[i].Modifiers, "Shift") == true)
                        WowControl.KeyToWnd(0x10, true);
                }
        }
        public static bool IsSpellOntarget(Memory.Memory wow, int UnitIndex, int Spell)
        {
            if (wow.Objects[UnitIndex].Auras == null)
                return true;
            for (int i = 0; i < wow.Objects[UnitIndex].Auras.Count; i++)
            {
                if (wow.Objects[UnitIndex].Auras[i].auraId == Spell)
                    return true;
            }
            return false;
        }
		public static bool SubCheckAbilityOptions(int AbilityIndex, Memory.Memory wow, String Type, int Unit)
		{
			bool result = false;
			int PlayerRange = 0;
			if (WowControl.ClassType == 0)//Ranged
				PlayerRange = 30;
			else PlayerRange = 7;
			if (AbilityIndex == 14)
			{
				int y = 0;
				y++;
			}
			double Range = Convert.ToInt32(WowControl.CheckPoint(wow.Objects[0].X, wow.Objects[0].Y, wow.Objects[0].Z, wow.Objects[Unit].X, wow.Objects[Unit].Y, wow.Objects[Unit].Z));
			for (int i = 0; i < BindingsList[AbilityIndex].OptionsList.Count; i++)
			{
				if ((BindingsList[AbilityIndex].OptionsList[i].Unit == "Player") & (Unit == 0))
				{
					if (i == 0)
						result = CheckOptionsValues(Unit, BindingsList[AbilityIndex].OptionsList[i].Parameter, BindingsList[AbilityIndex].OptionsList[i].Condition, BindingsList[AbilityIndex].OptionsList[i].Value, wow);
					else
					{
						if (BindingsList[AbilityIndex].OptionsList[i - 1].AndOr == "AND")
							result = result & CheckOptionsValues(Unit, BindingsList[AbilityIndex].OptionsList[i].Parameter, BindingsList[AbilityIndex].OptionsList[i].Condition, BindingsList[AbilityIndex].OptionsList[i].Value, wow);
						else if (BindingsList[AbilityIndex].OptionsList[i - 1].AndOr == "OR")
							result = result | CheckOptionsValues(Unit, BindingsList[AbilityIndex].OptionsList[i].Parameter, BindingsList[AbilityIndex].OptionsList[i].Condition, BindingsList[AbilityIndex].OptionsList[i].Value, wow);
					}
				} else if ((BindingsList[AbilityIndex].OptionsList[i].Unit == "Enemy") & (wow.Objects[Unit].Faction != wow.Objects[0].Faction) & (Range <= WowControl.EnemyCheckRange))
				{
					if (i == 0)
						result = CheckOptionsValues(Unit, BindingsList[AbilityIndex].OptionsList[i].Parameter, BindingsList[AbilityIndex].OptionsList[i].Condition, BindingsList[AbilityIndex].OptionsList[i].Value, wow);
					else
					{
						if (BindingsList[AbilityIndex].OptionsList[i - 1].AndOr == "AND")
							result = result & CheckOptionsValues(Unit, BindingsList[AbilityIndex].OptionsList[i].Parameter, BindingsList[AbilityIndex].OptionsList[i].Condition, BindingsList[AbilityIndex].OptionsList[i].Value, wow);
						else if (BindingsList[AbilityIndex].OptionsList[i - 1].AndOr == "OR")
							result = result | CheckOptionsValues(Unit, BindingsList[AbilityIndex].OptionsList[i].Parameter, BindingsList[AbilityIndex].OptionsList[i].Condition, BindingsList[AbilityIndex].OptionsList[i].Value, wow);
					}
				} else if ((BindingsList[AbilityIndex].OptionsList[i].Unit == "Friend") & (wow.Objects[Unit].Faction == wow.Objects[0].Faction) & (Range <= WowControl.FriendCheckRange))
				{
					if (i == 0)
						result = CheckOptionsValues(Unit, BindingsList[AbilityIndex].OptionsList[i].Parameter, BindingsList[AbilityIndex].OptionsList[i].Condition, BindingsList[AbilityIndex].OptionsList[i].Value, wow);
					else
					{
						if (BindingsList[AbilityIndex].OptionsList[i - 1].AndOr == "AND")
							result = result & CheckOptionsValues(Unit, BindingsList[AbilityIndex].OptionsList[i].Parameter, BindingsList[AbilityIndex].OptionsList[i].Condition, BindingsList[AbilityIndex].OptionsList[i].Value, wow);
						else if (BindingsList[AbilityIndex].OptionsList[i - 1].AndOr == "OR")
							result = result | CheckOptionsValues(Unit, BindingsList[AbilityIndex].OptionsList[i].Parameter, BindingsList[AbilityIndex].OptionsList[i].Condition, BindingsList[AbilityIndex].OptionsList[i].Value, wow);
					}
				}
			}
			if (result == true)
			{//Выставляем приоритет
				int Priority = 0;
				if (wow.Objects[Unit].MaxHealth != 0)
				{
					int Health = (int)Convert.ToInt64(Convert.ToDouble(wow.Objects[Unit].Health) / Convert.ToDouble(wow.Objects[Unit].MaxHealth) * 100);
					if (wow.Objects[Unit].Faction == wow.Objects[0].Faction)
					{//Friend or player
						if (Unit == 0)//Player
							Priority = Health * 2;
						else Priority = Convert.ToInt32(Health + (PlayerRange - Range) * 100 / 30);
					} else
					{//Enemy
						Priority = Convert.ToInt32(Health + (PlayerRange - Range) * 100 / 30);
					}
					if (Prioritets[AbilityIndex, Unit].Prioritet < Priority)
						Prioritets[AbilityIndex, Unit].Prioritet = Priority;
					if (Priority >= Prioritets[AbilityIndex, Unit].Prioritet)
						Prioritets[AbilityIndex, Unit].GUID = wow.Objects[Unit].GUID;
					if ((Math.Abs(wow.Objects[0].Level - wow.Objects[Unit].Level) > 5) & (Type == "Bg"))
						Prioritets[AbilityIndex, Unit].Prioritet = 0;
				}
				/*
				for (int i = 0; i < BindingsList[AbilityIndex].OptionsList.Count; i++)
				{//Прогоняем все опции для абилы
					if (wow.Objects[Unit].MaxHealth != 0)
					{
						if (BindingsList[AbilityIndex].OptionsList[i].Unit == "Friend")
						{
							if (BindingsList[AbilityIndex].OptionsList[i].Parameter != "SpellID")
								Priority = (100 - (int)Convert.ToInt64(Convert.ToDouble(wow.Objects[Unit].Health) / Convert.ToDouble(wow.Objects[Unit].MaxHealth) * 100)) * 3;
							else Priority = 90;
							if (Unit == 0)
								Priority *= 2;
						}
						if (BindingsList[AbilityIndex].OptionsList[i].Unit == "Player")
							if (BindingsList[AbilityIndex].OptionsList[i].Parameter != "SpellID")
								Priority = (100 - (int)Convert.ToInt64(Convert.ToDouble(wow.Objects[Unit].Health) / Convert.ToDouble(wow.Objects[Unit].MaxHealth) * 100)) * 4;
							else Priority = 90;
						if (BindingsList[AbilityIndex].OptionsList[i].Unit == "Enemy")
						{
							double param =(PlayerRange - Range) / 10;
							if (param > 0)
								Priority = (int)Convert.ToInt64(Convert.ToDouble(wow.Objects[Unit].Health) / Convert.ToDouble(wow.Objects[Unit].MaxHealth) * 100 * param);
						}
					}
					if (Prioritets[AbilityIndex, Unit].Prioritet < Priority)
						Prioritets[AbilityIndex, Unit].Prioritet = Priority;

					/*
					if (BindingsList[AbilityIndex].OptionsList[i].Parameter == "SpellID")
					{
						if (BindingsList[AbilityIndex].OptionsList[i].Unit == "Friend")
						{
							Priority = (100 - (int)Convert.ToInt64(Convert.ToDouble(wow.Objects[Unit].Health) / Convert.ToDouble(wow.Objects[Unit].MaxHealth) * 100)) * 3;
							if (Unit == 0)
								Priority *= 2;
						}
						if (BindingsList[AbilityIndex].OptionsList[i].Unit == "Player")
							Priority = (100 - (int)Convert.ToInt64(Convert.ToDouble(wow.Objects[Unit].Health) / Convert.ToDouble(wow.Objects[Unit].MaxHealth) * 100)) * 4;
						if (BindingsList[AbilityIndex].OptionsList[i].Unit == "Enemy")
						{
							int param = Convert.ToInt32((PlayerRange - Range) / 10);
							Priority = (int)Convert.ToInt64(Convert.ToDouble(wow.Objects[Unit].Health) / Convert.ToDouble(wow.Objects[Unit].MaxHealth) * 100) * param;
						}
					} else if (BindingsList[AbilityIndex].OptionsList[i].Parameter == "Health")
					{
						if (wow.Objects[Unit].MaxHealth != 0)
						{
							if (BindingsList[AbilityIndex].OptionsList[i].Unit == "Friend")
							{
								Priority = (100 - (int)Convert.ToInt64(Convert.ToDouble(wow.Objects[Unit].Health) / Convert.ToDouble(wow.Objects[Unit].MaxHealth) * 100)) * 3;
								if (Unit == 0)
									Priority *= 2;
							}
							if (BindingsList[AbilityIndex].OptionsList[i].Unit == "Player")
								Priority = (100 - (int)Convert.ToInt64(Convert.ToDouble(wow.Objects[Unit].Health) / Convert.ToDouble(wow.Objects[Unit].MaxHealth) * 100)) * 4;
							if (BindingsList[AbilityIndex].OptionsList[i].Unit == "Enemy")
							{
								int param = Convert.ToInt32((PlayerRange - Range) / 10);
								Priority = (int)Convert.ToInt64(Convert.ToDouble(wow.Objects[Unit].Health) / Convert.ToDouble(wow.Objects[Unit].MaxHealth) * 100) * param;
							}
						}
						if (Prioritets[AbilityIndex, Unit].Prioritet < Priority)
							Prioritets[AbilityIndex, Unit].Prioritet = Priority;
					} else if (BindingsList[AbilityIndex].OptionsList[i].Parameter == "Range")
					{
						if (Prioritets[AbilityIndex, Unit].Prioritet == 0)
							if (WowControl.ClassType == 0)//Ranged
								Priority = 100 - Convert.ToInt32(WowControl.CheckPoint(wow.Objects[0].X, wow.Objects[0].Y, wow.Objects[0].Z, wow.Objects[Unit].X, wow.Objects[Unit].Y, wow.Objects[Unit].Z));
							else Priority = 100 - Convert.ToInt32(WowControl.CheckPoint(wow.Objects[0].X, wow.Objects[0].Y, wow.Objects[0].Z, wow.Objects[Unit].X, wow.Objects[Unit].Y, wow.Objects[Unit].Z)) * 2;
						if (Prioritets[AbilityIndex, Unit].Prioritet < Priority)
							Prioritets[AbilityIndex, Unit].Prioritet = Priority;
					} else if (BindingsList[AbilityIndex].OptionsList[i].Parameter == "Power")
					{
						if (Prioritets[AbilityIndex, Unit].Prioritet == 0)
							Priority = 70;
					}
					 
				}*/
			}
			return result;
		}
		public static UInt64 CheckAbilityOptions(int AbilityIndex, Memory.Memory wow, String Type)
		{
			for (int Unit = 0; Unit < wow.Objects.Count; Unit++)
			{
				if (Type != "Bg")
				{
					if ((WowControl.CheckForBad(wow.Objects[Unit].GUID) == false) & (wow.Objects[Unit].Health > 1))
						switch (BindingsList[AbilityIndex].InCombat)
						{
							case 0://Always
								if ((SubCheckAbilityOptions(AbilityIndex, wow, Type, Unit) == true) & (Prioritets[AbilityIndex, Unit].Prioritet != 0))
									return wow.Objects[Unit].GUID;
								break;
							case 1://In combat
								if (wow.Objects[0].IsInCombat != 1)
									break;
								if ((SubCheckAbilityOptions(AbilityIndex, wow, Type, Unit) == true) & (Prioritets[AbilityIndex, Unit].Prioritet != 0))
									return wow.Objects[Unit].GUID;
								break;
							case 2://Out of combat
								if (wow.Objects[0].IsInCombat != 0)
									break;
								if ((SubCheckAbilityOptions(AbilityIndex, wow, Type, Unit) == true) & (Prioritets[AbilityIndex, Unit].Prioritet != 0))
									return wow.Objects[Unit].GUID;
								break;
						}
				} else
				{
					if ((WowControl.CheckForBad(wow.Objects[Unit].GUID) == false) & (wow.Objects[Unit].Health > 1) & (wow.Objects[Unit].Type == 4))
						switch (BindingsList[AbilityIndex].InCombat)
						{
							case 0://Always
								if ((SubCheckAbilityOptions(AbilityIndex, wow, Type, Unit) == true) & (Prioritets[AbilityIndex, Unit].Prioritet != 0))
									return wow.Objects[Unit].GUID;
								break;
							case 1://In combat
								if (wow.Objects[0].IsInCombat != 1)
									break;
								if ((SubCheckAbilityOptions(AbilityIndex, wow, Type, Unit) == true) & (Prioritets[AbilityIndex, Unit].Prioritet != 0))
									return wow.Objects[Unit].GUID;
								break;
							case 2://Out of combat
								if (wow.Objects[0].IsInCombat != 0)
									break;
								if ((SubCheckAbilityOptions(AbilityIndex, wow, Type, Unit) == true) & (Prioritets[AbilityIndex, Unit].Prioritet != 0))
									return wow.Objects[Unit].GUID;
								break;
						}
				}
			}
			return 0;
		}       

		public static bool CheckOptionsValues(int Unit, String Parameter, String Condition, String Value, Memory.Memory wow)
		{
			bool result = false;
			int UnitParam = 0;
			int ValueParam = Convert.ToInt32(Value);
			if (Parameter == "Health")
				if (wow.Objects[Unit].MaxHealth != 0)
					UnitParam = (int)Convert.ToInt64(Convert.ToDouble(wow.Objects[Unit].Health) / Convert.ToDouble(wow.Objects[Unit].MaxHealth) * 100);
			if (Parameter == "SpellID")
			{
				if (IsSpellOntarget(wow, Unit, Convert.ToInt32(Value)) == true)
				{
					if (Condition == "=")
						return true;
					else if (Condition == "!=")
						return false;
				} else
				{
					if (Condition == "=")
						return false;
					else if (Condition == "!=")
						return true;
				}
			}
			if (Parameter == "Range")
				UnitParam = Convert.ToInt32(WowControl.CheckPoint(wow.Objects[0].X, wow.Objects[0].Y, wow.Objects[0].Z, wow.Objects[Unit].X, wow.Objects[Unit].Y, wow.Objects[Unit].Z));
			if (Parameter == "Power")
				UnitParam = wow.Objects[Unit].Power;
			if (Condition == ">")
				result = (UnitParam > ValueParam);
			else if (Condition == "<")
				result = (UnitParam < ValueParam);
			else if (Condition == "=")
				result = (UnitParam == ValueParam);
			else if (Condition == "!=")
				result = (UnitParam != ValueParam);
			return result;

		}
		public static void UseAbilities(Memory.Memory wow, String Type)
		{
			Ability[] Bindings = BindingsList.ToArray();
			Prioritets = new AbilityPrioritet[BindingsList.Count, wow.Objects.Count];
			if (EndCast <= DateTime.Now)
				AbilityStatus = "Ready";
			if (AbilityStatus == "Ready")
			{
				for (int i = 0; i < Bindings.Length; i++)
				{//Прогоняем все абилы
					if (Bindings[i].Try < 5)
					{
						if ((DateTime.Now - Bindings[i].LastUse).TotalSeconds >= Bindings[i].CoolDown)
						{
							if ((Bindings[i].InCombat >= 0) & (Bindings[i].InCombat <= 2))
								CheckAbilityOptions(i, wow, Type);
						}
					}
				}

				int MaxPriority = 0;
				int BestAbilityIndex = -1;
				int BestUnit = -1;
				for (int Unit = 0; Unit < wow.Objects.Count; Unit++)
					for (int i = 0; i < BindingsList.Count; i++)
					{
						if (MaxPriority < Prioritets[i, Unit].Prioritet)
						{
							BestUnit = Unit;
							MaxPriority = Prioritets[i, Unit].Prioritet;
							BestAbilityIndex = i;
						}
					}
				if (BestAbilityIndex != -1)
				{
					Target = WowControl.GetUnitIndexByGuid(wow, Prioritets[BestAbilityIndex, BestUnit].GUID);
					WowControl.SelectTarget(Prioritets[BestAbilityIndex, BestUnit].GUID);
					if (Target != 0)
						Moving(wow, Target, BestAbilityIndex);
					if (Target == 0)
						Button(BindingsList[BestAbilityIndex].AbilityName);
					int Health = 0;
					if (wow.Objects[Target].MaxHealth != 0)
						Health = (int)Convert.ToInt64(Convert.ToDouble(wow.Objects[Target].Health) / Convert.ToDouble(wow.Objects[Target].MaxHealth) * 100);
					WowControl.UpdateCombatLog(WowControl.GetTimeStr() + " Using \"" + Bindings[BestAbilityIndex].AbilityName + "\" on \"" + wow.Objects[Target].Name + "\" Health = " + Health + ".");
					Bindings[BestAbilityIndex].LastUse = BindingsList[BestAbilityIndex].LastUse;
				} else Target = -1;
				BindingsList.Clear();
				for (int i = 0; i < Bindings.Length; i++)
					BindingsList.Add(Bindings[i]);
			}
		}
		public static void Moving(Memory.Memory wow, int Target, int BestAbilityIndex)
		{
			if (Target > 0)
			{
				if (WowControl.ClassType == 1)
				{
					if (Abilities.AbilityStatus != "Casting")
					{
						Button(BindingsList[BestAbilityIndex].AbilityName);
						wow.GoTo(0, 0, 0, wow.Objects[Abilities.Target].GUID, 11);
					}

				} else
				{
					double Range = WowControl.CheckPoint(wow.Objects[0].X, wow.Objects[0].Y, wow.Objects[0].Z, wow.Objects[Abilities.Target].X, wow.Objects[Abilities.Target].Y, wow.Objects[Abilities.Target].Z);
					if (Range > 25)
					{
						if (Abilities.AbilityStatus != "Casting")
							wow.GoTo(wow.Objects[Abilities.Target].X, wow.Objects[Abilities.Target].Y, wow.Objects[Abilities.Target].Z, wow.Objects[Abilities.Target].GUID, 4);
					} else
					{
						Button(BindingsList[BestAbilityIndex].AbilityName);
						wow.GoTo(0, 0, 0, 0, 3);
						WowControl.Sleep(100);
						wow.GoTo(0, 0, 0, wow.Objects[Abilities.Target].GUID, 1);
					}
				}
				if (WowControl.ErrorState == "Bad target")
				{
					//WowControl.BadObjects.Add(wow.Objects[Abilities.Target].GUID);
					WowControl.UpdateCombatLog("Bad target " + wow.Objects[Abilities.Target].Name + ".");
				}
			}

		}

    }
}
