using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace FarmHelper_beta
{
    class Farm
    {
        public Memory.Memory wow;
        public int CurrentWP, TryToMount, TryToLoot, TryLoFly, UnstuckTimes, TryToSkin, TryToReach, TryToFight;
        private bool WPDetected, StuckStarted, Looting, Skining;
        private ulong Target;
        private double LastRange;
        private WowControl.LootObject LootingObject, SkinningObject;
        private DateTime LastTry, StuckTime, LastCheck, LastTime;
        private MainForm.WayPoints Corpse, LastLocation;
        public Farm()
        {
            wow = new Memory.Memory();
        }
        public void Process()
        {
            if (wow.BgStatus == 3)
            {
                WowControl.FarmStatus = "In bg";
                return;
            }
            if (wow.Objects[0].Health == 0)
            {//токашо похерились
                Abilities.AbilityStatus = "Ready";
                Corpse.X = wow.Objects[0].X;
                Corpse.Y = wow.Objects[0].Y;
                Corpse.Z = wow.Objects[0].Z;
                if ((DateTime.Now - LastCheck).TotalMilliseconds > 20000)
                {
                    WowControl.UpdateStatus("Player dead, releasing spirit.");
                    WowControl.Command("script  RepopMe()");
                    LastCheck = DateTime.Now;
                    WowControl.WaitTime = DateTime.Now.AddSeconds(10);
                }
                return;
            }
            if (wow.Objects[0].Health == 1)
            {
                Abilities.AbilityStatus = "Ready";
                WowControl.UpdateStatus("Going to corpse.");
                wow.GoTo(Corpse.X, Corpse.Y, Corpse.Z, 0, 4);
                if ((DateTime.Now - LastTime).TotalMilliseconds > 1000)
                {
                    LastTime = DateTime.Now;
                    Abilities.Button("Jump");
                }
                if (WowControl.CheckPoint(wow.Objects[0].X, wow.Objects[0].Y, wow.Objects[0].Z, Corpse.X, Corpse.Y, Corpse.Z) < 15)
                {
                    if ((DateTime.Now - LastCheck).TotalMilliseconds > 20000)
                    {
                        WowControl.Command("script  RetrieveCorpse()");
                        LastCheck = DateTime.Now;
                    }
                    WowControl.Sleep(5000);
                }
                return;
            }
			if (wow.IsMounted == 0)
			{
				CheckCombat();
				Abilities.UseAbilities(wow, "");
			}
            if (Abilities.AbilityStatus == "Casting")
                return;
            CheckForLootSkin();
            if (wow.IsMounted != 0)
            {
                TryToMount = 0;
                WowControl.FarmStatus = "Searching";
            }
            if ((WowControl.FarmStatus == "In water") & (wow.Objects[0].IsInCombat == 0))
            {
                WowControl.UpdateStatus("Bot in water, trying to unstuck.");
                Unstuck();
                return;
            }
            if (WowControl.LoadedWP != wow.Location)
                WowControl.WP = WowControl.LoadWaypoints(wow.Location);
            for (int i = 0; i < WowControl.WP.ToArray().Length; i++)
            {
                if (WowControl.CheckPoint(wow.Objects[0].X, wow.Objects[0].Y, wow.Objects[0].Z, WowControl.WP[i].X, WowControl.WP[i].Y, WowControl.WP[i].Z) < 15)
                {
                    CurrentWP = WowControl.WP[i].NextWaypoint;
                    WPDetected = true;
                    break;
                }
            }
            if (WPDetected == false)
            {
                CurrentWP = GetClosestWP(WowControl.WP);
                WPDetected = true;
            }
            if (CurrentWP >= WowControl.WP.Length)
                CurrentWP = 0;
            double RangeToLastLoc = WowControl.CheckPoint(wow.Objects[0].X, wow.Objects[0].Y, wow.Objects[0].Z, LastLocation.X, LastLocation.Y, LastLocation.Z);
            LastLocation.X = wow.Objects[0].X;
            LastLocation.Y = wow.Objects[0].Y;
            LastLocation.Z = wow.Objects[0].Z;
            if (RangeToLastLoc > 50)
            {
                CurrentWP = GetClosestWP(WowControl.WP);
                WowControl.UpdateStatus("Changed position, detecting closest waypoint.");
            }
            int ClosestObject = 0;
            int CLosestLootable = 0;
            int CLosestSkinable = 0;
            double Range = 0;
            if (((WowControl.LootMobs == true) || (WowControl.FarmSkinning == true)) & (FindClosestGoods() == 0))
            {//Killing mobs block
                if (wow.Objects[0].IsInCombat == 0)
                {
                    if (WowControl.SkiningKillAll == true)
                        ClosestObject = FindClosestMob();
                    CLosestLootable = FindClosestLootableMob();
                    if (WowControl.SkinAll == true)
                        CLosestSkinable = FindClosestSkinableMob();
                    if (CLosestLootable != 0)
                    {//Лутаем ближайшего моба
                        Range = WowControl.CheckPoint(wow.Objects[0].X, wow.Objects[0].Y, wow.Objects[0].Z, wow.Objects[CLosestLootable].X, wow.Objects[CLosestLootable].Y, wow.Objects[CLosestLootable].Z);
                        if (((DateTime.Now - LastCheck).Milliseconds > 200) & (wow.IsMounted != 0))
                        {
                            if ((float)Math.Abs(Range - LastRange) < 0.03)
                                TryToReach++;
                            LastRange = Range;
                            LastCheck = DateTime.Now;
                        }
                        if (TryToReach > 20)
                        {
                            WowControl.UpdateStatus("Can't reach object.");
                            WowControl.BadObjects.Add(wow.Objects[CLosestLootable].GUID);
                            TryToReach = 0;
                            return;
                        }
                        if (Range > 15)
                        {
                            if ((wow.IsMounted == 0) & (wow.IsIndoors == 0))
                            {
                                WowControl.GetOnMount(wow, WowControl.FarmMount);
                                return;
                            }
                            if ((wow.IsMounted != 0) & (WowControl.FarmMount == 1) & (DateTime.Now - LastTime).TotalMilliseconds > 3000)
                            {
                                LastTime = DateTime.Now;
                                Abilities.Button("Jump");
                            }
                            wow.GoTo(wow.Objects[CLosestLootable].X, wow.Objects[CLosestLootable].Y, wow.Objects[CLosestLootable].Z, wow.Objects[CLosestLootable].GUID, 4);
                        }
                        else
                        {
                            WowControl.FarmStatus = "Looting";
                            Looting = true;
                            LootingObject.Guid = wow.Objects[CLosestLootable].GUID;
                            LootingObject.Name = wow.Objects[CLosestLootable].Name;
                            LootingObject.Time = WowControl.GetTimeStr();
                            Move(wow.Objects[0].R /*- (float)Math.PI*/, 5);
                            Abilities.AbilityStatus = "Casting";
                            Abilities.EndCast = DateTime.Now.AddSeconds(2);
                            WowControl.Sleep(100);
                            wow.GoTo(0, 0, 0, wow.Objects[CLosestLootable].GUID, 6);//Loot
                        }
                        return;
                    }
                    if (CLosestSkinable != 0)
                    {//Скиним ближайшего моба
                        Range = WowControl.CheckPoint(wow.Objects[0].X, wow.Objects[0].Y, wow.Objects[0].Z, wow.Objects[CLosestLootable].X, wow.Objects[CLosestLootable].Y, wow.Objects[CLosestLootable].Z);
                        if ((DateTime.Now - LastCheck).Milliseconds > 200)
                        {
                            if ((float)Math.Abs(Range - LastRange) < 0.03)
                                TryToReach++;
                            LastRange = Range;
                            LastCheck = DateTime.Now;
                        }
                        if (TryToReach > 20)
                        {
                            WowControl.UpdateStatus("Can't reach object.");
                            WowControl.BadObjects.Add(wow.Objects[CLosestLootable].GUID);
                            TryToReach = 0;
                            return;
                        }
                        if (Range < 15)
                        {
                            WowControl.FarmStatus = "Skining";
                            if (wow.LastError == "С этого существа нельзя снять шкуру.")
                            {
                                WowControl.BadObjects.Add(wow.Objects[CLosestSkinable].GUID);
                                WowControl.UpdateStatus(wow.LastError);
                                return;
                            }
                            if (wow.LastError == "Недостаточный уровень навыка.")
                            {
                                WowControl.BadObjects.Add(wow.Objects[CLosestSkinable].GUID);
                                WowControl.UpdateStatus(wow.LastError);
                                return;
                            }
                            Skining = true;
                            TryToSkin++;
                            if (TryToSkin > 5)
                            {
                                WowControl.BadObjects.Add(wow.Objects[CLosestSkinable].GUID);
                                WowControl.UpdateStatus("Can't skin \"" + wow.Objects[CLosestSkinable] + "\".");
                                return;
                            }
                            SkinningObject.Guid = wow.Objects[CLosestSkinable].GUID;
                            SkinningObject.Name = wow.Objects[CLosestSkinable].Name;
                            SkinningObject.Time = WowControl.GetTimeStr();
                            Move(wow.Objects[0].R /*- (float)Math.PI*/, 5);
                            Abilities.AbilityStatus = "Casting";
                            Abilities.EndCast = DateTime.Now.AddSeconds(4);
                            WowControl.Sleep(100);
                            wow.GoTo(0, 0, 0, wow.Objects[CLosestSkinable].GUID, 9);//Skin
                        }
                        else
                        {
                            if ((wow.IsMounted == 0) & (wow.IsIndoors == 0))
                            {
                                WowControl.GetOnMount(wow, WowControl.FarmMount);
                                return;
                            }
                            if ((wow.IsMounted != 0) & (WowControl.FarmMount == 1) & (DateTime.Now - LastTime).TotalMilliseconds > 3000)
                            {
                                LastTime = DateTime.Now;
                                Abilities.Button("Jump");
                            }
                            wow.GoTo(wow.Objects[CLosestSkinable].X, wow.Objects[CLosestSkinable].Y, wow.Objects[CLosestSkinable].Z, wow.Objects[CLosestSkinable].GUID, 4);
                        }
                        return;
                    }
                    if (ClosestObject != 0)
                    {//Некого лутать и скинить но есть моб которого нужно отмудохать
                        wow.SelectTarget(wow.Objects[ClosestObject].GUID);
                        Range = WowControl.CheckPoint(wow.Objects[0].X, wow.Objects[0].Y, wow.Objects[0].Z, wow.Objects[ClosestObject].X, wow.Objects[ClosestObject].Y, wow.Objects[ClosestObject].Z + 5);
                        if ((DateTime.Now - LastCheck).Milliseconds > 200)
                        {
							if (WowControl.ErrorState == "Bad target")
							{
								WowControl.BadObjects.Add(wow.Objects[ClosestObject].GUID);
								return;
							}
                            if ((float)Math.Abs(Range - LastRange) < 0.03)
                                TryToReach++;
                            LastRange = Range;
                            LastCheck = DateTime.Now;
                        }
                        if (TryToReach > 10)
                        {
                            WowControl.UpdateStatus("Can't reach object.");
                            WowControl.BadObjects.Add(wow.Objects[ClosestObject].GUID);
                            TryToReach = 0;
                            return;
                        }
                        if (Range > 20)
                        {
                            if ((wow.IsMounted == 0) & (wow.IsIndoors == 0))
                            {
                                WowControl.GetOnMount(wow, WowControl.FarmMount);
                                return;
                            }
                            if ((wow.IsMounted != 0) & (WowControl.FarmMount == 1) & (DateTime.Now - LastTime).TotalMilliseconds > 3000)
                            {
                                LastTime = DateTime.Now;
                                Abilities.Button("Jump");
                            }
                            wow.GoTo(wow.Objects[ClosestObject].X, wow.Objects[ClosestObject].Y, wow.Objects[ClosestObject].Z, wow.Objects[ClosestObject].GUID, 4);
                        }
                        if (WowControl.FarmStatus == "Mounting")
                            return;
                        if (WowControl.ClassType == 1)
                            wow.GoTo(0, 0, 0, wow.Objects[ClosestObject].GUID, 11);
                        else
                        {
                            Range = WowControl.CheckPoint(wow.Objects[0].X, wow.Objects[0].Y, wow.Objects[0].Z, wow.Objects[ClosestObject].X, wow.Objects[ClosestObject].Y, wow.Objects[ClosestObject].Z);
                            if (Range > 30)
                                wow.GoTo(0, 0, 0, wow.Objects[ClosestObject].GUID, 11);
                            else
                            {
                                wow.GoTo(0, 0, 0, 0, 3);
                                WowControl.Sleep(100);
                                wow.GoTo(0, 0, 0, wow.Objects[ClosestObject].GUID, 1);
                            }
                        }

                        Range = WowControl.CheckPoint(wow.Objects[0].X, wow.Objects[0].Y, wow.Objects[0].Z, wow.Objects[ClosestObject].X, wow.Objects[ClosestObject].Y, wow.Objects[ClosestObject].Z);
                        int CheckRange = 0;
                        if (WowControl.ClassType == 1)
                            CheckRange = 7;
                        else CheckRange = 30;
                        if ((wow.Objects[0].IsInCombat == 0) & (Range < CheckRange))
                            TryToFight++;
                        if (TryToFight > 10)
                        {
                            TryToFight = 0;
                            WowControl.BadObjects.Add(wow.Objects[ClosestObject].GUID);
                            WowControl.UpdateCombatLog("Can't attack " + wow.Objects[ClosestObject].Name + ".");
                            WowControl.UpdateStatus("Can't attack " + wow.Objects[ClosestObject].Name + ".");
                        }
                        wow.SetPlayerToCombat();
                        Abilities.UseAbilities(wow, "");
                        WowControl.UpdateCombatLog("Fighting with " + wow.Objects[ClosestObject].Name + ".");
                        return;
                    }
                }
            }
            if (WowControl.FarmMining == true)
            {//farm goods block
                int ClosestIndex = FindClosestGoods();
                if (ClosestIndex != 0)
                {//Finded 
                    if (IsPlayerCloseToVein(ClosestIndex) == true)
                    {
                        WowControl.UpdateStatus("Another player close, going away.");
                        WowControl.BadObjects.Add(wow.Objects[ClosestIndex].GUID);
                        return;
                    }
                    WowControl.UpdateStatus("Finded \"" + wow.Objects[ClosestIndex].Name + "\".");
                    Range = WowControl.CheckPoint(wow.Objects[0].X, wow.Objects[0].Y, wow.Objects[0].Z, wow.Objects[ClosestIndex].X, wow.Objects[ClosestIndex].Y, wow.Objects[ClosestIndex].Z + 5);
                    if (((DateTime.Now - LastCheck).Milliseconds > 200) & (wow.IsMounted != 0))
                    {
                        if ((float)Math.Abs(Range - LastRange) < 0.03)
                            TryToLoot++;
                        LastRange = Range;
                        LastCheck = DateTime.Now;
                    }
                    /*if (TryToLoot > 5)
                    {
                        //WowControl.UpdateStatus("Bot stucked while trying to reach object.");
                        Abilities.Button("Jump", 1000);
                    }*/
                    if (TryToLoot > 20)
                    {
                        WowControl.UpdateStatus("Can't reach object.");
                        WowControl.BadObjects.Add(wow.Objects[ClosestIndex].GUID);
                        TryToLoot = 0;
                        return;
                    }
                    if (Range > 8)
                    {//Далеко от объекта
                        if (wow.IsMounted == 0)
                        {
                            WowControl.GetOnMount(wow, WowControl.FarmMount);
                            return;
                        }
                        if ((wow.IsMounted != 0) & (WowControl.FarmMount == 1) & (DateTime.Now - LastTime).TotalMilliseconds > 3000)
                        {
                            LastTime = DateTime.Now;
                            Abilities.Button("Jump");
                        }
                        WowControl.FarmStatus = "Going to goods";
                        if (wow.Objects[0].Z < wow.Objects[ClosestIndex].Z - 5)
                        {
                            wow.GoTo(wow.Objects[ClosestIndex].X, wow.Objects[ClosestIndex].Y, wow.Objects[ClosestIndex].Z + 10, wow.Objects[ClosestIndex].GUID, 4);
                        }
                        else
                        {
                            wow.GoTo(wow.Objects[ClosestIndex].X, wow.Objects[ClosestIndex].Y, wow.Objects[ClosestIndex].Z + 10, wow.Objects[ClosestIndex].GUID, 4);
                        }
                    }
                    else
                    {//Близко
                        if (wow.IsMounted != 0)//Dismount
                        {
                            WowControl.FarmStatus = "Dismounting";
                            wow.GoTo(0, 0, 0, 0, 3);//Stop
                            if (WowControl.FarmMount == 0)
                                Abilities.Button("Fly mount");
                            else Abilities.Button("Ground mount");
                            Abilities.AbilityStatus = "Ready";
                            WowControl.Sleep(1500);
                        }
                        WowControl.Sleep(200);
                        WowControl.FarmStatus = "Looting";
                        Looting = true;
                        LootingObject.Guid = wow.Objects[ClosestIndex].GUID;
                        LootingObject.Name = wow.Objects[ClosestIndex].Name;
                        LootingObject.Time = WowControl.GetTimeStr();
                        if (wow.Objects[ClosestIndex].Type != 3)
                        {
                            //wow.GoTo(wow.Objects[0].X - 10, wow.Objects[0].Y - 10, wow.Objects[0].Z, wow.Objects[0].GUID, 4);
                            Move(wow.Objects[0].R /*- (float)Math.PI*/, 25);
                            WowControl.Sleep(100);
                            //wow.GoTo(0, 0, 0, 0, 3);//Stop
                            wow.GoTo(wow.Objects[ClosestIndex].X, wow.Objects[ClosestIndex].Y, wow.Objects[ClosestIndex].Z, wow.Objects[ClosestIndex].GUID, 7);
                        }
                        Abilities.Button("Mote xtrtr");
                        TryToLoot++;
                        if (WowControl.FindTextInString(wow.LastError, "быть не менее") == true)
                        {
                            WowControl.UpdateStatus(wow.LastError);
                            WowControl.BadObjects.Add(wow.Objects[ClosestIndex].GUID);
                        }
                        return;
                    }
                }
                else
                {
                    Abilities.UseAbilities(wow, "");
                    if ((wow.IsMounted == 0) & (wow.IsIndoors == 0))
                    {
                        WowControl.GetOnMount(wow, WowControl.FarmMount);
                    }
                    if ((wow.IsMounted != 0) & (WowControl.FarmMount == 1) & (DateTime.Now - LastTime).TotalMilliseconds > 3000)
                    {
                        LastTime = DateTime.Now;
                        Abilities.Button("Jump");
                    }
                    else
                    {
                        Range = WowControl.CheckPoint(wow.Objects[0].X, wow.Objects[0].Y, wow.Objects[0].Z, WowControl.WP[CurrentWP].X, WowControl.WP[CurrentWP].Y, WowControl.WP[CurrentWP].Z);
                        if ((DateTime.Now - LastCheck).Milliseconds > 200)
                        {
                            if ((float)Math.Abs(Range - LastRange) < 0.03)
                                TryLoFly++;
                            LastRange = Range;
                            LastCheck = DateTime.Now;
                        }
                        if (TryLoFly > 10)
                        {
                            UnstuckTimes++;
                            WowControl.UpdateStatus("Can't reach waypoint.");
                            TryLoFly = 0;
                            Move(wow.Objects[0].R - (float)(Math.PI * 0.5), 25);
                            WowControl.Sleep(2000);
                            Abilities.Button("Jump", 2000);
                            if (UnstuckTimes > 3)
                            {
                                Unstuck();
                                CurrentWP--;
                                UnstuckTimes = 0;
                                if ((GetClosestWP(WowControl.WP) - CurrentWP) > 3)
                                    CurrentWP = GetClosestWP(WowControl.WP);
                            }
                            return;
                        }
                        WowControl.FarmStatus = "Searching";
                        wow.GoTo(WowControl.WP[CurrentWP].X, WowControl.WP[CurrentWP].Y, WowControl.WP[CurrentWP].Z, 0, 4);
                        WowControl.UpdateStatus("Going to waypoint № " + CurrentWP + ".");
                    }
                }
            }
        }
        private void CheckCombat()
        {
            if ((wow.Objects[0].IsInCombat == 1) & (wow.IsMounted == 0))
            {
                int Index = GetTarget();
                if (Index != 0)
                {
                    double Range = WowControl.CheckPoint(wow.Objects[0].X, wow.Objects[0].Y, wow.Objects[0].Z, wow.Objects[Index].X, wow.Objects[Index].Y, wow.Objects[Index].Z);
                    wow.SelectTarget(wow.Objects[Index].GUID);
                    WowControl.FarmStatus = "Fighting";
                    Target = wow.Objects[Index].GUID;
                    WowControl.UpdateStatus("Fighting with \"" + wow.Objects[Index].Name + "\".");
                    WowControl.UpdateCombatLog("Fighting with \"" + wow.Objects[Index].Name + "\".");
                    if (WowControl.ClassType == 1)
                        wow.GoTo(0, 0, 0, wow.Objects[Index].GUID, 11);
                    else
                    {
                        if (Range > 30)
                            wow.GoTo(0, 0, 0, wow.Objects[Index].GUID, 11);
                        else
                        {
                            wow.GoTo(0, 0, 0, 0, 3);
                            WowControl.Sleep(100);
                            wow.GoTo(0, 0, 0, wow.Objects[Index].GUID, 1);
                        }
                    }
                    Abilities.UseAbilities(wow, "");
                }
                return;
            }
        }
        private int GetTarget()
        {
            int ClosestIndex = 0;
            double MinRange = 10000;
            for (int i = 1; i < wow.Objects.Count; i++)
            {
                if (wow.Objects[i].GUID == Target)
                    if ((wow.Objects[i].Health == 0) & (wow.Objects[i].Name != ""))
                    {
                        WowControl.UpdateStatus("\"" + wow.Objects[i].Name + "\" owned xD.");
                        WowControl.UpdateCombatLog("\"" + wow.Objects[i].Name + "\" owned xD.");
                    }
                if (wow.Objects[i].IsInCombat == 1)
                {
                    double Range = WowControl.CheckPoint(wow.Objects[i].X, wow.Objects[i].Y, wow.Objects[i].Z, wow.Objects[0].X, wow.Objects[0].Y, wow.Objects[0].Z);
                    if (Range < MinRange)
                    {
                        ClosestIndex = i;
                        if (WowControl.CheckPoint(wow.Objects[i].X, wow.Objects[i].Y, wow.Objects[i].Z, wow.Objects[0].X, wow.Objects[0].Y, wow.Objects[0].Z) < MinRange)
                            MinRange = WowControl.CheckPoint(wow.Objects[i].X, wow.Objects[i].Y, wow.Objects[i].Z, wow.Objects[0].X, wow.Objects[0].Y, wow.Objects[0].Z);
                    }
                }
            }
            return ClosestIndex;
        }
        private void Unstuck()
        {
            if ((DateTime.Now - LastTry).TotalSeconds > 10)
            {
                LastTry = DateTime.Now;
                if (StuckStarted == false)
                {
                    StuckTime = DateTime.Now;
                    StuckStarted = true;
                }
                WowControl.GetOnMount(wow, WowControl.FarmMount);
            }
            if ((DateTime.Now - StuckTime).TotalSeconds > 60)
            {
                StuckTime = DateTime.Now;
                Move(wow.Objects[0].R + (float)Math.PI / 2, 30);
            }
            else
                Move(wow.Objects[0].R, 30);

        }
        private int GetClosestWP(MainForm.WayPoints[] WP)
        {
            double MinRange = 10000;
            int ClosestIndex = 0;
            for (int i = 0; i < WP.Length; i++)
            {
                double Range = WowControl.CheckPoint(WP[i].X, WP[i].Y, WP[i].Z, wow.Objects[0].X, wow.Objects[0].Y, wow.Objects[0].Z);
                if (Range < MinRange)
                {
                    ClosestIndex = i;
                    MinRange = WowControl.CheckPoint(WP[i].X, WP[i].Y, WP[i].Z, wow.Objects[0].X, wow.Objects[0].Y, wow.Objects[0].Z);
                }
            }
            return ClosestIndex;
        }
        private void Move(float R, float Range)
        {
            float X = (float)Convert.ToDouble(wow.Objects[0].X + Math.Cos(R) * Range);
            float Y = (float)Convert.ToDouble(wow.Objects[0].Y + Math.Sin(R) * Range);
            float Z = wow.Objects[0].Z;
            wow.GoTo(X, Y, Z, 0, 4);
        }
        private int FindClosestMob()
        {
            int ClosestIndex = 0;
            double MinRange = 10000;
            for (int i = 0; i < wow.Objects.Count; i++)
            {
                bool Bad = false;
                if (WowControl.BadObjects.Count > 0)
                    for (int c = 0; c < WowControl.BadObjects.Count; c++)
                    {
                        if (WowControl.BadObjects[c] == wow.Objects[i].GUID)
                            Bad = true;
                    }
                if ((Bad == false) & (wow.Objects[i].Type == 3) & (wow.Objects[i].Level > 1) & (wow.Objects[i].Health > 1))
                {
                    if ((WowControl.CheckPoint(wow.Objects[i].X, wow.Objects[i].Y, wow.Objects[i].Z, wow.Objects[0].X, wow.Objects[0].Y, wow.Objects[0].Z) < MinRange) & (WowControl.CheckPoint(wow.Objects[i].X, wow.Objects[i].Y, wow.Objects[i].Z, wow.Objects[0].X, wow.Objects[0].Y, wow.Objects[0].Z) < WowControl.SkiningRange))
                    {
                        ClosestIndex = i;
                        if (WowControl.CheckPoint(wow.Objects[i].X, wow.Objects[i].Y, wow.Objects[i].Z, wow.Objects[0].X, wow.Objects[0].Y, wow.Objects[0].Z) < MinRange)
                            MinRange = WowControl.CheckPoint(wow.Objects[i].X, wow.Objects[i].Y, wow.Objects[i].Z, wow.Objects[0].X, wow.Objects[0].Y, wow.Objects[0].Z);
                    }
                }
            }
            return ClosestIndex;
        }
        private int FindClosestLootableMob()
        {
            int ClosestIndex = 0;
            double MinRange = 10000;
            for (int i = 0; i < wow.Objects.Count; i++)
            {
                bool Bad = false;
                if (WowControl.BadObjects.Count > 0)
                    for (int c = 0; c < WowControl.BadObjects.Count; c++)
                    {
                        if (WowControl.BadObjects[c] == wow.Objects[i].GUID)
                            Bad = true;
                    }
                if ((Bad == false) & (wow.Objects[i].Type == 3) & (wow.Objects[i].Level > 1) & ((wow.Objects[i].IsLootable == 1) || (wow.Objects[i].IsLootable==13)) & (wow.Objects[i].Health == 0))
                {
                    if ((WowControl.CheckPoint(wow.Objects[i].X, wow.Objects[i].Y, wow.Objects[i].Z, wow.Objects[0].X, wow.Objects[0].Y, wow.Objects[0].Z) < MinRange) & (WowControl.CheckPoint(wow.Objects[i].X, wow.Objects[i].Y, wow.Objects[i].Z, wow.Objects[0].X, wow.Objects[0].Y, wow.Objects[0].Z) < WowControl.SkiningRange))
                    {
                        ClosestIndex = i;
                        if (WowControl.CheckPoint(wow.Objects[i].X, wow.Objects[i].Y, wow.Objects[i].Z, wow.Objects[0].X, wow.Objects[0].Y, wow.Objects[0].Z) < MinRange)
                            MinRange = WowControl.CheckPoint(wow.Objects[i].X, wow.Objects[i].Y, wow.Objects[i].Z, wow.Objects[0].X, wow.Objects[0].Y, wow.Objects[0].Z);
                    }
                }
            }
            return ClosestIndex;
        }
        private void CheckForLootSkin()
        {
            if ((DateTime.Now - WowControl.BotStartTime).TotalMinutes > 60)
            {
                WowControl.BadObjects.Clear();
                WowControl.BotStartTime = DateTime.Now;
            }
            for (int i = 0; i < wow.Objects.Count; i++)
            {
                bool Looted = true;
                bool Skined = true;
                bool Bad = false;
                if (WowControl.BadObjects.Count > 0)
                    for (int c = 0; c < WowControl.BadObjects.Count; c++)
                    {
                        if (WowControl.BadObjects[c] == wow.Objects[i].GUID)
                            Bad = true;
                    }
                if ((wow.Objects[i].GUID != 0) & (Bad == false) & (wow.Objects[i].GUID == LootingObject.Guid))
                {
                    if (wow.Objects[i].IsLootable == 0)
                        Looted = false;
                    if (wow.Objects[i].Type == 5)
                        Looted = false;
                }
                if ((Bad == false) & (wow.Objects[i].GUID == SkinningObject.Guid))
                    Skined = false;
                if ((CheckForBad(SkinningObject.Guid) == false) & (Skined == true) & (Skining == true))
                {
                    Skining = false;
                    TryToSkin = 0;
                    CurrentWP = GetClosestWP(WowControl.WP);
                    WowControl.Loot.Add("Skined: \"" + SkinningObject.Name + "\".");
                    WowControl.UpdateStatus("Skined: \"" + SkinningObject.Name + "\".");
                }
                if ((CheckForBad(LootingObject.Guid) == false) & (Looted == true) & (Looting == true) & (Skining == false))
                {
                    Looting = false;
                    TryToLoot = 0;
                    CurrentWP = GetClosestWP(WowControl.WP);
                    WowControl.Loot.Add("Looted : \"" + LootingObject.Name + "\".");
                    WowControl.UpdateStatus("Looted : \"" + LootingObject.Name + "\".");
                }
            }
        }
        private bool CheckForBad(ulong Guid)
        {
            for (int i = 0; i < WowControl.BadObjects.Count; i++)
                if (WowControl.BadObjects[i] == Guid)
                    return true;
            return false;
        }
        private int FindClosestSkinableMob()
        {
            int ClosestIndex = 0;
            double MinRange = 10000;
            for (int i = 0; i < wow.Objects.Count; i++)
            {
                bool Bad = false;
                if (WowControl.BadObjects.Count > 0)
                    for (int c = 0; c < WowControl.BadObjects.Count; c++)
                    {
                        if (WowControl.BadObjects[c] == wow.Objects[i].GUID)
                            Bad = true;
                    }
                if ((Bad == false) & (wow.Objects[i].Type == 3) & (wow.Objects[i].Level > 1) & (wow.Objects[i].IsLootable == 0))
                {
                    if ((WowControl.CheckPoint(wow.Objects[i].X, wow.Objects[i].Y, wow.Objects[i].Z, wow.Objects[0].X, wow.Objects[0].Y, wow.Objects[0].Z) < MinRange) & (WowControl.CheckPoint(wow.Objects[i].X, wow.Objects[i].Y, wow.Objects[i].Z, wow.Objects[0].X, wow.Objects[0].Y, wow.Objects[0].Z) < WowControl.SkiningRange))
                    {
                        ClosestIndex = i;
                        if (WowControl.CheckPoint(wow.Objects[i].X, wow.Objects[i].Y, wow.Objects[i].Z, wow.Objects[0].X, wow.Objects[0].Y, wow.Objects[0].Z) < MinRange)
                            MinRange = WowControl.CheckPoint(wow.Objects[i].X, wow.Objects[i].Y, wow.Objects[i].Z, wow.Objects[0].X, wow.Objects[0].Y, wow.Objects[0].Z);
                    }
                }
            }
            return ClosestIndex;
        }
        private int FindClosestGoods()
        {
            int ClosestIndex = 0;
            double MinRange = 10000;
            for (int n = 0; n < WowControl.Goods.Count; n++)
                for (int i = 0; i < wow.Objects.Count; i++)
                {
                    bool Bad = false;
                    if (WowControl.BadObjects.Count > 0)
                        for (int c = 0; c < WowControl.BadObjects.Count; c++)
                        {
                            if (WowControl.BadObjects[c] == wow.Objects[i].GUID)
                                Bad = true;
                        }
                    if ((Bad == false) & (wow.Objects[i].Type != 4) & (wow.Objects[i].Name == WowControl.Goods[n]))
                    {
                        if (WowControl.CheckPoint(wow.Objects[i].X, wow.Objects[i].Y, wow.Objects[i].Z, wow.Objects[0].X, wow.Objects[0].Y, wow.Objects[0].Z) < MinRange)
                        {
                            ClosestIndex = i;
                            if (WowControl.CheckPoint(wow.Objects[i].X, wow.Objects[i].Y, wow.Objects[i].Z, wow.Objects[0].X, wow.Objects[0].Y, wow.Objects[0].Z) < MinRange)
                                MinRange = WowControl.CheckPoint(wow.Objects[i].X, wow.Objects[i].Y, wow.Objects[i].Z, wow.Objects[0].X, wow.Objects[0].Y, wow.Objects[0].Z);
                        }
                    }
                }
            return ClosestIndex;
        }
        private bool IsPlayerCloseToVein(int index)
        {
            double PlayerRange = WowControl.CheckPoint(wow.Objects[0].X, wow.Objects[0].Y, wow.Objects[0].Z, wow.Objects[index].X, wow.Objects[index].Y, wow.Objects[index].Z);
            for (int i = 0; i < wow.Objects.Count; i++)
            {
                if (wow.Objects[i].Type == 4)
                {
                    double EnemyRange = WowControl.CheckPoint(wow.Objects[i].X, wow.Objects[i].Y, wow.Objects[i].Z, wow.Objects[index].X, wow.Objects[index].Y, wow.Objects[index].Z);
                    if (EnemyRange < 10)
                        if (EnemyRange < PlayerRange)
                            return true;
                }
            }
            return false;
        }
    }
}
