using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.IO;

namespace FarmHelper_beta
{
	public partial class SpellSearcher : Form
	{
		public SpellSearcher()
		{
			Rank = 0;
			InitializeComponent();
			if (WowControl.AllSpells.Length == 0)
				WowControl.AllSpells = File.ReadAllLines(Application.StartupPath + "\\Data\\SpellIDs.fh");
		}
		public int OutSid, Rank;
		public SpellInfo[] FindedSpells;
		public struct SpellInfo
		{
			public int SpellID;
			public String SpellName;
			public int SpellRank;
		}
		private SpellInfo[] GetSpellsByName(String Name)
		{
			List<SpellInfo> Result = new List<SpellInfo>();
			for (int i = 0; i < WowControl.AllSpells.Length; i++)
			{
				if (WowControl.FindTextInString(WowControl.AllSpells[i].ToLower(), Name) == true)
					Result.Add(GetSpellFromString(WowControl.AllSpells[i]));
			}
			return Result.ToArray();
		}
		private SpellInfo[] GetSpellsByName(String Name, int Rank)
		{
			List<SpellInfo> Result = new List<SpellInfo>();
			for (int i = 0; i < WowControl.AllSpells.Length; i++)
			{
				SpellInfo temp = GetSpellFromString(WowControl.AllSpells[i]);
				if ((temp.SpellRank == Rank) & (WowControl.FindTextInString(WowControl.AllSpells[i].ToLower(), Name) == true))
					Result.Add(temp);
			}
			return Result.ToArray();
		}
		private SpellInfo GetSpellFromString(String Source)
		{
			int StartSpellID=0, EndSpellID = 0, StartSpellName = 0, EndSpellName = 0, StartRank = 0, EndRank = 0;
			SpellInfo Result = new SpellInfo();
			try
			{
				for (int i = 0; i < Source.Length - 4; i++)
				{
					if (Source.Substring(i, 4) == "SID:")
					{
						StartSpellID = 4;
					}
					if ((StartSpellID != 0) & (Source.Substring(i, 4) == ";SN:"))
					{
						EndSpellID = i;
						StartSpellName = i + 4;
					}
					if ((StartSpellName != 0) & (Source.Substring(i, 4) == ";SR:"))
					{
						EndSpellName = i;
						StartRank = i + 4;
					}
					if (StartRank != 0)
						EndRank = Source.Length - 1;
				}
				Result.SpellID = Convert.ToInt32(Source.Substring(StartSpellID, EndSpellID - StartSpellID));
				Result.SpellName = Source.Substring(StartSpellName, EndSpellName - StartSpellName);
				Result.SpellRank = Convert.ToInt32(Source.Substring(StartRank, EndRank - StartRank));
			} catch (Exception) { }
			return Result;
		}
		private String GetSpellNameByID(int ID)
		{
			for (int i = 0; i < WowControl.AllSpells.Length; i++)
			{
				SpellInfo temp = GetSpellFromString(WowControl.AllSpells[i]);
				if (temp.SpellID == ID)
					if (temp.SpellRank != 0)
						return temp.SpellName + " (Rank " + temp.SpellRank + ")";
					else return temp.SpellName;
			}
			return "Unknown";
		}
		private void listBox1_SelectedIndexChanged(object sender, EventArgs e)
		{
			try
			{
				textBox2.Text = FindedSpells[listBox1.SelectedIndex].SpellID.ToString();
			} catch (Exception) { }
		}

		private void button3_Click(object sender, EventArgs e)
		{
			if (WowControl.PlayerBuffs.Count == 0)
			{
				listBox2.Items.Clear();
				listBox2.Items.Add("No auras");
			}
			if (WowControl.PlayerBuffs.Count > 0)
				if (WowControl.PlayerBuffs.Count != listBox2.Items.Count)
				{
					listBox2.Items.Clear();
					for (int i = 0; i < WowControl.PlayerBuffs.Count; i++)
						listBox2.Items.Add(GetSpellNameByID((int)WowControl.PlayerBuffs[i]) + " ID: " + WowControl.PlayerBuffs[i].ToString());
				}
			if (WowControl.TargetBuffs.Count == 0)
			{
				listBox3.Items.Clear();
				listBox3.Items.Add("No auras");
			}
			if (WowControl.TargetBuffs.Count > 0)
				if (WowControl.TargetBuffs.Count != listBox3.Items.Count)
				{
					listBox3.Items.Clear();
					for (int i = 0; i < WowControl.TargetBuffs.Count; i++)
						listBox3.Items.Add(GetSpellNameByID((int)WowControl.TargetBuffs[i]) + " ID: " + WowControl.PlayerBuffs[i].ToString());
				}
		}

		private void button2_Click(object sender, EventArgs e)
		{
			Search();
		}
		private void Search()
		{
			listBox1.Items.Clear();
			if (Rank == 0)
				FindedSpells = GetSpellsByName(textBox1.Text.ToLower());
			else FindedSpells = GetSpellsByName(textBox1.Text.ToLower(), Rank);
			for (int i = 0; i < FindedSpells.Length; i++)
			{
				if (FindedSpells[i].SpellRank != 0)
					listBox1.Items.Add(FindedSpells[i].SpellName + " (Rank: " + FindedSpells[i].SpellRank + ") ID: " + FindedSpells[i].SpellID);
				else listBox1.Items.Add(FindedSpells[i].SpellName + " ID: " + FindedSpells[i].SpellID);
			}
		}
		private void button1_Click(object sender, EventArgs e)
		{
			try
			{
				OutSid = FindedSpells[listBox1.SelectedIndex].SpellID;
			} catch (Exception) { } 
			this.Hide();
		}

		private void checkBox1_CheckedChanged(object sender, EventArgs e)
		{
			if (checkBox1.Checked == true)
				this.TopMost = true;
			else this.TopMost = false;
		}

		private void textBox1_KeyDown(object sender, KeyEventArgs e)
		{
			if (e.KeyValue == 13)
				Search();
		}

		private void textBox3_TextChanged(object sender, EventArgs e)
		{
			if (textBox3.Text == "")
				Rank = 0;
			else
				Rank = Convert.ToInt32(textBox3.Text);
		}

		private void textBox3_KeyDown(object sender, KeyEventArgs e)
		{
			if (e.KeyValue==13)
				Search();
		}

		private void button4_Click(object sender, EventArgs e)
		{
			OutSid = 0;
			this.Hide();
		}
	
	
	}
}
