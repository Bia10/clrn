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
    public partial class WaypointWriter : Form
    {
        public WaypointWriter()
        {
            InitializeComponent();
            wow = new Memory.Memory();
            PCoord = new List<MainForm.Coords>();
            WPRange = 100;
            LoadWaypoints("");
            LoadedLocation = "";
            SelectedLoc = -1;
        }
        private List<MainForm.WayPoints> WayPoints = new List<MainForm.WayPoints>();
        private Memory.Memory wow;
        private int WPRange, SelectedLoc;
        private List<MainForm.Coords> PCoord;
        private String LoadedLocation;
        public void LoadWaypoints(String Location)
        {
            String[] WPTxt = File.ReadAllLines(Application.StartupPath+"\\Data\\Waypoints.fh");
            WayPoints.Clear();
            comboBox1.Items.Clear();
            List<MainForm.WayPoints> Result = new List<MainForm.WayPoints>();
            MainForm.WayPoints Temp = new MainForm.WayPoints();
            String Loc = "";
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
                if (Loc!= WPTxt[i].Substring(S6 + 1, S7 - S6 - 1))
                {
                    comboBox1.Items.Add(WPTxt[i].Substring(S6 + 1, S7 - S6 - 1));
                    Loc = WPTxt[i].Substring(S6 + 1, S7 - S6 - 1);
                }
                if (Loc == Location)
                {
                    WayPoints.Add(Temp);
                }
           }
        }

        private void button1_Click(object sender, EventArgs e)
        {
            if (comboBox1.SelectedIndex != -1)
            {
                SelectedLoc = comboBox1.SelectedIndex;
                listBox1.Items.Clear();
                LoadedLocation = comboBox1.Items[comboBox1.SelectedIndex].ToString();
                LoadWaypoints(comboBox1.Items[comboBox1.SelectedIndex].ToString());
                for (int i = 0; i < WayPoints.Count; i++)
                    listBox1.Items.Add("№ " + i.ToString() + " X= " + WayPoints[i].X + " Y= " + WayPoints[i].Y + " Z= " + WayPoints[i].Z + " Next= " + WayPoints[i].NextWaypoint);
            }
            else
            {
                MessageBox.Show("Select location first!", "Farm helper info.", MessageBoxButtons.OK, MessageBoxIcon.Information);
                return;
            }
            checkBox2.Checked = false;
        }
        private void listBox1_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (listBox1.SelectedIndex >= 0)
            {
                button3.Enabled = true;
                textBox1.Enabled = true;
                textBox2.Enabled = true;
                textBox3.Enabled = true;
                textBox4.Enabled = true;
                textBox1.Text = WayPoints[listBox1.SelectedIndex].X.ToString();
                textBox2.Text = WayPoints[listBox1.SelectedIndex].Y.ToString();
                textBox3.Text = WayPoints[listBox1.SelectedIndex].Z.ToString();
                textBox4.Text = WayPoints[listBox1.SelectedIndex].NextWaypoint.ToString();
            }
            else
            {
                button3.Enabled = false;
                textBox1.Enabled = false;
                textBox2.Enabled = false;
                textBox3.Enabled = false;
                textBox4.Enabled = false;
            }
        }

        private void textBox1_KeyUp(object sender, KeyEventArgs e)
        {
            if (listBox1.SelectedIndex < 0)
                return;
            try
            {
                int Index = listBox1.SelectedIndex;
                MainForm.WayPoints[] TempArray = WayPoints.ToArray();
                TempArray[listBox1.SelectedIndex].X = (float)Convert.ToDouble(textBox1.Text);
                WayPoints.Clear();
                for (int i = 0; i < TempArray.Length; i++)
                    WayPoints.Add(TempArray[i]);
                //LoadWaypoints(comboBox1.Items[Index].ToString());
                listBox1.Items.Clear();
                for (int i = 0; i < WayPoints.Count; i++)
                    listBox1.Items.Add("№ " + i.ToString() + " X= " + WayPoints[i].X + " Y= " + WayPoints[i].Y + " Z= " + WayPoints[i].Z + " Next= " + WayPoints[i].NextWaypoint);
                listBox1.SelectedIndex = Index;
            }
            catch (Exception E)
            {
                MessageBox.Show("Bad value! " + E.Message, "Farm helper error.", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        private void textBox2_KeyUp(object sender, KeyEventArgs e)
        {
            if (listBox1.SelectedIndex < 0)
                return;
            try
            {
                int Index = listBox1.SelectedIndex;
                MainForm.WayPoints[] TempArray = WayPoints.ToArray();
                TempArray[listBox1.SelectedIndex].Y = (float)Convert.ToDouble(textBox2.Text);
                WayPoints.Clear();
                for (int i = 0; i < TempArray.Length; i++)
                    WayPoints.Add(TempArray[i]);
                //LoadWaypoints(comboBox1.Items[Index].ToString());
                listBox1.Items.Clear();
                for (int i = 0; i < WayPoints.Count; i++)
                    listBox1.Items.Add("№ " + i.ToString() + " X= " + WayPoints[i].X + " Y= " + WayPoints[i].Y + " Z= " + WayPoints[i].Z + " Next= " + WayPoints[i].NextWaypoint);
                listBox1.SelectedIndex = Index;
            }
            catch (Exception E)
            {
                MessageBox.Show("Bad value! " + E.Message, "Farm helper error.", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        private void textBox3_KeyUp(object sender, KeyEventArgs e)
        {
            if (listBox1.SelectedIndex < 0)
                return;
            try
            {
                int Index = listBox1.SelectedIndex;
                MainForm.WayPoints[] TempArray = WayPoints.ToArray();
                TempArray[listBox1.SelectedIndex].Z = (float)Convert.ToDouble(textBox3.Text);
                WayPoints.Clear();
                for (int i = 0; i < TempArray.Length; i++)
                    WayPoints.Add(TempArray[i]);
                listBox1.Items.Clear();
                for (int i = 0; i < WayPoints.Count; i++)
                    listBox1.Items.Add("№ " + i.ToString() + " X= " + WayPoints[i].X + " Y= " + WayPoints[i].Y + " Z= " + WayPoints[i].Z + " Next= " + WayPoints[i].NextWaypoint);
                listBox1.SelectedIndex = Index;
            }
            catch (Exception E)
            {
                MessageBox.Show("Bad value! " + E.Message, "Farm helper error.", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        private void textBox4_KeyUp(object sender, KeyEventArgs e)
        {
            if (listBox1.SelectedIndex < 0)
                return;
            try
            {
                int Index = listBox1.SelectedIndex;
                MainForm.WayPoints[] TempArray = WayPoints.ToArray();
                TempArray[listBox1.SelectedIndex].NextWaypoint = Convert.ToInt16(textBox4.Text);
                WayPoints.Clear();
                for (int i = 0; i < TempArray.Length; i++)
                    WayPoints.Add(TempArray[i]);
                listBox1.Items.Clear();
                for (int i = 0; i < WayPoints.Count; i++)
                    listBox1.Items.Add("№ " + i.ToString() + " X= " + WayPoints[i].X + " Y= " + WayPoints[i].Y + " Z= " + WayPoints[i].Z + " Next= " + WayPoints[i].NextWaypoint);
                listBox1.SelectedIndex = Index;
            }
            catch (Exception E)
            {
                MessageBox.Show("Bad value! " + E.Message, "Farm helper error.", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        private void button3_Click(object sender, EventArgs e)
        {
            if (listBox1.SelectedIndex >= 0)
            {
                WayPoints.RemoveAt(listBox1.SelectedIndex);
                listBox1.Items.RemoveAt(listBox1.SelectedIndex);
            }
        }

        private void button2_Click(object sender, EventArgs e)
        {
            if (SelectedLoc == -1)
            {
                MessageBox.Show("Select location first!", "Farm helper info.", MessageBoxButtons.OK, MessageBoxIcon.Information);
                return;
            }
            MainForm.WayPoints New = new MainForm.WayPoints();
            wow.Read();
            New.X = wow.Objects[0].X;
            New.Y = wow.Objects[0].Y;
            New.Z = wow.Objects[0].Z;
            New.NextWaypoint = WayPoints.Count + 1;
            WayPoints.Add(New);
            FillListBox();
        }

        private void WPTimer_Tick(object sender, EventArgs e)
        {
            if (checkBox1.Checked == true)
            {
                MainForm.WayPoints New = new MainForm.WayPoints();
                MainForm.Coords Temp = new MainForm.Coords();
                wow.Read();
                if (wow.Objects.Count == 0)
                    return;
                Temp.X = wow.Objects[0].X;
                Temp.Y = wow.Objects[0].Y;
                Temp.Z = wow.Objects[0].Z;
                if (PCoord.ToArray().Length == 0)
                {
                    New.X = wow.Objects[0].X;
                    New.Y = wow.Objects[0].Y;
                    New.Z = wow.Objects[0].Z;
                    New.NextWaypoint = WayPoints.Count + 1;
                    WayPoints.Add(New);
                    FillListBox();
                    PCoord.Add(Temp);
                }
                else
                {
                    if (WowControl.CheckPoint(wow.Objects[0].X, wow.Objects[0].Y, wow.Objects[0].Z, PCoord[PCoord.ToArray().Length - 1].X, PCoord[PCoord.ToArray().Length - 1].Y, PCoord[PCoord.ToArray().Length - 1].Z) > WPRange)
                    {
                        New.X = wow.Objects[0].X;
                        New.Y = wow.Objects[0].Y;
                        New.Z = wow.Objects[0].Z;
                        New.NextWaypoint = WayPoints.Count + 1;
                        WayPoints.Add(New);
                        if (checkBox2.Checked == true)
                            SetNextWP();
                        FillListBox();
                        PCoord.Add(Temp);
                    }
                }
            }

        }
        private void FillListBox()
        {
            listBox1.Items.Clear();
            for (int i = 0; i < WayPoints.Count; i++)
                listBox1.Items.Add("№ " + i.ToString() + " X= " + WayPoints[i].X + " Y= " + WayPoints[i].Y + " Z= " + WayPoints[i].Z + " Next= " + WayPoints[i].NextWaypoint);
            listBox1.SelectedIndex = listBox1.Items.Count - 1;
        }

        private void button4_Click(object sender, EventArgs e)
        {
            WayPoints.Clear();
            listBox1.Items.Clear();
        }

        private void checkBox1_CheckedChanged(object sender, EventArgs e)
        {
            if (checkBox1.Checked == true)
            {
                checkBox2.Enabled = true;
                button2.Enabled = false;
                button3.Enabled = false;
                button4.Enabled = false;
                textBox5.Enabled = true;
                textBox1.Enabled = false;
                textBox2.Enabled = false;
                textBox3.Enabled = false;
                textBox4.Enabled = false;
                wow.Read();
                if (wow.Location != comboBox1.Items[SelectedLoc].ToString())
                {
                    MessageBox.Show("Your location: " + wow.Location + ", you are trying to write waypoints for: " + LoadedLocation, "Farm helper error.", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    return;
                }

            }
            else
            {
                checkBox2.Enabled = false;
                button2.Enabled = true;
                button3.Enabled = true;
                button4.Enabled = true;
                textBox5.Enabled = false;
                textBox1.Enabled = true;
                textBox2.Enabled = true;
                textBox3.Enabled = true;
                textBox4.Enabled = true;
            }
        }

        private void textBox5_TextChanged(object sender, EventArgs e)
        {
            try
            {
                WPRange = Convert.ToInt16(textBox5.Text);
            }
            catch (Exception E)
            {
                MessageBox.Show("Bad value! " + E.Message, "Farm helper error.", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }  
        }
        private void SetNextWP()
        {
            MainForm.WayPoints[] TempArray = WayPoints.ToArray();
            if (TempArray.Length > 1)
            {
                TempArray[WayPoints.Count - 2].NextWaypoint = WayPoints.Count - 1;
            }
            TempArray[WayPoints.Count - 1].NextWaypoint = 0;
            WayPoints.Clear();
            for (int i = 0; i < TempArray.Length; i++)
                WayPoints.Add(TempArray[i]);
        }

        private void checkBox2_CheckedChanged(object sender, EventArgs e)
        {
            MainForm.WayPoints[] TempArray = WayPoints.ToArray();
            for (int i = 0; i < TempArray.Length; i++)
            {
                TempArray[i].NextWaypoint = i + 1;
            }
            WayPoints.Clear();
            for (int i = 0; i < TempArray.Length; i++)
                WayPoints.Add(TempArray[i]);
            if (checkBox2.Checked == true)
            {
                SetNextWP();
            }
            FillListBox();
        }
        private void SaveWaypoints()
        {
            String Location = comboBox1.Items[SelectedLoc].ToString();
            String[] OldWaypoints = File.ReadAllLines(Application.StartupPath+"\\Data\\Waypoints.fh");
            List<String> CleanedWaypoints = new List<String>();
            for (int i = 0; i < OldWaypoints.Length; i++)
            {
                int Count = 0;
                int S1 = 0, S2 = 0, S3 = 0, S4 = 0, S5 = 0, S6 = 0, S7 = 0;
                for (int n = 0; n < OldWaypoints[i].Length; n++)
                {
                    if (OldWaypoints[i].Substring(n, 1) == " ")
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
                    if (OldWaypoints[i].Substring(n, 1) == ".")
                        S6 = n;
                    if (OldWaypoints[i].Substring(n, 1) == ";")
                        S7 = n;
                }
                if (Location != OldWaypoints[i].Substring(S6 + 1, S7 - S6 - 1))
                    CleanedWaypoints.Add(OldWaypoints[i]);
            }
            for (int i = 0; i < WayPoints.Count; i++)
                CleanedWaypoints.Add(WayPoints[i].X.ToString() + " " + WayPoints[i].Y.ToString() + " " + WayPoints[i].Z.ToString() + " " + WayPoints[i].NextWaypoint.ToString() + " ." + Location + ";" + i.ToString());
            String[] OutPut = CleanedWaypoints.ToArray();
            File.WriteAllLines(Application.StartupPath+"\\Data\\Waypoints.fh", OutPut);
        }

        private void button6_Click(object sender, EventArgs e)
        {
            if (SelectedLoc == -1)
            {
                MessageBox.Show("Select location first!", "Farm helper info.", MessageBoxButtons.OK, MessageBoxIcon.Information);
                return;
            }
            if (WayPoints.Count == 0)
            {
                if (MessageBox.Show("You are trying to save clear waypoints for: " + comboBox1.Items[SelectedLoc].ToString() + ". All waypoints for " + comboBox1.Items[SelectedLoc].ToString() + " would be deleted! \nContinue?", "Farm helper info.", MessageBoxButtons.OKCancel, MessageBoxIcon.Question) != DialogResult.OK)
                    return;
            }
            SaveWaypoints();
            this.Close();
        }

        private void button5_Click(object sender, EventArgs e)
        {
            String Location = wow.Location;
            for (int i = 0; i < comboBox1.Items.Count; i++)
            {
                if (Location == comboBox1.Items[i].ToString())
                {
                    MessageBox.Show("Location " + Location + " already exists!", "Farm helper info.", MessageBoxButtons.OK, MessageBoxIcon.Information);
                    return;
                }
            }
            wow.Read();
            comboBox1.Items.Add(wow.Location);
            comboBox1.SelectedIndex = comboBox1.Items.Count - 1;
        }

        private void comboBox1_SelectedIndexChanged(object sender, EventArgs e)
        {
            SelectedLoc = comboBox1.SelectedIndex;
        }
    }
}
