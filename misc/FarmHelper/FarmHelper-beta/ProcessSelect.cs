using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Diagnostics;

namespace FarmHelper_beta
{
    public partial class ProcessSelect : Form
    {
        public Process[] AllWow;
        public int PId;
        public ProcessSelect()
        {
            InitializeComponent();
            comboBox1.Items.Clear();
            AllWow = Process.GetProcessesByName("wow");
            for (int i = 0; i < AllWow.Length; i++)
                comboBox1.Items.Add("Wow: " + AllWow[i].Id.ToString());
        }

        private void comboBox1_SelectedIndexChanged(object sender, EventArgs e)
        {
            PId = AllWow[comboBox1.SelectedIndex].Id;
            this.Close();
        }
    }
}
