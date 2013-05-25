using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Memory;

namespace FarmHelper_beta
{
    class FarmSkining
    {
        public FarmSkining()
        {
            wow = new Memory.Memory();
        }
        private ulong Target; 
        public Memory.Memory wow;
        private MainForm.WayPoints Corpse, LastLocation;
        private WowControl.LootObject LootingObject, SkinningObject;
        private bool Looting, Skining, WPDetected, StuckStarted;
        private double LastRange;
        private DateTime LastCheck,LastTry, StuckTime, LastTime;
        public int TryToMount, CurrentWP, TryToLoot, TryToFight, TryToSkin, TryToReach, TryLoFly, UnstuckTimes;
        public void Process()
        {
            double Range = 0;
            if (wow.BgStatus == 3)
                return;
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
            CheckCombat();
            Abilities.UseAbilities(wow);
            if (Abilities.AbilityStatus == "Casting")
                return;
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
            //if ((WowControl.Stuck(wow.Speed) == true) & (wow.IsMounted != 0))
            //Abilities.Button("Jump", 500);
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

            int ClosestTarget = 0;
            if (WowControl.SkiningKillAll == true)
                ClosestTarget = FindClosestMob();
            else if (wow.Objects[0].IsInCombat == 1)
                ClosestTarget = FindClosestMob();
            if (ClosestTarget != 0)
            {
                if (wow.Objects[ClosestTarget].Health > 1)
                {
                    Range = WowControl.CheckPoint(wow.Objects[0].X, wow.Objects[0].Y, wow.Objects[0].Z, wow.Objects[ClosestTarget].X, wow.Objects[ClosestTarget].Y, wow.Objects[ClosestTarget].Z + 5);
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
                        WowControl.BadObjects.Add(wow.Objects[ClosestTarget].GUID);
                        TryToReach = 0;
                        return;
                    }
                    if (Range > 30)
                    {
                        if ((wow.IsMounted == 0) & (wow.IsIndoors == 0))
                        {
                            WowControl.GetOnMount(wow, WowControl.FarmMount);
                        }
                    }
                    if (WowControl.FarmStatus == "Mounting")
                        return;
                    wow.SelectTarget(wow.Objects[ClosestTarget].GUID);
                    if (WowControl.ClassType == 1)
                        wow.GoTo(0, 0, 0, wow.Objects[ClosestTarget].GUID, 11);
                    else
                    {
                        Range = WowControl.CheckPoint(wow.Objects[0].X, wow.Objects[0].Y, wow.Objects[0].Z, wow.Objects[ClosestTarget].X, wow.Objects[ClosestTarget].Y, wow.Objects[ClosestTarget].Z);
                        if (Range > 30)
                            wow.GoTo(0, 0, 0, wow.Objects[ClosestTarget].GUID, 11);
                        else
                        {
                            wow.GoTo(0, 0, 0, 0, 3);
                            WowControl.Sleep(100);
                            wow.GoTo(0, 0, 0, wow.Objects[ClosestTarget].GUID, 1);
                        }
                    }
                    Range = WowControl.CheckPoint(wow.Objects[0].X, wow.Objects[0].Y, wow.Objects[0].Z, wow.Objects[ClosestTarget].X, wow.Objects[ClosestTarget].Y, wow.Objects[ClosestTarget].Z);
                    int CheckRange = 0;
                    if (WowControl.ClassType == 1)
                        CheckRange = 7;
                    else CheckRange = 30;
                    if ((wow.Objects[0].IsInCombat == 0) & (Range < CheckRange))
                        TryToFight++;
                    if (TryToFight > 50)
                    {
                        TryToFight = 0;
                        WowControl.BadObjects.Add(wow.Objects[ClosestTarget].GUID);
                        WowControl.UpdateCombatLog("Can't attack " + wow.Objects[ClosestTarget].Name + ".");
                        WowControl.UpdateStatus("Can't attack " + wow.Objects[ClosestTarget].Name + ".");
                    }
                    WowControl.UpdateCombatLog("Fighting with " + wow.Objects[ClosestTarget].Name + ".");
                }
                else
                {
                    if (wow.Objects[ClosestTarget].IsLootable == 1)
                    {
                        WowControl.UpdateStatus("Looting \"" + wow.Objects[ClosestTarget].Name + "\".");
                        WowControl.FarmStatus = "Looting";
                        Looting = true;
                        LootingObject.Guid = wow.Objects[ClosestTarget].GUID;
                        LootingObject.Name = wow.Objects[ClosestTarget].Name;
                        LootingObject.Time = WowControl.GetTimeStr();
                        Move(wow.Objects[0].R /*- (float)Math.PI*/, 5);
                        Abilities.AbilityStatus = "Casting";
                        Abilities.EndCast = DateTime.Now.AddSeconds(2);
                        WowControl.Sleep(100);
                        wow.GoTo(0, 0, 0, wow.Objects[ClosestTarget].GUID, 6);//Loot
                    }
                    else
                    {
                        if (WowControl.SkinAll == true)
                        {
                            WowControl.UpdateStatus("Skining \"" + wow.Objects[ClosestTarget].Name + "\".");
                            WowControl.FarmStatus = "Skining";
                            if (wow.LastError == "С этого существа нельзя снять шкуру.")
                            {
                                WowControl.BadObjects.Add(wow.Objects[ClosestTarget].GUID);
                                WowControl.UpdateStatus(wow.LastError);
                                return;
                            }
                            if (wow.LastError == "Недостаточный уровень навыка.")
                            {
                                WowControl.BadObjects.Add(wow.Objects[ClosestTarget].GUID);
                                WowControl.UpdateStatus(wow.LastError);
                                return;
                            }
                            Skining = true;
                            TryToSkin++;
                            if (TryToSkin > 5)
                            {
                                WowControl.BadObjects.Add(wow.Objects[ClosestTarget].GUID);
                                WowControl.UpdateStatus("Can't skin \"" + wow.Objects[ClosestTarget] + "\".");
                                return;
                            }
                            SkinningObject.Guid = wow.Objects[ClosestTarget].GUID;
                            SkinningObject.Name = wow.Objects[ClosestTarget].Name;
                            SkinningObject.Time = WowControl.GetTimeStr();
                            Move(wow.Objects[0].R /*- (float)Math.PI*/, 5);
                            Abilities.AbilityStatus = "Casting";
                            Abilities.EndCast = DateTime.Now.AddSeconds(4);
                            WowControl.Sleep(100);
                            wow.GoTo(0, 0, 0, wow.Objects[ClosestTarget].GUID, 9);//Skin
                        }
                    }
                }
            }
            else
            {
                if ((wow.IsMounted == 0) & (wow.IsIndoors == 0))
                {
                    WowControl.GetOnMount(wow, WowControl.FarmMount);
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
                        Move(wow.Objects[0].R - (float)Math.PI, 25);
                        WowControl.Sleep(2000);
                        Abilities.Button("Jump", 2000);
                        if (UnstuckTimes > 3)
                        {
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
            bool Looted = false;
            bool Skined = true;
                for (int i = 0; i < wow.Objects.Count; i++)
                {
                    bool Bad = false;
                    if (WowControl.BadObjects.Count > 0)
                        for (int c = 0; c < WowControl.BadObjects.Count; c++)
                        {
                            if (WowControl.BadObjects[c] == wow.Objects[i].GUID)
                                Bad = true;
                        }
                    if ((Bad == false) & (wow.Objects[i].Type == 3) & (wow.Objects[i].Level > 1))
                    {
                        if ((WowControl.CheckPoint(wow.Objects[i].X, wow.Objects[i].Y, wow.Objects[i].Z, wow.Objects[0].X, wow.Objects[0].Y, wow.Objects[0].Z) < MinRange) & (WowControl.CheckPoint(wow.Objects[i].X, wow.Objects[i].Y, wow.Objects[i].Z, wow.Objects[0].X, wow.Objects[0].Y, wow.Objects[0].Z) < WowControl.SkiningRange))
                        {
                            ClosestIndex = i;
                            if (WowControl.CheckPoint(wow.Objects[i].X, wow.Objects[i].Y, wow.Objects[i].Z, wow.Objects[0].X, wow.Objects[0].Y, wow.Objects[0].Z) < MinRange)
                                MinRange = WowControl.CheckPoint(wow.Objects[i].X, wow.Objects[i].Y, wow.Objects[i].Z, wow.Objects[0].X, wow.Objects[0].Y, wow.Objects[0].Z);
                        }
                    }
                    if (SkinningObject.Guid == wow.Objects[i].GUID)
                        Skined = false;
                    if ((LootingObject.Guid == wow.Objects[i].GUID) & (wow.Objects[i].IsLootable==0))
                        Looted = true;
                }
                if ((Skined == true) & (Skining == true))
                {
                    Skining = false;
                    TryToSkin = 0;
                    WowControl.Loot.Add(SkinningObject);
                    CurrentWP = GetClosestWP(WowControl.WP);
                    WowControl.UpdateStatus("Skined: \"" + SkinningObject.Name + "\".");
                }
                if ((Looted == true) & (Looting == true) & (Skining == false))
                {
                    Looted = false;
                    TryToLoot = 0;
                    CurrentWP = GetClosestWP(WowControl.WP);
                    WowControl.UpdateStatus("Looted : \"" + LootingObject.Name + "\".");
                }
                return ClosestIndex;
        }
        private int GetClosestWP(MainForm.WayPoints[] WP)
        {
            if (WP == null)
                return 0;
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
                Abilities.AbilityStatus = "Ready";
                WowControl.GetOnMount(wow, WowControl.FarmMount);
                WowControl.WaitTime = DateTime.Now.AddSeconds(3);
                return;
            }
            if ((DateTime.Now - StuckTime).TotalSeconds > 60)
            {
                StuckTime = DateTime.Now;
                Move(wow.Objects[0].R + (float)Math.PI / 2, 30);
            }
            else
                Move(wow.Objects[0].R, 30);

        }
        private void CheckCombat()
        {
            if ((wow.Objects[0].IsInCombat == 1) & (wow.IsMounted == 0))
            {
                int Index = FindClosestMob();
                if (Index != 0)
                {
                    WowControl.FarmStatus = "Fighting";
                    Target = wow.Objects[Index].GUID;
                    WowControl.UpdateStatus("Fighting with \"" + wow.Objects[Index].Name + "\".");
                    WowControl.UpdateCombatLog("Fighting with \"" + wow.Objects[Index].Name + "\".");
                    if (wow.Objects[Index].IsInCombat == 1)
                        switch (WowControl.ClassType)
                        {
                            case 0://Ranged
                                wow.GoTo(0, 0, 0, wow.Objects[Index].GUID, 1);
                                break;
                            case 1://Melee
                                wow.GoTo(0, 0, 0, wow.Objects[Index].GUID, 11);
                                break;
                        }
                    Abilities.UseAbilities(wow);
                }
                return;
            }
        }
       
    }
}
