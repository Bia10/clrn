using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Memory;

namespace FarmHelper_beta
{
    class FarmMining
    {
        public int CurrentWP, TryToMount, TryToLoot, TryLoFly, UnstuckTimes;
        private bool WPDetected, StuckStarted, Looting;
        public String Status;
        private ulong Target;
        private double LastRange;
        private WowControl.LootObject LootingObject;
        private DateTime LastTry, StuckTime, LastCheck, LastTime;
        private MainForm.WayPoints Corpse, LastLocation;
        public FarmMining()
        {
            wow = new Memory.Memory();
            WPDetected = false;
            LootingObject = new WowControl.LootObject();
        }
        public Memory.Memory wow;
        public void Process()
        {
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
            if (wow.IsMounted == 0)
                CheckCombat();
            Abilities.UseAbilities(wow);
            if (Abilities.AbilityStatus == "Casting")
                return;
            if (wow.IsMounted != 0)
            {
                TryToMount = 0;
                WowControl.FarmStatus = "Searching";
            }
            if ((WowControl.FarmStatus == "In water")&(wow.Objects[0].IsInCombat==0))
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
            int ClosestIndex = FindClosestGoods();
            if (ClosestIndex != 0)
            {//Finded 
                if (IsPlayerCloseToVein(ClosestIndex) == true)
                {
                    WowControl.UpdateStatus("Another player close, going away.");
                    WowControl.BadObjects.Add(wow.Objects[ClosestIndex].GUID);
                    return;
                }
                WowControl.UpdateStatus("Finded \""+wow.Objects[ClosestIndex].Name+"\".");
                double Range = WowControl.CheckPoint(wow.Objects[0].X, wow.Objects[0].Y, wow.Objects[0].Z, wow.Objects[ClosestIndex].X, wow.Objects[ClosestIndex].Y, wow.Objects[ClosestIndex].Z +5);
                if ((DateTime.Now - LastCheck).Milliseconds > 200)
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
                        Abilities.Button("Fly mount");
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
                    if (WowControl.FindTextInString(wow.LastError, "быть не менее")==true)
                    {
                        WowControl.UpdateStatus(wow.LastError);
                        WowControl.BadObjects.Add(wow.Objects[ClosestIndex].GUID);
                    }
                    return;
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
                    double Range = WowControl.CheckPoint(wow.Objects[0].X, wow.Objects[0].Y, wow.Objects[0].Z, WowControl.WP[CurrentWP].X, WowControl.WP[CurrentWP].Y, WowControl.WP[CurrentWP].Z);
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
        private void CheckCombat()
        {
            if ((wow.Objects[0].IsInCombat == 1) & (wow.IsMounted == 0))
            {
                int Index = GetTarget();
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
        private void Loot(int Index)
        {
            if (Abilities.AbilityStatus == "Casting")
                Status = "Looting";
            else if ((Status == "Looting") & (Abilities.AbilityStatus == "Ready"))
                Status = "Ready";
            if (WowControl.CheckPoint(wow.Objects[0].X, wow.Objects[0].Y, wow.Objects[0].Z, wow.Objects[Index].X, wow.Objects[Index].Y, wow.Objects[Index].Z) > 10)
            {
                if (wow.Objects[0].Z < wow.Objects[Index].Z)
                    wow.GoTo(wow.Objects[0].X, wow.Objects[0].Y, wow.Objects[Index].Z + 30, wow.Objects[Index].GUID, 4);
                else
                    wow.GoTo(wow.Objects[Index].X, wow.Objects[Index].Y, wow.Objects[Index].Z, wow.Objects[Index].GUID, 4);
            }
            else
            {
                if (wow.Objects[Index].Type == 3)
                    Abilities.Button("Mote xtrtr");
                else
                    wow.GoTo(wow.Objects[Index].X, wow.Objects[Index].Y, wow.Objects[Index].Z, wow.Objects[Index].GUID, 7);
            }

        }
        private int GetClosestWP(MainForm.WayPoints[] WP)
        {
            double MinRange=10000;
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
        private int FindClosestGoods()
        {
            int ClosestIndex = 0;
            double MinRange = 10000;
            bool Looted = true;
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
                    if ((LootingObject.Guid == wow.Objects[i].GUID) & (Bad == false))
                        Looted = false;
                }
            if ((Looted == true) & (Looting == true))
            {
                Looting = false;
                TryToLoot = 0;
                WowControl.Loot.Add(LootingObject);
                CurrentWP = GetClosestWP(WowControl.WP);
                WowControl.UpdateStatus("Looted: \"" + LootingObject.Name + "\".");
            }
            return ClosestIndex;
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
                    double Range=WowControl.CheckPoint(wow.Objects[i].X, wow.Objects[i].Y, wow.Objects[i].Z, wow.Objects[0].X, wow.Objects[0].Y, wow.Objects[0].Z);
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
        private void Move(float R, float Range)
        {
            float X = (float)Convert.ToDouble(wow.Objects[0].X + Math.Cos(R) * Range);
            float Y = (float)Convert.ToDouble(wow.Objects[0].Y + Math.Sin(R) * Range);
            float Z = wow.Objects[0].Z;
            wow.GoTo(X, Y, Z, 0, 4);
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

    }
}
