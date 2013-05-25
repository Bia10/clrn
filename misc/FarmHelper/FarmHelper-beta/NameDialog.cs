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
    public partial class NameDialog : Form
    {
        public NameDialog()
        {
            InitializeComponent();
            textBox1.Select();
        }
        public String CfgName;

        private void button1_Click(object sender, EventArgs e)
        {
            CfgName = textBox1.Text;
            this.Close();
        }

        private void button2_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        private void textBox1_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyValue == 13)
            {
                CfgName = textBox1.Text; 
                this.Close();
            }
        }
    }
}
