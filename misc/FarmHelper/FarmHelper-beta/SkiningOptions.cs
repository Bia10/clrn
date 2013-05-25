using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace FarmHelper_beta
{
    public partial class SkiningOptions : Form
    {
        public SkiningOptions()
        {
            InitializeComponent();
            checkBox1.Checked = WowControl.SkiningKillAll;
            textBox1.Text = WowControl.SkiningRange.ToString();
            checkBox2.Checked = WowControl.SkinAll;
        }

        private void checkBox1_CheckedChanged(object sender, EventArgs e)
        {
            if (checkBox1.Checked == true)
            {
                textBox1.Enabled = true;
                WowControl.SkiningKillAll = true;
            }
            else
            {
                textBox1.Enabled = false;
                WowControl.SkiningKillAll = false;
            }
        }

        private void textBox1_TextChanged(object sender, EventArgs e)
        {
            try
            {
                WowControl.SkiningRange = Convert.ToInt32(textBox1.Text);
            }
            catch (Exception E) { WowControl.UpdateStatus(textBox1.Text+". "+ E.Message); }
        }

        private void button1_Click(object sender, EventArgs e)
        {
            this.Close();
        }


        private void checkBox2_CheckedChanged(object sender, EventArgs e)
        {
            if (checkBox1.Checked == true)
                WowControl.SkinAll = true;
            else WowControl.SkinAll = false;
        }
    }
}
