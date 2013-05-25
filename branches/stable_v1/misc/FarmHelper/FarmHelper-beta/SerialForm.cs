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
    public partial class SerialForm : Form
    {
        public SerialForm()
        {
            InitializeComponent();
            textBox1.Text = Defender.GetSerial();
            try { richTextBox1.Text = File.ReadAllText(Application.StartupPath + "\\Data\\license.fh"); }
            catch (Exception) { }
        }

        private void richTextBox1_TextChanged(object sender, EventArgs e)
        {
            if (Defender.CheckLicense(richTextBox1.Text) == true)
            {
                File.WriteAllText(Application.StartupPath + "\\Data\\license.fh", richTextBox1.Text);
                MessageBox.Show("License key saved!", "Farm helper license.", MessageBoxButtons.OK, MessageBoxIcon.Information);
                this.Close();
            }
            else
                MessageBox.Show("Invalid license key!", "Farm helper license.", MessageBoxButtons.OK, MessageBoxIcon.Error);
        }
    }
}
