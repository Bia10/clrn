using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Text;
using System.Windows.Forms;

namespace FarmHelper_beta
{
    public partial class MainForm : Form
    {
        private bool Running;
		SpellSearcher SS;
        private FarmHonor FH;
        private Farm Farmer;
        public Memory.Memory wow;
        private int LastLogLenght;
        private List<Coords> PCoord;
        public int CharNo, RealmNo;
        public bool Autoreconnect;
        public String LoadedProfile;
        public MainForm()
        {
			try
			{
				InitializeComponent();
				WowControl.BotStartTime = DateTime.Now;
				/*Updater Updtr = new Updater();
				if (Updtr.CheckForUpdate() == true)
					WowControl.UpdateStatus("New version available: " + Updtr.AvailableVersion + ". Your version: " + Updtr.CurrentVersion + "."); 
				else 
					WowControl.UpdateStatus("Your version is up to date.");*/
				ReadConfigs();
				InitMain();
				this.Text = WowControl.RandomString(20);
				Abilities.InitBinds();
				FillListbox();
			} catch (Exception)
			{
				//WowControl.UpdateStatus("Init main error " + E.Message + " " + E.InnerException.Message); 
			}
        }
        private void FillListbox()
        {
            listBox3.Items.Clear();
            for (int i = 0; i < Abilities.BindingsList.ToArray().Length; i++)
            {
                if ((Abilities.BindingsList[i].AbilityName != "") & (Abilities.BindingsList[i].AbilityName != null))
                    listBox3.Items.Add(Abilities.BindingsList[i].AbilityName);
            }
        }
        private void InitMain()
        {
            wow = new Memory.Memory();
			SS = new SpellSearcher();
            FH = new FarmHonor();
            Farmer = new Farm();
            Running = false;
            PCoord = new List<Coords>();
            if (wow.wow.ProcessId != 0)
            {
                notifyIcon1.Text = "Farmhelper: attached to wow: " + wow.wow.ProcessId.ToString();
                WowControl.UpdateStatus("Attached to wow: " + wow.wow.ProcessId.ToString() + ".");
                MainTimer.Enabled = true;
            }
            else
            {
                WowControl.UpdateStatus("Not found wow process.");
                notifyIcon1.Text = "Farmhelper: not found wow process";
            }
            WowControl.LoadGoods();
            ProcessMainPage();
        }
        private void ReadConfigs()
        {
            String[] Configs = Directory.GetFiles(Application.StartupPath+"\\Configs\\", "*.cfg");
            comboBox1.Items.Clear();
            for (int i = 0; i < Configs.Length; i++)
                comboBox1.Items.Add(GetCfgName(Configs[i]));
        }
        public String GetCfgName(String InputName)
        {
            int EndIndex = 0;
            for (int i = 0; i < InputName.Length; i++)
                if (InputName.Substring(i, 1) == "\\")
                    EndIndex = i;
            return InputName.Substring(EndIndex + 1, (InputName.Length - 4) - (EndIndex + 1));
        }
        public struct Coords
        {
            public float X;
            public float Y;
            public float Z;
        }
        public struct WayPoints
        {
            public float X;
            public float Y;
            public float Z;
            public int NextWaypoint;
        }
        private void MainTimer_Tick(object sender, EventArgs e)
        {
			try
			{
				String[] Files = Directory.GetFiles(Application.StartupPath, "*.exe");
				for (int i = 0; i < Files.Length; i++)
					if (Files[i].Substring(Files[i].Length - 14, 14) == "Newversion.exe")
						File.Delete(Files[i]);
				if (wow.wow.ProcessId != 0)
				{
					wow.Read();
					label18.Text = wow.LoadingState.ToString()+ wow.LoginState.ToString()+wow.Combopoints.ToString();
					if ((Autoreconnect == true) & (Running == true))
						CheckWowState();
					ProcessMainPage();
					if ((wow.Objects.ToArray().Length > 0) & (wow.Online == true))
					{
						if (Running == true)
						{
							if (WowControl.WaitTime > DateTime.Now)
							{
								//wow.GoTo(0, 0, 0, 0, 3);
								WowControl.UpdateStatus("Waiting.");
								return;
							}
							ProcessFarm();
						}
					}
					wow.ReadChat();
					ProcessChat();
				} else
				{
					InitMain();
					ProcessMainPage();
				}
			} catch (Exception) { }
        }
        private int GetLastStringIndex(String[] Chat)
        {
            int LastStringIndex = 0;
            for (int i = 0; i < Chat.Length; i++)
            {
                if (Chat[i] != null)
                    LastStringIndex = i;
            }
            return LastStringIndex;
        }
        private void ProcessChat()
        {
            if (AllChatBox.Items.Count == 0)
                for (int i = 0; i < WowControl.AllChatParsed.ToArray().Length; i++)
                    AllChatBox.Items.Add(WowControl.AllChatParsed[i]);
            else
            {
                int Len = WowControl.AllChatParsed.ToArray().Length;
                int Razn = Len - AllChatBox.Items.Count;
                if (Razn > 0)
                {
                    for (int i = Len - Razn; i < Len; i++)
                    {
                        AllChatBox.Items.Add(WowControl.AllChatParsed[i]);
                    }
                }
            }
            if (SayChatBox.Items.Count == 0)
                for (int i = 0; i < WowControl.SayChatParsed.ToArray().Length; i++)
                    SayChatBox.Items.Add(WowControl.SayChatParsed[i]);
            else
            {
                int Len = WowControl.SayChatParsed.ToArray().Length;
                int Razn = Len - SayChatBox.Items.Count;
                if (Razn > 0)
                {
                    for (int i = Len - Razn; i < Len; i++)
                    {
                        SayChatBox.Items.Add(WowControl.SayChatParsed[i]);
                    }
                }
            }
            if (WhisperChatBox.Items.Count == 0)
                for (int i = 0; i < WowControl.WisperChatParsed.ToArray().Length; i++)
                    WhisperChatBox.Items.Add(WowControl.WisperChatParsed[i]);
            else
            {
                int Len = WowControl.WisperChatParsed.ToArray().Length;
                int Razn = Len - WhisperChatBox.Items.Count;
                if (Razn > 0)
                {
                    for (int i = Len - Razn; i < Len; i++)
                    {
                        WhisperChatBox.Items.Add(WowControl.WisperChatParsed[i]);
                    }
                }
            }
            if (checkBox3.Checked == true)
            {
                AllChatBox.SelectedIndex = AllChatBox.Items.Count - 1;
                SayChatBox.SelectedIndex = SayChatBox.Items.Count - 1;
                WhisperChatBox.SelectedIndex = WhisperChatBox.Items.Count - 1;
            }
        }
        private void ProcessFarm()
        {
            if (WowControl.FarmHonor == true)
            {
                FH.wow = wow;
                FH.Process();
            }
            if ((WowControl.FarmSkinning == true) || (WowControl.FarmMining == true))
            {
                Farmer.wow = wow;
                Farmer.Process();
            }
        }
        private void ProcessMainPage()
        {

            if (LastLogLenght != WowControl.Log.ToArray().Length)
            {
                for (int i = LastLogLenght; i < WowControl.Log.Count; i++)
                {
                    AllListbox.Items.Add(WowControl.Log[i]);
                }
                LastLogLenght = WowControl.Log.ToArray().Length;
                AllListbox.SelectedIndex = LastLogLenght - 1;
            }
            if (LootListbox.Items.Count < WowControl.Loot.ToArray().Length)
            {
                for (int i = LootListbox.Items.Count; i < WowControl.Loot.ToArray().Length; i++)
                {
                    LootListbox.Items.Add(WowControl.Loot[i]);
                }
                LootListbox.SelectedIndex = LootListbox.Items.Count - 1;
            }
            if (CombatListbox.Items.Count < WowControl.CombatLog.ToArray().Length)
            {
                for (int i = CombatListbox.Items.Count; i < WowControl.CombatLog.ToArray().Length; i++)
                {
                    CombatListbox.Items.Add(WowControl.CombatLog[i]);
                }
                CombatListbox.SelectedIndex = CombatListbox.Items.Count - 1;
            }
            switch (wow.BgStatus)
            {
                case 0://Not queued
                    label11.Text = "BG status: Not queued";
                    break;
                case 1://Waiting
                    label11.Text = "BG status: Waiting";
                    break;
                case 2://Confirm
                    label11.Text = "BG status: Confirm join";
                    break;
                case 3://In BG
                    label11.Text = "BG status: Active";
                    break;
            }
            label12.Text = "Farm status: " + WowControl.FarmStatus;
            if (wow.Objects.Count > 0)
            {
                label1.Text = wow.Objects[0].Name.ToString();
                label3.Text = "Level " + wow.Objects[0].Level.ToString();
                label9.Text = Convert.ToInt32(wow.Objects[0].Health).ToString() + "/" + Convert.ToInt32(wow.Objects[0].MaxHealth).ToString();
                progressBar1.Value = Convert.ToInt32((Convert.ToDouble(wow.Objects[0].Health) / Convert.ToDouble(wow.Objects[0].MaxHealth)) * 100);
            }
            bool Target = false;
            for (int i = 0; i < wow.Objects.ToArray().Length; i++)
            {
                if (wow.Objects[i].IsTarget == true)
                {
                    label2.Text = wow.Objects[i].Name.ToString();
                    label4.Text = "Level " + wow.Objects[i].Level.ToString();
                    label10.Text = Convert.ToInt32(wow.Objects[i].Health).ToString() + "/" + Convert.ToInt32(wow.Objects[i].MaxHealth).ToString();
                    progressBar2.Value = Convert.ToInt32((Convert.ToDouble(wow.Objects[i].Health) / Convert.ToDouble(wow.Objects[i].MaxHealth)) * 100);
                    Target = true;
                }
            }
            if (Target == false)
            {
                label2.Text = "No target";
                label4.Text = "";
                label10.Text = "";
                progressBar2.Value = 0;
            }
        }

