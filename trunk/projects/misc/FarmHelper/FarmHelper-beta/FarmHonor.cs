using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Memory;
using System.IO;

namespace FarmHelper_beta
{
    public class FarmHonor
    {
        public FarmHonor()
        {
            wow = new Memory.Memory();
            CurrentWP = 0;
        }
        public struct GroupInfo
        {
            public List<int> Indexes;
            public Double Range;
            public int CountPlayers;
            public float X;
            public float Y;
            public float Z;
        }
		public struct CourseInfo
		{
			public int Index;
			public bool Bad;
			public int Lenght;
			public DateTime BadTime;
		}
		public struct WaypointsInfo
		{
			public int Index;
			public double Range;
		}
		public struct BadWaypoint
		{
			public int From;
			public int To;
		}
		private List<CourseInfo> BadCourses = new List<CourseInfo>();
		private List<List<int>> Courses = new List<List<int>>();
        private List<GroupInfo> Groups = new List<GroupInfo>();
		private List<WaypointsInfo> CalculatedCourses = new List<WaypointsInfo>();
        private bool WPDetected;
        public Memory.Memory wow;
        private DateTime LastTime, TeleportTime, RessurectTime, LastCourseDetect;
        public int CurrentWP, StuckTimes, LastGroup;
        private MainForm.WayPoints LastLocation;
        private int GetClosestWP(int CurrentWP, MainForm.Coords[] WP)
        {
            int ClosestIndex = 0;
            double ClosestRange = 10000;
            for (int i = 0; i < WP.Length; i++)
            {
                double p = WowControl.CheckPoint(WP[i].X, WP[i].Y, WP[i].Z, wow.Objects[0].X, wow.Objects[0].Y, wow.Objects[0].Z);
                if (p < ClosestRange)
                {
                    ClosestIndex = i;
                    ClosestRange = WowControl.CheckPoint(WP[CurrentWP].X, WP[CurrentWP].Y, WP[CurrentWP].Z, wow.Objects[0].X, wow.Objects[0].Y, wow.Objects[0].Z);
                }
            }
            return ClosestIndex;
        }
		public void Process()
		{
			if (WowControl.LoadedWP != wow.Location)
				WowControl.WP = WowControl.LoadWaypoints(wow.Location);
			if (wow.BattlefieldInstanceExpiration != 0)
			{
				WowControl.Sleep(2000);
				wow.Read();
				WowControl.UpdateStatus("Leaving BG.");
				wow.GoTo(0, 0, 0, 0, 3);//Stop
				WowControl.Command("script LeaveBattlefield()");
				WowControl.WaitTime = DateTime.Now.AddSeconds(30);
				return;
			}
			switch (wow.BgStatus)
			{
				case 0://Not queued
					if ((DateTime.Now - LastTime).TotalMilliseconds > 20000)
					{
						if ((DateTime.Now - WowControl.LocationChangeTime).TotalSeconds >= 60)
							WowControl.UpdateStatus("Joining BG queue.");
						LastTime = DateTime.Now;
						WowControl.btn('H');
						WowControl.btn('H');
						WowControl.Sleep(1000);
						WowControl.Command("script JoinBattlefield(1)");
						WowControl.WaitTime = DateTime.Now.AddSeconds(5);
					}
					break;
				case 1://Waiting
					if (wow.Objects[0].IsInCombat != 0)
						Abilities.UseAbilities(wow, "");
					//WowControl.UpdateStatus("Waiting BG.");
					break;
				case 2://Confirm
					if ((DateTime.Now - LastTime).TotalMilliseconds > 5000)
					{
						WowControl.UpdateStatus("Joining BG.");
						LastTime = DateTime.Now;
						WowControl.Command("script  AcceptBattlefieldPort(1,1)");
						WowControl.WaitTime = DateTime.Now.AddSeconds(20);
						return;
					}
					break;
				case 3://In BG
					if (wow.Objects[0].Health == 1)
					{
						StuckTimes = 0;
						CurrentWP = GetClosestWP(WowControl.WP);
						WowControl.BadObjects.Clear();
						WowControl.UpdateStatus("Waiting for ressurect.");
						wow.GoTo(0, 0, 0, 0, 3);//Stop and waiting for ressurect
						Abilities.Ability[] Temp = Abilities.BindingsList.ToArray();
						Abilities.BindingsList.Clear();
						for (int i = 0; i < Temp.Length; i++)
						{
							Temp[i].Try = 0;
							Abilities.BindingsList.Add(Temp[i]);
						}
						RessurectTime = DateTime.Now;
						return;
					}
					if (wow.Objects[0].Health == 0)
					{
						WowControl.BadObjects.Clear();
						WowControl.Sleep(2000);
						RessurectTime = DateTime.Now;
						StuckTimes = 0;
						WowControl.UpdateStatus("Releasing spirit.");
						if ((DateTime.Now - LastTime).TotalMilliseconds > 5000)
						{
							LastTime = DateTime.Now;
							WowControl.Command("script  RepopMe()");
							WowControl.WaitTime = DateTime.Now.AddSeconds(5);
						}
						return;
					}
					Abilities.UseAbilities(wow, "Bg");
					if (Abilities.AbilityStatus != "Casting")
						if (Moving() == true)
							return;
					break;
			}
		}
        private void Move(float R, float Range)
        {
            Abilities.AbilityStatus = "Ready";
            float X = (float)Convert.ToDouble(wow.Objects[0].X + Math.Cos(R) * Range);
            float Y = (float)Convert.ToDouble(wow.Objects[0].Y + Math.Sin(R) * Range);
            float Z = wow.Objects[0].Z;
            wow.GoTo(X, Y, Z, 0, 4);
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
		private int GetClosestWP(float X, float Y, float Z)
		{
			MainForm.WayPoints WP = new MainForm.WayPoints();
			WP.X = X; WP.Y = Y; WP.Z = Z;
			double MinRange = 10000;
			int ClosestIndex = 0;
			for (int i = 0; i < WowControl.WP.Length; i++)
			{
				double Range = WowControl.CheckPoint(WowControl.WP[i].X, WowControl.WP[i].Y, WowControl.WP[i].Z, WP.X, WP.Y, WP.Z);
				if (Range < MinRange)
				{
					ClosestIndex = i;
					MinRange = WowControl.CheckPoint(WowControl.WP[i].X, WowControl.WP[i].Y, WowControl.WP[i].Z, WP.X, WP.Y, WP.Z);
				}
			}
			return ClosestIndex;
		}
        private int GetClosestTeleport()
        {
            double MinRange = 10000;
			
            int ClosestIndex = 0;
            for (int i = 0; i < wow.Objects.Count; i++)
            {
                if ((wow.Objects[i].Type == 5) & ((wow.Objects[i].Name == "Телепорт") || (wow.Objects[i].Name == "Портал защитника")))
                {
                    double Range = WowControl.CheckPoint(wow.Objects[i].X, wow.Objects[i].Y, wow.Objects[i].Z, wow.Objects[0].X, wow.Objects[0].Y, wow.Objects[0].Z);
                    if (Range < MinRange)
                    {
                        ClosestIndex = i;
                        MinRange = WowControl.CheckPoint(wow.Objects[i].X, wow.Objects[i].Y, wow.Objects[i].Z, wow.Objects[0].X, wow.Objects[0].Y, wow.Objects[0].Z);
                    }
                }
            }
            return ClosestIndex;
        }
		private bool Moving()
		{
			//if (FollowTarget() == true)
				//return true;
			if (Abilities.Target == -1)
			{
				if (FollowGroup() == true)
					return true;
				if (Stuck() == true)
					return true;
				//if (FollowCourse() == true)
					//return true;
				FollowWaypoints();
			}
			return false;
		}
		private bool FollowCourse()
		{
			int FinishWP = 0;
			int OptimalCourse = 0;
			if ((DateTime.Now - LastCourseDetect).TotalSeconds > 10)
			{
				LastCourseDetect = DateTime.Now;
				int RangeFromPlayerToGroup = WowControl.RangeFromPlayerToGroup;
				WowControl.RangeFromPlayerToGroup = 2500;
				int ClosestGroup = FindClosestGroup();
				WowControl.RangeFromPlayerToGroup = RangeFromPlayerToGroup;
				if (ClosestGroup != 1000)
				{
					LastGroup = ClosestGroup;
					//CurrentWP = 0;
					FinishWP = GetClosestWP(Groups[ClosestGroup].X, Groups[ClosestGroup].Y, Groups[ClosestGroup].Z);
				} else return false;
				//if (LastGroup != ClosestGroup)
				{
					FindCourse(FinishWP);
					CurrentWP = 0;
				}
			}
			if (CalculatedCourses.Count == 0)
			{
				FindCourse(FinishWP);
				CurrentWP = 0;
			}
			if (CalculatedCourses.Count != 0)
			{
				/*for (int i = 0; i < Courses[CalculatedCourses[OptimalCourse].Index].Count; i++)
				{
					if (WowControl.CheckPoint(wow.Objects[0].X, wow.Objects[0].Y, wow.Objects[0].Z, WowControl.WP[Courses[CalculatedCourses[0].Index][i]].X, WowControl.WP[Courses[CalculatedCourses[0].Index][i]].Y, WowControl.WP[Courses[CalculatedCourses[0].Index][i]].Z) < 5)
					{
						CurrentWP = i + 1;
						break;
					}
				}*/
				if (CurrentWP >= Courses[CalculatedCourses[0].Index].Count)
					return false;
				if (WowControl.CheckPoint(wow.Objects[0].X, wow.Objects[0].Y, wow.Objects[0].Z, WowControl.WP[Courses[CalculatedCourses[0].Index][CurrentWP]].X, WowControl.WP[Courses[CalculatedCourses[0].Index][CurrentWP]].Y, WowControl.WP[Courses[CalculatedCourses[0].Index][CurrentWP]].Z) < 5)
					CurrentWP++;
				int CourseWP = Courses[CalculatedCourses[OptimalCourse].Index][CurrentWP];
				wow.GoTo(WowControl.WP[CourseWP].X, WowControl.WP[CourseWP].Y, WowControl.WP[CourseWP].Z, 0, 4);
				WowControl.UpdateStatus("Going to course waypoint № " + CurrentWP + ".");
				if ((DateTime.Now - LastTime).TotalMilliseconds > 3000)
				{
					LastTime = DateTime.Now;
					if (wow.IsMounted == 0)
						Abilities.Button("Jump");
				}
				return true;
			}
			return false;
		}
		private void FollowWaypoints()
		{
			if (Abilities.AbilityStatus != "Casting")
			{
				if ((Abilities.AbilityStatus == "Ready") & (wow.IsIndoors == 0) & (wow.Objects[0].IsInCombat == 0) & (wow.IsMounted == 0))
				{
					WowControl.GetOnMount(wow, 1);
					WowControl.Sleep(2000);
					LastTime = DateTime.Now;
				}

					for (int i = 0; i < WowControl.WP.ToArray().Length; i++)
					{
						if (WowControl.CheckPoint(wow.Objects[0].X, wow.Objects[0].Y, wow.Objects[0].Z, WowControl.WP[i].X, WowControl.WP[i].Y, WowControl.WP[i].Z) < 5)
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
					wow.GoTo(WowControl.WP[CurrentWP].X, WowControl.WP[CurrentWP].Y, WowControl.WP[CurrentWP].Z, 0, 4);
					if ((DateTime.Now - LastTime).TotalMilliseconds > 3000)
					{
						LastTime = DateTime.Now;
						if (wow.IsMounted == 0)
							Abilities.Button("Jump");
					}
				
				WowControl.UpdateStatus("Going to waypoint № " + CurrentWP + ".");
			}
		}
		private bool FollowTarget()
		{
			if (Abilities.Target > 0)
			{
				if (WowControl.ClassType == 1)
				{
					if (Abilities.AbilityStatus != "Casting")
						wow.GoTo(0, 0, 0, wow.Objects[Abilities.Target].GUID, 11);
				} else
				{
					double Range = WowControl.CheckPoint(wow.Objects[0].X, wow.Objects[0].Y, wow.Objects[0].Z, wow.Objects[Abilities.Target].X, wow.Objects[Abilities.Target].Y, wow.Objects[Abilities.Target].Z);
					if (Range > 25)
					{
						if (Abilities.AbilityStatus != "Casting")
							wow.GoTo(wow.Objects[Abilities.Target].X, wow.Objects[Abilities.Target].Y, wow.Objects[Abilities.Target].Z, wow.Objects[Abilities.Target].GUID, 4);
					} else
					{
						wow.GoTo(0, 0, 0, 0, 3);
						WowControl.Sleep(100);
						wow.GoTo(0, 0, 0, wow.Objects[Abilities.Target].GUID, 1);
					}
				}
				//wow.SetPlayerToCombat();
				//Abilities.UseAbilities(wow, "Bg");
				//WowControl.UpdateCombatLog("Fighting with " + wow.Objects[Abilities.Target].Name + ".");
				/*if (WowControl.ErrorState == "Bad target")
				{
					WowControl.BadObjects.Add(wow.Objects[Abilities.Target].GUID);
					WowControl.UpdateCombatLog("Bad target " + wow.Objects[Abilities.Target].Name + ".");
				}*/
				if (WowControl.StayWithGroup == false)
					return true;
			}
			return false;
		}
		private bool FollowGroup()
		{
			if (WowControl.StayWithGroup == false)
				return true;
			int ClosestGroup = FindClosestGroup();
			if (ClosestGroup != 1000)
			{
				CurrentWP = GetClosestWP(WowControl.WP);
				if ((Abilities.AbilityStatus == "Ready") & (wow.IsIndoors == 0) & (wow.Objects[0].IsInCombat == 0) & (wow.IsMounted == 0))
				{
					WowControl.GetOnMount(wow, 1);
					WowControl.Sleep(2000);
					LastTime = DateTime.Now;
				}
				if (WowControl.InDoors == 1)
				{
					WowControl.UpdateStatus("Can't mount, going with group without mount.");
					Abilities.AbilityStatus = "Ready";
					WowControl.Sleep(150);
					wow.GoTo(Groups[ClosestGroup].X, Groups[ClosestGroup].Y, Groups[ClosestGroup].Z, 0, 4);
					WowControl.WaitTime = DateTime.Now.AddSeconds(2);
					//WowControl.Sleep(2000);
					WowControl.InDoors = 0;
					return true;
				}
				double RangeToGroup = WowControl.CheckPoint(wow.Objects[0].X, wow.Objects[0].Y, wow.Objects[0].Z, Groups[ClosestGroup].X, Groups[ClosestGroup].Y, Groups[ClosestGroup].Z);
				if (RangeToGroup > 4)
					wow.GoTo(Groups[ClosestGroup].X, Groups[ClosestGroup].Y, Groups[ClosestGroup].Z, 0, 4);
				if ((DateTime.Now - LastTime).TotalMilliseconds > 3000)
				{
					LastTime = DateTime.Now;
					if (wow.IsMounted == 0)
						Abilities.Button("Jump");
				}
				WowControl.UpdateStatus("Going with group.");
				return true;
			}
			return false;
		}
		private bool Stuck()
		{
			if (wow.Objects[0].IsInCombat != 0)
				return false;
			if ((Abilities.AbilityStatus != "Casting") & (WowControl.InWater == 0) & (wow.Speed < 0.005) & (wow.Objects[0].Health > 1) & ((DateTime.Now - RessurectTime).TotalSeconds > 20))
				StuckTimes++;
			else StuckTimes = 0;
			if (StuckTimes > 10)
			{
				if ((DateTime.Now - TeleportTime).TotalSeconds > 20)
				{
					TeleportTime = DateTime.Now;
					int Teleport = GetClosestTeleport();
					if (Teleport != 0)
					{
						WowControl.UpdateStatus("Can't reach waypoint, searching for teleport.");
						wow.GoTo(wow.Objects[Teleport].X, wow.Objects[Teleport].Y, wow.Objects[Teleport].Z, wow.Objects[Teleport].GUID, 7);
						WowControl.WaitTime = DateTime.Now.AddSeconds(10);
						return true;
					}
				}
				WowControl.UpdateStatus("Can't reach waypoint, going to previous waypoint.");
				CurrentWP--;
				if ((GetClosestWP(WowControl.WP) - CurrentWP) > 3)
					CurrentWP = GetClosestWP(WowControl.WP);
				if (CurrentWP < 0)
					CurrentWP = GetClosestWP(WowControl.WP);
				StuckTimes = 0;
			}
			if (CurrentWP < 0)
				CurrentWP = WowControl.WP.Length;
			if ((Abilities.AbilityStatus == "Ready") & (wow.IsIndoors == 0) & (wow.Objects[0].IsInCombat == 0) & (wow.IsMounted == 0))
			{
				WowControl.GetOnMount(wow, 1);
				WowControl.Sleep(2000);
				LastTime = DateTime.Now;
			}
			if (WowControl.InDoors == 1)
			{
				WowControl.UpdateStatus("Can't mount, going to next waypoint without mount.");
				Abilities.AbilityStatus = "Ready";
				WowControl.Sleep(150);
				wow.GoTo(WowControl.WP[CurrentWP].X, WowControl.WP[CurrentWP].Y, WowControl.WP[CurrentWP].Z, 0, 4);
				WowControl.WaitTime = DateTime.Now.AddSeconds(2);
				//WowControl.Sleep(2000);
				WowControl.InDoors = 0;
				return true;
			}
			return false;
		}
        private int FindClosestGroup()
        {
            Groups.Clear();
            int GroupRange = WowControl.GroupRange;
            int RangeFromPlayerToGroup = WowControl.RangeFromPlayerToGroup;
            int MinPlayersInGroup = WowControl.MinPlayersInGroup;
            for (int i = 1; i < wow.Objects.Count; i++)
            {
                if ((wow.Objects[i].Health > 1) & (wow.Objects[i].Faction == wow.Objects[0].Faction))
                {
                    GroupInfo Temp = new GroupInfo();
                    Temp.Indexes = new List<int>();
                    if (Groups.Count == 0)
                    {
                        Temp.X = wow.Objects[i].X;
                        Temp.Y = wow.Objects[i].Y;
                        Temp.Z = wow.Objects[i].Z;
                        Temp.CountPlayers++;
                        Temp.Range = WowControl.CheckPoint(wow.Objects[0].X, wow.Objects[0].Y, wow.Objects[0].Z, wow.Objects[i].X, wow.Objects[i].Y, wow.Objects[i].Z);
                        Temp.Indexes.Add(i);
                        Groups.Add(Temp);
                    }
                    else
                    {
                        bool InGroup = false;
                        for (int g = 0; g < Groups.Count; g++)
                        {
                            double Range = WowControl.CheckPoint(wow.Objects[i].X, wow.Objects[i].Y, wow.Objects[i].Z, Groups[g].X, Groups[g].Y, Groups[g].Z);
                            if (Range < GroupRange)
                            {
                                InGroup = true;
                                GroupInfo[] TempGroup = Groups.ToArray();
                                //TempGroup[g].Indexes = new List<int>(Groups[g].Indexes.Count);
                                //for (int count = 0; count < Groups[g].Indexes.Count; count++)
                                    //TempGroup[g].Indexes.Add(Groups[g].Indexes[count]);
                                TempGroup[g].Indexes.Add(i);
                                TempGroup[g].CountPlayers++;
                                float x = 0, y = 0, z = 0;
                                for (int gi = 0; gi < TempGroup[g].Indexes.Count; gi++)
                                {
                                    x += wow.Objects[TempGroup[g].Indexes[gi]].X;
                                    y += wow.Objects[TempGroup[g].Indexes[gi]].Y;
                                    z += wow.Objects[TempGroup[g].Indexes[gi]].Z;
                                }
                                TempGroup[g].X = x / TempGroup[g].Indexes.Count;
                                TempGroup[g].Y = y / TempGroup[g].Indexes.Count;
                                TempGroup[g].Z = z / TempGroup[g].Indexes.Count;
                                TempGroup[g].Range = WowControl.CheckPoint(TempGroup[g].X, TempGroup[g].Y, TempGroup[g].Z, Groups[0].X, Groups[0].Y, Groups[0].Z);
                                Groups.Clear();
                                for (int c = 0; c < TempGroup.Length; c++)
                                {
                                    Groups.Add(TempGroup[c]);
                                    //for (int ii = 0; ii < TempGroup[c].Indexes.Count; ii++)
                                        //Groups[c].Indexes.Add(TempGroup[c].Indexes[ii]);
                                }
                            }
                        }
                        if (InGroup == false)
                        {
                            Temp.X = wow.Objects[i].X;
                            Temp.Y = wow.Objects[i].Y;
                            Temp.Z = wow.Objects[i].Z;
                            Temp.CountPlayers++;
                            Temp.Range = WowControl.CheckPoint(wow.Objects[0].X, wow.Objects[0].Y, wow.Objects[0].Z, wow.Objects[i].X, wow.Objects[i].Y, wow.Objects[i].Z);
                            Temp.Indexes.Add(i);
                            Groups.Add(Temp);
                        }
                    }
                }
            }
            double MinRange = 10000;
            int ClosestGroup = 1000;
			for (int i = 0; i < Groups.Count; i++)
			{
				double Range = WowControl.CheckPoint(wow.Objects[0].X, wow.Objects[0].Y, wow.Objects[0].Z, Groups[i].X, Groups[i].Y, Groups[i].Z);
				if (Range < RangeFromPlayerToGroup)
					if ((Groups[i].Range != 0) & (Groups[i].Range < MinRange) & (Groups[i].Range < RangeFromPlayerToGroup) & (Groups[i].CountPlayers >= MinPlayersInGroup))
					{
						MinRange = Groups[i].Range;
						ClosestGroup = i;
					}
			}
            return ClosestGroup;
        }

		private bool WaypointAlreadyInCourse(int WP, int Course)
		{
			bool result = false;
			for (int c = 0; c < Courses[Course].Count; c++)
				if (Courses[Course][c] == WP)
					return true;
			return result;
		}
		private bool WaypointAlreadyInSameCourse(int WP, int FinishWaypoint)
		{
			bool Result = false;
			List<int> TestCourse = new List<int>();
			for (int i = 0; i < Courses.Count; i++)
			{
				TestCourse.Clear();
				TestCourse = Courses[i];
				TestCourse.Add(WP);
				if (TestCourse == Courses[i])
				{
					int LastWaypoint = TestCourse[Courses[i].Count - 1];
					int NextWP = GetClosestWP(LastWaypoint, i, 300);
					if ((NextWP != -1) & ((LastWaypoint != FinishWaypoint)))
						Courses[i].Add(NextWP);					
					return true;
				}
			}
			return Result;
		}
		private void FindCourse(int FinishWaypoint)
		{
			int StartWaypoint = GetClosestWP(wow.Objects[0].X, wow.Objects[0].Y, wow.Objects[0].Z);
			WowControl.UpdateStatus("Detecting course from " + StartWaypoint + " to " + FinishWaypoint + ".");
			Courses = new List<List<int>>(WowControl.WP.Length);
			List<int> FirstCourse = new List<int>();
			BadCourses = new List<CourseInfo>();
			FirstCourse.Add(StartWaypoint);
			Courses.Add(FirstCourse);
			ProcessCourses(FinishWaypoint);
			if (CalculatedCourses.Count > 0)
				WowControl.UpdateStatus(CalculatedCourses.Count + " courses detected. Optimal lenght " + CalculatedCourses[0].Range);
		}
		private void AddCoursesWithNewWaypoint(int SourceCourse, int WP)
		{
			List<int> Temp = new List<int>();
			for (int i = 0; i < Courses[SourceCourse].Count; i++)
				Temp.Add(Courses[SourceCourse][i]);
			Temp.Add(WP);
			Courses.Add(Temp);
		}
		private int DetectWPSearchRange()
		{
			/*int CourseLenght = 0;
			for (int i = 0; i < WowControl.WP.Length; i++)
				if (i < WowControl.WP.Length - 1)
					CourseLenght += (int)WowControl.CheckPoint(WowControl.WP[i].X, WowControl.WP[i].Y, WowControl.WP[i].Z, WowControl.WP[i + 1].X, WowControl.WP[i + 1].Y, WowControl.WP[i + 1].Z);
			*/
			int I = WowControl.WP.Length / 2;
			int T = (int)WowControl.CheckPoint(WowControl.WP[I].X, WowControl.WP[I].Y, WowControl.WP[I].Z, WowControl.WP[I + 1].X, WowControl.WP[I + 1].Y, WowControl.WP[I + 1].Z);
			return (int)(T*1.3);//Convert.ToInt32(CourseLenght * 1.2 / WowControl.WP.Length);
		}
		private void ProcessCourses(int FinishWaypoint)
		{
			DateTime StartAnalyzTime = DateTime.Now;
			int LastWaypoint = -1;
			int WaypointsSearchRange = DetectWPSearchRange();
			for (int i = 0; i < Courses.Count; i++)
			{
				if ((DateTime.Now - StartAnalyzTime).TotalSeconds > 10)
					goto Output;
				LastWaypoint = Courses[i][Courses[i].Count - 1];
				while (LastWaypoint != FinishWaypoint)
				{
					LastWaypoint = Courses[i][Courses[i].Count - 1];
					List<WaypointsInfo> ClosestWaypoints = GetClosestWPList(LastWaypoint, i, WaypointsSearchRange, FinishWaypoint);
					for (int w = 0; w < ClosestWaypoints.Count; w++)
						if (w != 0)
							AddCoursesWithNewWaypoint(i, ClosestWaypoints[w].Index);
					if (ClosestWaypoints.Count >= 1)
						Courses[i].Add(ClosestWaypoints[0].Index);
					LastWaypoint = Courses[i][Courses[i].Count - 1];
					if (ClosestWaypoints.Count == 0)
					{
						if (LastWaypoint != FinishWaypoint)
						{
							CourseInfo T = new CourseInfo();
							T.Bad = true;
							T.BadTime = DateTime.Now;
							T.Index = i;
							if (CourseIsBad(i) == false)
								BadCourses.Add(T);
							Courses[i].Add(FinishWaypoint);
							LastWaypoint = Courses[i][Courses[i].Count - 1];
						} 
					}
				}
			}
			Output:
			CalculateCourses();
			CalculatedCourses.Sort((a, b) => a.Range.CompareTo(b.Range));
			
			List<String> OutData = new List<String>();
			for (int i = 0; i < CalculatedCourses.Count; i++)
			{
				StringBuilder Sb = new StringBuilder();
				for (int c = 0; c < Courses[CalculatedCourses[i].Index].Count; c++)
				{
					Sb.Append(Courses[CalculatedCourses[i].Index][c].ToString() + " ");
				}
				OutData.Add("Course lenght: " + CalculatedCourses[i].Range+"|"+ Sb.ToString());
			}
			
			OutData.Add("Analiz time: " + ((DateTime.Now - StartAnalyzTime).TotalMilliseconds).ToString());
			File.WriteAllLines("C:\\WPProcessTest.txt", OutData.ToArray());
			return;
		}
		private void CalculateCourses()
		{
			for (int i = 0; i < Courses.Count; i++)
			{
				if (CourseIsBad(i) == false)
				{
					int CourseLenght = 0;
					for (int c = 0; c < Courses[i].Count; c++)
					{
						if (c < Courses[i].Count - 1)
							CourseLenght += (int)WowControl.CheckPoint(WowControl.WP[Courses[i][c]].X, WowControl.WP[Courses[i][c]].Y, WowControl.WP[Courses[i][c]].Z, WowControl.WP[Courses[i][c + 1]].X, WowControl.WP[Courses[i][c + 1]].Y, WowControl.WP[Courses[i][c + 1]].Z);
					}
					WaypointsInfo Temp = new WaypointsInfo();
					Temp.Index = i;
					Temp.Range = CourseLenght;
					CalculatedCourses.Add(Temp);
				}
			}
		}
		private List<WaypointsInfo> GetClosestWPList(int WPNO, int Course, int WPRange, int FinishWaypoint)
		{
			List<WaypointsInfo> Result = new List<WaypointsInfo>();
			MainForm.WayPoints WP = new MainForm.WayPoints();
			WP.X = WowControl.WP[WPNO].X; WP.Y = WowControl.WP[WPNO].Y; WP.Z = WowControl.WP[WPNO].Z;
			for (int i = 0; i < WowControl.WP.Length; i++)
			{
				double Range = WowControl.CheckPoint(WowControl.WP[i].X, WowControl.WP[i].Y, WowControl.WP[i].Z, WP.X, WP.Y, WP.Z);
				if ((Range < WPRange) & (WaypointAlreadyInCourse(i, Course) == false))
				{
					WaypointsInfo Temp = new WaypointsInfo();
					Temp.Index = i;
					Temp.Range = Range;
					Result.Add(Temp);
				}
			}
			Result.Sort((a, b) => a.Range.CompareTo(b.Range));
			return Result;
		}
		private bool CourseIsBad(int Course)
		{
			for (int i = 0; i < BadCourses.Count; i++)
			{
				if (BadCourses[i].Index == Course)
					return true;
			}
			return false;
		}

    }
    
}
