using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using Microsoft.Win32.SafeHandles;
using System.Runtime.InteropServices;
using System.IO;
using System.Diagnostics;
using System.Net;
using System.Net.Sockets;
using System.Threading;

namespace FarmHelper
{
    public partial class Form1 : Form
    {
        const string sProcessName = "Wow";
        string sCommnad;
        int nProcsessId;
        CSocketMessanger SocketMessanger;
        public Form1()
        {
            InitializeComponent();
            nProcsessId = Process.GetProcessesByName(sProcessName)[0].Id;
            SocketMessanger = new CSocketMessanger("localhost", 27015, nProcsessId);
           
        }
        private void button1_Click(object sender, EventArgs e)
        {
            Win32.LoadDll(nProcsessId);
        }

        private void button2_Click(object sender, EventArgs e)
        {
            SocketMessanger.Send("Shutdown");
            Thread.Sleep(1000);
            Win32.UnloadDll(nProcsessId);
        }

        private void button3_Click(object sender, EventArgs e)
        {
            long nPid = Convert.ToInt32(textBox1.Text);
        }

        private void button4_Click(object sender, EventArgs e)
        {
            int nId = Process.GetProcessesByName(sProcessName)[0].Id;

        }

        private void button5_Click(object sender, EventArgs e)
        {

            //  CBot.Execute(nPid, ref one);
        }

        private void button6_Click(object sender, EventArgs e)
        {
            nProcsessId = Process.GetProcessesByName(sProcessName)[0].Id;
           // CSocketMessanger SocketMessanger = new CSocketMessanger("localhost", 27015, nProcsessId);
            PipeMessage Answer = SocketMessanger.Send(sCommnad);
            string sAnswer = new string(Answer.Message);
            listBox1.Items.Add(sCommnad + " => " + sAnswer);
        }

        private void textBox2_TextChanged(object sender, EventArgs e)
        {
            sCommnad = textBox2.Text;
        }

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {

        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            //SocketMessanger.Send("Get");
        }
    }
}