        private void button1_Click(object sender, EventArgs e)
        {
            if (wow.wow.ProcessId != 0)
            {
                if (Running == false)
                {
                    if (Defender.CheckLicense() == true)
                    {

                        WowControl.UpdateStatus("License key found.");
                        Running = true;
                        button1.Text = "Stop";
                        WowControl.UpdateStatus("Starting bot.");
                    }
                    else
                        WowControl.UpdateStatus("Invalid license key.");
                }
                else
                {
                    wow.GoTo(0, 0, 0, 0, 3);//Stop
                    WowControl.FarmStatus = "Stopped";
                    Running = false;
                    button1.Text = "Run";
                    WowControl.UpdateStatus("Stopping bot.");
                }
            }
            else WowControl.UpdateStatus("Can't run, not found wow process.");
        }
        private void checkBox7_CheckedChanged(object sender, EventArgs e)
        {
            if (checkBox7.Checked == true)
            {
                button6.Enabled = true;
                WowControl.UpdateStatus("Waypoint writing mode enabled.");
            }
            else
            {
                button6.Enabled = false;
                WowControl.UpdateStatus("Waypoint writing mode disabled.");
            }
        }

        private void checkBox5_CheckedChanged(object sender, EventArgs e)
        {
            if (checkBox5.Checked == true)
            {
                button7.Enabled = true;
                WowControl.UpdateStatus("Killing mob mode enabled.");
                WowControl.FarmSkinning = true;
            }
            else
            {
                button7.Enabled = false;
                WowControl.UpdateStatus("Killing mob mode disabled.");
                WowControl.FarmSkinning = false;
            }
        }

