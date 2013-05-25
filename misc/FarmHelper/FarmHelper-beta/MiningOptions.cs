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
    public partial class MiningOptions : Form
    {
        public MiningOptions()
        {
            InitializeComponent();
            comboBox1.SelectedIndex = WowControl.FarmMount;
            checkBox1.Checked = WowControl.LootMobs;
        }
        private void listBox1_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (listBox1.SelectedIndex != -1)
            {
                button2.Enabled = true;
                textBox1.Text = WowControl.Goods[listBox1.SelectedIndex];
            }
            else button2.Enabled = false;
        }

        private void button1_Click(object sender, EventArgs e)
        {
            Add();
        }
        private void Add()
        {
            if (textBox1.Text != "")
            {
                WowControl.Goods.Add(textBox1.Text);
                listBox1.Items.Add(textBox1.Text);
            }
            else
            {
                MessageBox.Show("Enter name! ", "Farm helper error.", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
            textBox1.Text = "";
        }
        private void button2_Click(object sender, EventArgs e)
        {
            if (listBox1.SelectedIndex != -1)
            {
                WowControl.Goods.RemoveAt(listBox1.SelectedIndex);
                listBox1.Items.RemoveAt(listBox1.SelectedIndex);
            }
        }

        private void textBox1_KeyPress(object sender, KeyPressEventArgs e)
        {
            if (e.KeyChar == 13)
                Add();
        }


        private void textBox1_TextChanged(object sender, EventArgs e)
        {
            if (listBox1.SelectedIndex != -1)
            {
                int Index = listBox1.SelectedIndex;
                String[] TempArray = WowControl.Goods.ToArray();
                TempArray[listBox1.SelectedIndex] = textBox1.Text;
                WowControl.Goods.Clear();
                for (int i = 0; i < TempArray.Length; i++)
                    WowControl.Goods.Add(TempArray[i]);
                listBox1.Items.Clear();
                for (int i = 0; i < WowControl.Goods.Count; i++)
                    listBox1.Items.Add(WowControl.Goods[i]);
                listBox1.SelectedIndex = Index;
            }
        }

        private void button3_Click(object sender, EventArgs e)
        {
            WowControl.SaveGoods();
            this.Close();
        }
        private void FillListBox()
        {
            listBox1.Items.Clear();
            for (int i = 0; i < WowControl.Goods.Count; i++)
                listBox1.Items.Add(WowControl.Goods[i]);
        }


        private void LoadGoods(String FileName)
        {
            String[] Loaded = File.ReadAllLines(FileName);
            for (int i = 0; i < Loaded.Length; i++)
                WowControl.Goods.Add(Loaded[i]);
        }
        private void MiningOptions_Load(object sender, EventArgs e)
        {
            FillListBox();
        }

        private void button4_Click(object sender, EventArgs e)
        {
            String[] LoadedMines = File.ReadAllLines(Application.StartupPath + "\\Data\\Mines.fh");
            for (int i = 0; i < LoadedMines.Length; i++)
                WowControl.Goods.Add(LoadedMines[i]);
            FillListBox();
        }

        private void button5_Click(object sender, EventArgs e)
        {
            String[] LoadedHerbs = File.ReadAllLines(Application.StartupPath + "\\Data\\Herbs.fh");
            for (int i = 0; i < LoadedHerbs.Length; i++)
                WowControl.Goods.Add(LoadedHerbs[i]);
            FillListBox();
        }

        private void button7_Click(object sender, EventArgs e)
        {
            String[] LoadedMines = File.ReadAllLines(Application.StartupPath + "\\Data\\Mines.fh");
            for (int i = 0; i < LoadedMines.Length; i++)
                for (int n = 0; n < WowControl.Goods.Count; n++)
                    if (LoadedMines[i] == WowControl.Goods[n])
                        WowControl.Goods.RemoveAt(n);
            FillListBox();
        }

        private void button6_Click(object sender, EventArgs e)
        {
            String[] LoadedHerbs = File.ReadAllLines(Application.StartupPath + "\\Data\\Herbs.fh");
            for (int i = 0; i < LoadedHerbs.Length; i++)
                for (int n = 0; n < WowControl.Goods.Count; n++)
                    if (LoadedHerbs[i] == WowControl.Goods[n])
                        WowControl.Goods.RemoveAt(n);
            FillListBox();
        }

        private void button8_Click(object sender, EventArgs e)
        {
            String[] LoadedGases = File.ReadAllLines(Application.StartupPath + "\\Data\\Gases.fh");
            for (int i = 0; i < LoadedGases.Length; i++)
                WowControl.Goods.Add(LoadedGases[i]);
            FillListBox();
        }

        private void button9_Click(object sender, EventArgs e)
        {
            String[] LoadedGases = File.ReadAllLines(Application.StartupPath + "\\Data\\Gases.fh");
            for (int i = 0; i < LoadedGases.Length; i++)
                for (int n = 0; n < WowControl.Goods.Count; n++)
                    if (LoadedGases[i] == WowControl.Goods[n])
                        WowControl.Goods.RemoveAt(n);
            FillListBox();
        }

        private void comboBox1_SelectedIndexChanged(object sender, EventArgs e)
        {
            WowControl.FarmMount = comboBox1.SelectedIndex;
        }

        private void checkBox1_CheckedChanged(object sender, EventArgs e)
        {
            if (checkBox1.Checked == true)
                WowControl.LootMobs = true;
            else WowControl.LootMobs = false;
        }
    }
}
