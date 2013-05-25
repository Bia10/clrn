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
    public partial class FarmHonorOptions : Form
    {
        public FarmHonorOptions()
        {
            InitializeComponent();
            checkBox1.Checked = WowControl.StayWithGroup;
            textBox1.Text = WowControl.FriendCheckRange.ToString();
            textBox2.Text = WowControl.GroupRange.ToString();
            textBox3.Text = WowControl.RangeFromPlayerToGroup.ToString();
            textBox4.Text = WowControl.MinPlayersInGroup.ToString();
            textBox5.Text = WowControl.EnemyCheckRange.ToString();
            trackBar1.Value = WowControl.HealDDPriority;
        }

        private void checkBox1_CheckedChanged(object sender, EventArgs e)
        {
            if (checkBox1.Checked == true)
            {
                WowControl.StayWithGroup = true;
                textBox2.Enabled = true;
                textBox3.Enabled = true;
                textBox4.Enabled = true;
            }
            else
            {
                WowControl.StayWithGroup = false;
                textBox2.Enabled = false;
                textBox3.Enabled = false;
                textBox4.Enabled = false;
            }
        }

        private void textBox1_TextChanged(object sender, EventArgs e)
        {
            try
            {
                WowControl.FriendCheckRange = Convert.ToInt32(textBox1.Text);
            }
            catch (Exception E) { WowControl.UpdateStatus(E.Message); }
        }

        private void textBox2_TextChanged(object sender, EventArgs e)
        {
            try
            {
                WowControl.GroupRange = Convert.ToInt32(textBox2.Text);
            }
            catch (Exception E) { WowControl.UpdateStatus(E.Message); }

        }

        private void textBox3_TextChanged(object sender, EventArgs e)
        {
            try
            {
                WowControl.RangeFromPlayerToGroup = Convert.ToInt32(textBox3.Text);
            }
            catch (Exception E) { WowControl.UpdateStatus(E.Message); }

        }

        private void textBox4_TextChanged(object sender, EventArgs e)
        {
            try
            {
                WowControl.MinPlayersInGroup = Convert.ToInt32(textBox4.Text);
            }
            catch (Exception E) { WowControl.UpdateStatus(E.Message); }

        }

        private void textBox5_TextChanged(object sender, EventArgs e)
        {
            try
            {
                WowControl.EnemyCheckRange = Convert.ToInt32(textBox5.Text);
            }
            catch (Exception E) { WowControl.UpdateStatus(E.Message); }
        }

        private void trackBar1_Scroll(object sender, EventArgs e)
        {
            WowControl.HealDDPriority = trackBar1.Value;
        }

    }
}