        private void checkBox6_CheckedChanged(object sender, EventArgs e)
        {
            if (checkBox6.Checked == true)
            {
                button8.Enabled = true;
                WowControl.UpdateStatus("Gathering mode enabled.");
                WowControl.FarmMining = true;
            }
            else
            {
                button8.Enabled = false;
                WowControl.UpdateStatus("Gathering mode disabled.");
                WowControl.FarmMining = false;
            }
        }

        private void checkBox8_CheckedChanged(object sender, EventArgs e)
        {
            if (checkBox8.Checked == true)
            {
                button9.Enabled = true;
                WowControl.UpdateStatus("Honor farm mode enabled.");
                MessageBox.Show("You must manually select bg (first time) to enable auto join!", "Farm helper info.", MessageBoxButtons.OK, MessageBoxIcon.Information);
                WowControl.FarmHonor = true;
            }
            else
            {
                button9.Enabled = false;
                WowControl.UpdateStatus("Honor farm mode disabled.");
                WowControl.FarmHonor = false;
            }
        }

        private void checkBox1_CheckedChanged(object sender, EventArgs e)
        {
            if (checkBox1.Checked == true)
            {
                Autoreconnect = true;
                textBox1.Enabled = true;
                textBox2.Enabled = true;
                comboBox2.Enabled = true;
                comboBox3.Enabled = true;
            }
            else
            {
                Autoreconnect = false;
                textBox1.Enabled = false;
                textBox2.Enabled = false;
                comboBox2.Enabled = false;
                comboBox3.Enabled = false;
            }
        }

        private void listBox3_SelectedIndexChanged(object sender, EventArgs e)
        {
			try
			{
				if (listBox3.SelectedIndex != -1)
				{
					try
					{
						if ((Abilities.BindingsList[listBox3.SelectedIndex - 1].NotEditable == false) & (Abilities.BindingsList[listBox3.SelectedIndex].NotEditable == false))
							button19.Enabled = true;
						else button19.Enabled = false;
						if ((listBox3.SelectedIndex < listBox3.Items.Count - 1) & (Abilities.BindingsList[listBox3.SelectedIndex].NotEditable == false))
							button18.Enabled = true;
						else button18.Enabled = false;
					} catch (Exception) { }
					if (Abilities.BindingsList[listBox3.SelectedIndex].NotEditable == false)
					{
						textBox4.Enabled = true;
						textBox5.Enabled = true;
						comboBox7.Enabled = true;
						comboBox4.Enabled = true;
						button13.Enabled = true;
						dataGridView1.Enabled = true;
					} else
					{
						comboBox4.Enabled = false;
						button13.Enabled = false;
						textBox4.Enabled = false;
						textBox5.Enabled = false;
						comboBox7.Enabled = false;
						dataGridView1.Enabled = false;
					}
					textBox7.Text = Abilities.BindingsList[listBox3.SelectedIndex].AbilityName;
					textBox4.Text = Abilities.BindingsList[listBox3.SelectedIndex].CoolDown.ToString();
					textBox5.Text = Abilities.BindingsList[listBox3.SelectedIndex].CastTime.ToString();
					try
					{
						comboBox4.SelectedIndex = Abilities.BindingsList[listBox3.SelectedIndex].AbilityType;
					} catch (Exception) { comboBox4.SelectedIndex = 0; }
					comboBox7.SelectedIndex = Abilities.BindingsList[listBox3.SelectedIndex].InCombat;
					textBox6.Text = Abilities.BindingsList[listBox3.SelectedIndex].AbilityTxt.ToString();
					dataGridView1.Rows.Clear();
					if (Abilities.BindingsList[listBox3.SelectedIndex].OptionsList != null)
						for (int i = 0; i < Abilities.BindingsList[listBox3.SelectedIndex].OptionsList.Count; i++)
						{
								dataGridView1.Rows.Add();
								try
								{
									dataGridView1.Rows[i].Cells[0].Value = Abilities.BindingsList[listBox3.SelectedIndex].OptionsList[i].Unit;
									dataGridView1.Rows[i].Cells[1].Value = Abilities.BindingsList[listBox3.SelectedIndex].OptionsList[i].Parameter;
									dataGridView1.Rows[i].Cells[2].Value = Abilities.BindingsList[listBox3.SelectedIndex].OptionsList[i].Condition;
									dataGridView1.Rows[i].Cells[3].Value = Abilities.BindingsList[listBox3.SelectedIndex].OptionsList[i].Value;
									dataGridView1.Rows[i].Cells[4].Value = Abilities.BindingsList[listBox3.SelectedIndex].OptionsList[i].AndOr;
								} catch (System.Data.DataException E) { 
									String EE = E.Message; 
								}
							if (i == Abilities.BindingsList[listBox3.SelectedIndex].OptionsList.Count - 1)
								dataGridView1.Rows[i].Cells[4].ReadOnly = true;
							else dataGridView1.Rows[i].Cells[4].ReadOnly = false;
						}
				} else
				{
					button19.Enabled = false;
					button18.Enabled = false;
					button13.Enabled = false;
					textBox4.Enabled = false;
					textBox5.Enabled = false;
					comboBox7.Enabled = false;
					comboBox4.Enabled = false;
				}
			} catch (Exception) { }
        }

        private void button12_Click(object sender, EventArgs e)
        {
            Abilities.Ability Clear = new Abilities.Ability();
            Clear.AbilityTxt = "";
            NameDialog ND = new NameDialog();
            ND.ShowDialog();
            if ((ND.CfgName != "") & (ND.CfgName != null))
            {
                Clear.AbilityName = ND.CfgName;
                listBox3.Items.Add(Clear.AbilityName);
                Abilities.BindingsList.Add(Clear);
            }
        }

        private void button13_Click(object sender, EventArgs e)
        {
            if (listBox3.SelectedIndex >= 0)
            {
                Abilities.BindingsList.RemoveAt(listBox3.SelectedIndex);
                listBox3.Items.RemoveAt(listBox3.SelectedIndex);
            }
        }

        private void textBox4_TextChanged(object sender, EventArgs e)
        {
            try
            {
                if ((textBox4.Text != "") & (listBox3.SelectedIndex != -1))
                {
                    Abilities.Ability[] TempArray = Abilities.BindingsList.ToArray();
                    TempArray[listBox3.SelectedIndex].CoolDown = (float)Convert.ToDouble(textBox4.Text);
                    Abilities.BindingsList.Clear();
                    for (int i = 0; i < TempArray.Length; i++)
                        Abilities.BindingsList.Add(TempArray[i]);
                }
            }
            catch (Exception E) { WowControl.UpdateStatus(textBox4.Text + ": " + E.Message); }
        }

        private void textBox5_TextChanged(object sender, EventArgs e)
        {
            try
            {
                if ((textBox5.Text != "") & (listBox3.SelectedIndex != -1))
                {
                    Abilities.Ability[] TempArray = Abilities.BindingsList.ToArray();
                    TempArray[listBox3.SelectedIndex].CastTime = (float)Convert.ToDouble(textBox5.Text);
                    Abilities.BindingsList.Clear();
                    for (int i = 0; i < TempArray.Length; i++)
                        Abilities.BindingsList.Add(TempArray[i]);
                }
            }
            catch (Exception E) { WowControl.UpdateStatus(textBox5.Text + ": " + E.Message); }
        }

        private void comboBox7_SelectedIndexChanged(object sender, EventArgs e)
        {
            try
            {
                if (listBox3.SelectedIndex != -1)
                {
                    Abilities.Ability[] TempArray = Abilities.BindingsList.ToArray();
                    TempArray[listBox3.SelectedIndex].InCombat = comboBox7.SelectedIndex;
                    Abilities.BindingsList.Clear();
                    for (int i = 0; i < TempArray.Length; i++)
                        Abilities.BindingsList.Add(TempArray[i]);
                }
            }
            catch (Exception) { }
        }
        private void button10_Click(object sender, EventArgs e)
        {
            switch (tabControl2.SelectedIndex)
            {
                case 0:
                    WowControl.Command("say " + textBox8.Text);
                    textBox8.Text = "";
                    break;
                case 1:
                    if (WhisperChatBox.SelectedIndex != -1)
                    {
                        String Player = GetPlayerNameFromString(WhisperChatBox.Items[WhisperChatBox.SelectedIndex].ToString());
                        if (Player != "")
                            WowControl.Command("w " + Player + " " + textBox8.Text);
                        textBox8.Text = "";
                    }
                    break;
                case 2:
                    WowControl.Command(textBox8.Text);
                    textBox8.Text = "";
                    break;
            }
        }
        private String GetPlayerNameFromString(String Str)
        {
            Str = Str.Substring(12, Str.Length - 12);
            for (int i = 0; i < Str.Length; i++)
                if (Str.Substring(i, 1) == "]")
                    return Str.Substring(0, i);
            return "";
        }

        private void tabControl2_SelectedIndexChanged(object sender, EventArgs e)
        {
            switch (tabControl2.SelectedIndex)
            {
                case 0:
                    button10.Text = "/s";
                    break;
                case 1:
                    if (WhisperChatBox.SelectedIndex != -1)
                    {
                        String Player = GetPlayerNameFromString(WhisperChatBox.Items[WhisperChatBox.SelectedIndex].ToString());
                        if (Player != "")
                            button10.Text = "/w " + Player;
                    }
                    else button10.Text = "Select player";
                    break;
                case 2:
                    button10.Text = "/";
                    break;
            }
        }

        private void WhisperChatBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (tabControl2.SelectedIndex == 1)
            {
                if (WhisperChatBox.SelectedIndex != -1)
                {
                    String Player = GetPlayerNameFromString(WhisperChatBox.Items[WhisperChatBox.SelectedIndex].ToString());
                    if (Player != "")
                        button10.Text = "/w " + Player;
                }
            }
        }
        private void SaveCfg(String CfgName)
        {
            WowControl.Config.Clear();
            WowControl.Config.Add("[Variables]");
            WowControl.SaveVariable(WowControl.FarmHonor, "FarmHonor");
            WowControl.SaveVariable(WowControl.FarmMining, "FarmMining");
            WowControl.SaveVariable(WowControl.FarmSkinning, "FarmSkinning");
            WowControl.SaveVariable(WowControl.SkiningKillAll, "SkiningKillAll");
            WowControl.SaveVariable(WowControl.SkinAll, "SkinAll");
            WowControl.SaveVariable(WowControl.LootMobs, "LootMobs");
            WowControl.SaveVariable(WowControl.StayWithGroup, "StayWithGroup");
            WowControl.SaveVariable(WowControl.ClassType, "ClassType");
            WowControl.SaveVariable(WowControl.FriendCheckRange, "CheckRange");
            WowControl.SaveVariable(WowControl.EnemyCheckRange, "EnemyCheckRange");
            WowControl.SaveVariable(WowControl.HealDDPriority, "HealDDPriority");
            WowControl.SaveVariable(WowControl.GroupRange, "GroupRange");
            WowControl.SaveVariable(WowControl.MinPlayersInGroup, "MinPlayersInGroup");
            WowControl.SaveVariable(WowControl.RangeFromPlayerToGroup, "RangeFromPlayerToGroup");
            WowControl.SaveVariable(WowControl.SkiningRange, "SkiningRange");
            WowControl.SaveVariable(WowControl.FarmMount, "FarmMount");
            WowControl.SaveVariable(Autoreconnect, "Autoreconnect");
            WowControl.SaveVariable(WowControl.Account, "Account");
            WowControl.SaveVariable(CharNo, "CharNo");
            WowControl.SaveVariable(RealmNo, "RealmNo");
            Abilities.MakeBinds();
            WowControl.Config.Add("[End]");
            if (CfgName != "")
            {
                File.WriteAllLines(Application.StartupPath + "\\Configs\\" + CfgName + ".cfg", WowControl.Config.ToArray());
				WowControl.Log.Add(WowControl.GetTimeStr() + " Config " + CfgName + " saved!");
            }
            else MessageBox.Show("Type config name", "Farm helper error.", MessageBoxButtons.OK, MessageBoxIcon.Error);
        }
        private void LoadCfg(String CfgName)
        {
            WowControl.LoadCfgFromFile(CfgName);
            FillListbox();
            checkBox8.Checked = WowControl.LoadVariableBool("FarmHonor");
            checkBox6.Checked = WowControl.LoadVariableBool("FarmMining");
            checkBox5.Checked = WowControl.LoadVariableBool("FarmSkinning");
            checkBox7.Checked = WowControl.LoadVariableBool("WPWrite");
            checkBox1.Checked = WowControl.LoadVariableBool("Autoreconnect");
            comboBox3.SelectedIndex = WowControl.LoadVariableInt("CharNo");
            comboBox2.SelectedIndex = WowControl.LoadVariableInt("RealmNo");
            comboBox8.SelectedIndex = WowControl.LoadVariableInt("ClassType");
            textBox1.Text = WowControl.LoadVariableString("Account");
            WowControl.MinPlayersInGroup = WowControl.LoadVariableInt("MinPlayersInGroup");
            WowControl.FriendCheckRange = WowControl.LoadVariableInt("CheckRange");
            WowControl.EnemyCheckRange = WowControl.LoadVariableInt("EnemyCheckRange");
            WowControl.HealDDPriority = WowControl.LoadVariableInt("HealDDPriority");
            WowControl.GroupRange = WowControl.LoadVariableInt("GroupRange");
            WowControl.RangeFromPlayerToGroup = WowControl.LoadVariableInt("RangeFromPlayerToGroup");
            WowControl.StayWithGroup = WowControl.LoadVariableBool("StayWithGroup");
            WowControl.LootMobs = WowControl.LoadVariableBool("LootMobs");
            WowControl.FarmMount = WowControl.LoadVariableInt("FarmMount");
            WowControl.SkiningKillAll = WowControl.LoadVariableBool("SkiningKillAll");
            WowControl.SkiningRange = WowControl.LoadVariableInt("SkiningRange");
            WowControl.SkinAll = WowControl.LoadVariableBool("SkinAll");
            WowControl.UpdateStatus("Config " + comboBox1.Text + " loaded!");
        }
        private void button2_Click(object sender, EventArgs e)
        {
            try
            {
                if (comboBox1.Text != "")
                    LoadCfg(Application.StartupPath + "\\Configs\\" + comboBox1.Text + ".cfg");
            }
            catch (Exception)
            {
                MessageBox.Show("Bad config file!", "Farm helper error.", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        private void button3_Click(object sender, EventArgs e)
        {
            SaveCfg(comboBox1.Text);
        }

        private void textBox1_TextChanged(object sender, EventArgs e)
        {
            WowControl.Account = textBox1.Text;
        }

        private void textBox2_TextChanged(object sender, EventArgs e)
        {
            WowControl.Password = textBox2.Text;
        }

        private void comboBox2_SelectedIndexChanged(object sender, EventArgs e)
        {
            RealmNo = comboBox2.SelectedIndex;
        }

        private void comboBox3_SelectedIndexChanged(object sender, EventArgs e)
        {
            CharNo = comboBox3.SelectedIndex;
        }

        private void button4_Click(object sender, EventArgs e)
        {
            NameDialog ND = new NameDialog();
            ND.ShowDialog();
            if (ND.CfgName != "")
                SaveCfg(ND.CfgName);
            ReadConfigs();
        }

        private void button5_Click(object sender, EventArgs e)
        {
            if (MessageBox.Show("You are really wanna delete " + comboBox1.Text + ".cfg ?", "Farm helper request.", MessageBoxButtons.YesNo, MessageBoxIcon.Question) == DialogResult.Yes)
            {
                File.Delete(Application.StartupPath + "\\Configs\\" + comboBox1.Text + ".cfg");
                WowControl.Sleep(100);
                WowControl.UpdateStatus("Config " + comboBox1.Text + " deleted!");
                comboBox1.Text = "";
                ReadConfigs();
            }
        }

        private void textBox6_KeyDown(object sender, KeyEventArgs e)
        {
            if (listBox3.SelectedIndex < 0)
                return;
            try
            {
                Abilities.Ability[] TempArray = Abilities.BindingsList.ToArray();
                if (e.KeyData.ToString() == "Escape")
                {
                    TempArray[listBox3.SelectedIndex].AbilityTxt = "";
                    TempArray[listBox3.SelectedIndex].Key = 0;
                    TempArray[listBox3.SelectedIndex].Modifiers = "";
                    textBox6.Text = "";
                }
                else
                {
                    textBox6.Text = e.KeyData.ToString();
                    TempArray[listBox3.SelectedIndex].AbilityTxt = e.KeyData.ToString();
                    TempArray[listBox3.SelectedIndex].Key = e.KeyValue;
                    TempArray[listBox3.SelectedIndex].Modifiers = e.Modifiers.ToString();
                }
                Abilities.BindingsList.Clear();
                for (int i = 0; i < TempArray.Length; i++)
                    Abilities.BindingsList.Add(TempArray[i]);
            }
            catch (Exception) { }
        }

        private void comboBox8_SelectedIndexChanged(object sender, EventArgs e)
        {
            WowControl.ClassType = comboBox8.SelectedIndex;
        }

        private void button6_Click(object sender, EventArgs e)
        {
            WaypointWriter WPW = new WaypointWriter();
            WPW.ShowDialog();
            WPW.Dispose();
        }

        private void button8_Click(object sender, EventArgs e)
        {
            MiningOptions MO = new MiningOptions();
            MO.ShowDialog();
            MO.Dispose();
        }

        private void button11_Click(object sender, EventArgs e)
        {
            SerialForm SF = new SerialForm();
			SF.ShowDialog();
            SF.Dispose();
        }

        private void button14_Click(object sender, EventArgs e)
        {
			SS.ShowDialog();
			/*
            Browser B = new Browser();
            NameDialog ND = new NameDialog();
            ND.textBox1.Text = "Spell name";
            ND.ShowDialog();
            if ((ND.CfgName == "") || (ND.CfgName == null))
                return;
            List<String> Name = new List<string>();
            int LastSpace = -1;
            for (int i = 0; i < ND.CfgName.Length; i++)
            {
                if (ND.CfgName.Substring(i, 1) == " ")
                {
                    Name.Add(ND.CfgName.Substring(LastSpace + 1, i - LastSpace - 1));
                    LastSpace = i;
                }
            }
            if (Name.Count < 2)
                LastSpace = 0;
            Name.Add(ND.CfgName.Substring(LastSpace, ND.CfgName.Length - LastSpace));
            StringBuilder Sb = new StringBuilder();
            for (int i = 0; i < Name.Count; i++)
            {
                Sb.Append(Name[i]);
                if (Name.Count > i + 1)
                    Sb.Append("+");
            }
            Sb.Append("+Spell+ID");
            String NameToFind = Sb.ToString();
            B.webBrowser1.Navigate("http://www.google.com/search?hl=ru&client=opera&rls=ru&q=+site:thottbot.com+" + NameToFind + "&ei=8jECTKjjPMSgOMPCmdcE&sa=X&oi=manybox&resnum=4&ct=all-results&ved=0CAIQqAQwBA");
            B.Show();
			 * */
        }

        private void checkBox4_CheckedChanged(object sender, EventArgs e)
        {
            if (checkBox4.Checked == true)
                this.TopMost = true;
            else this.TopMost = false;
        }

        private void button15_Click(object sender, EventArgs e)
        {
            this.Hide();
        }

        private void toolStripMenuItem1_Click(object sender, EventArgs e)
        {
            this.Show();
        }

        private void toolStripMenuItem2_Click(object sender, EventArgs e)
        {
            Application.Exit();
        }

        private void notifyIcon1_MouseDoubleClick(object sender, MouseEventArgs e)
        {
            this.Show();
        }

        private void button16_Click(object sender, EventArgs e)
        {
            if (Process.GetProcessesByName("wow").Length > 1)
            {
                ProcessSelect PS = new ProcessSelect();
                PS.ShowDialog();
                wow = new Memory.Memory();
                wow.PId = PS.PId;
                wow.InitMemory();
                FH = new FarmHonor();
                Farmer = new Farm();
                Running = false;
                PCoord = new List<Coords>();
                if (wow.wow.ProcessId != 0)
                {
                    WowControl.UpdateStatus("Attached to wow: " + wow.wow.ProcessId.ToString() + ".");
                    MainTimer.Enabled = true;
                    notifyIcon1.Text = "Farmhelper: attached to wow: " + wow.wow.ProcessId.ToString();
                }
                else
                {
                    WowControl.UpdateStatus("Not found wow process.");
                    notifyIcon1.Text = "Farmhelper: not found wow process";
                }
                WowControl.LoadGoods();
                ProcessMainPage();
            }
            if (Process.GetProcessesByName("wow").Length == 1)
            {
                WowControl.UpdateStatus("Only one process found.");
                InitMain();
            }
            if (Process.GetProcessesByName("wow").Length == 0)
                WowControl.UpdateStatus("Not found wow process.");
        }

        private void button7_Click(object sender, EventArgs e)
        {
            SkiningOptions SO = new SkiningOptions();
            SO.ShowDialog();
            SO.Dispose();
        }

        private void button17_Click(object sender, EventArgs e)
        {
            Updater Updtr = new Updater();
            if (Updtr.CheckForUpdate() == true)
            {
                WowControl.UpdateStatus("New version available: " + Updtr.AvailableVersion + ". Your version: " + Updtr.CurrentVersion + ".");
                ProcessMainPage();
                WowControl.UpdateStatus("Downloading last version, please wait.");
                ProcessMainPage();
                Updtr.DownloadNewVersion();
                WowControl.UpdateStatus("Download complete.");
                ProcessMainPage();
                RunNewVersion();
            }
            else
            {
                WowControl.UpdateStatus("Your version is up to date.");
                ProcessMainPage();
            }
        }
        private void RunNewVersion()
        {
            Process New = new Process();
            New.StartInfo.FileName = Application.StartupPath + "\\newversion.exe";
            New.Start();
            Application.Exit();
        }
        private void CheckWowState()
        {
            if (wow.Online == false)
            {
              /*  if (wow.LoginState == "login")
                {
                    if (WowControl.Account == "")
                    {
                        WowControl.UpdateStatus("Not found password, can't login.");
                        return;
                    }
                    WowControl.UpdateStatus("Login screen detected, logining in.");
                    ProcessMainPage();
                    WowControl.Login();
                }
                if (wow.LoginState == "charselect")
                {
                    WowControl.UpdateStatus("Character selection screen detected, selecting character.");
                    ProcessMainPage();
                    WowControl.CharSelect();
                }*/
            }
        }
        private void FillAbilityOptions()
        {
            Abilities.Ability[] Temp = Abilities.BindingsList.ToArray();
            int Rows = dataGridView1.Rows.Count;
            int AbilityIndex=listBox3.SelectedIndex;
            if (AbilityIndex==-1)
                return;
            Temp[AbilityIndex].OptionsList = new List<Abilities.AbilityOptions>();
            for (int r = 0; r < Rows; r++)
            {
                Abilities.AbilityOptions TempOptions = new Abilities.AbilityOptions();
                try
                {
                    TempOptions.Unit = dataGridView1.Rows[r].Cells[0].Value.ToString();
                    TempOptions.Parameter = dataGridView1.Rows[r].Cells[1].Value.ToString();
                    TempOptions.Condition = dataGridView1.Rows[r].Cells[2].Value.ToString();
                    TempOptions.Value = dataGridView1.Rows[r].Cells[3].Value.ToString();
                    TempOptions.AndOr = dataGridView1.Rows[r].Cells[4].Value.ToString();
                    Temp[AbilityIndex].OptionsList.Add(TempOptions);
                }
                catch (Exception) 
                {
                    if ((TempOptions.Unit != null) || (TempOptions.Parameter != null) || (TempOptions.Condition != null) || (TempOptions.Value != null) || (TempOptions.AndOr != null))
                        Temp[AbilityIndex].OptionsList.Add(TempOptions); 
                }
            }
            Abilities.BindingsList.Clear();
            for (int i = 0; i < Temp.Length; i++)
                Abilities.BindingsList.Add(Temp[i]);
        }

        private void dataGridView1_MouseMove(object sender, MouseEventArgs e)
        {
			FillAbilities();
        }
		private void FillAbilities()
		{
			try
			{
				dataGridView1.EndEdit();
				FillAbilityOptions();
				if ((Abilities.BindingsList[listBox3.SelectedIndex].OptionsList != null) & (listBox3.SelectedIndex != -1))
					for (int i = 0; i < Abilities.BindingsList[listBox3.SelectedIndex].OptionsList.Count; i++)
					{
						if (i == Abilities.BindingsList[listBox3.SelectedIndex].OptionsList.Count - 1)
							dataGridView1.Rows[i].Cells[4].ReadOnly = true;
						else dataGridView1.Rows[i].Cells[4].ReadOnly = false;
					}
			} catch (Exception) { }
		}
        private void button19_Click(object sender, EventArgs e)
        {
            try
            {
                if (listBox3.SelectedIndex != -1)
                {
                    int LastIndex = listBox3.SelectedIndex;
                    Abilities.Ability MovedUp = Abilities.BindingsList[LastIndex];
                    Abilities.Ability MovedDown = Abilities.BindingsList[LastIndex - 1];
                    Abilities.BindingsList.RemoveRange(LastIndex - 1, 2);
                    Abilities.BindingsList.Insert(LastIndex - 1, MovedUp);
                    Abilities.BindingsList.Insert(LastIndex, MovedDown);
                    FillListbox();
                    listBox3.SelectedIndex = LastIndex - 1;
                }
            }
            catch (Exception) { }
        }

        private void button18_Click(object sender, EventArgs e)
        {
            try
            {
                if (listBox3.SelectedIndex != -1)
                {
                    int LastIndex = listBox3.SelectedIndex;
                    Abilities.Ability MovedUp = Abilities.BindingsList[LastIndex + 1];
                    Abilities.Ability MovedDown = Abilities.BindingsList[LastIndex];
                    Abilities.BindingsList.RemoveRange(LastIndex, 2);
                    Abilities.BindingsList.Insert(LastIndex, MovedUp);
                    Abilities.BindingsList.Insert(LastIndex + 1, MovedDown);
                    FillListbox();
                    listBox3.SelectedIndex = LastIndex + 1;
                }
            }
            catch (Exception) { }
        }

        private void button9_Click(object sender, EventArgs e)
        {
            FarmHonorOptions FHO = new FarmHonorOptions();
            FHO.ShowDialog();
            FHO.Dispose();
        }

        private void comboBox4_SelectedIndexChanged(object sender, EventArgs e)
        {
            try
            {
                if (listBox3.SelectedIndex != -1)
                {
                    Abilities.Ability[] TempArray = Abilities.BindingsList.ToArray();
                    TempArray[listBox3.SelectedIndex].AbilityType = comboBox4.SelectedIndex;
                    Abilities.BindingsList.Clear();
                    for (int i = 0; i < TempArray.Length; i++)
                        Abilities.BindingsList.Add(TempArray[i]);
                }
            }
            catch (Exception) { }
        }

		private void dataGridView1_CellClick(object sender, DataGridViewCellEventArgs e)
		{
			try
			{
				int AbilityIndex = listBox3.SelectedIndex;
				if (AbilityIndex == -1)
					return;
				int Row = e.RowIndex;
				int Column = e.ColumnIndex;
				if (Column == 3)
				{//был клик по "Value"
					if (Abilities.BindingsList[AbilityIndex].OptionsList[Row].Parameter == "SpellID")
					{
						SS.ShowDialog();
						if (SS.OutSid != 0)
						{
							dataGridView1.Rows[Row].Cells[Column].Value = SS.OutSid;
							FillAbilities();
						}
					}
				}
			} catch (Exception) { }
		}

		private void dataGridView1_DataError(object sender, DataGridViewDataErrorEventArgs e)
		{
			WowControl.Log.Add(WowControl.GetTimeStr() + "Got error while loading binds. Maybe old version cfg.");
		}
    }
}
