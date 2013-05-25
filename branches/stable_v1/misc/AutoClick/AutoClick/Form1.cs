using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Diagnostics;
using System.Drawing;

namespace AutoClick
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
            m_nRedToFind = 0;
            m_nGreenToFind = 0;
            m_nBlueToFind = 0;
            m_nRange = 1;
            m_SearchRect = new Win32.RECT();
        }

        //! Red color to find
        byte m_nRedToFind;

        //! Green color to find
        byte m_nGreenToFind;

        //! Blue color to find
        byte m_nBlueToFind;

        //! Search range
        int m_nRange;

        //! Coords of rectangle to search
        Win32.RECT m_SearchRect;

        int m_nX;
        int m_nY;


        void Click(int x, int y)
        {
            Win32.mouse_event(Win32.MOUSEEVENTF_MOVE, x, y, 0, IntPtr.Zero);
            Win32.mouse_event(Win32.MOUSEEVENTF_LEFTDOWN, x, y, 0, IntPtr.Zero);
            Win32.mouse_event(Win32.MOUSEEVENTF_LEFTUP, x, y, 0, IntPtr.Zero);
        }

        private void button1_Click(object sender, EventArgs e)
        {
            if (timer1.Enabled)
            {
                button1.Text = "Stop";
                timer1.Enabled = false;
            }
            else
            {
                button1.Text = "Start";
                timer1.Enabled = true;
            }
        }

        private void textBox1_TextChanged(object sender, EventArgs e)
        {
            try
            {
                m_nRedToFind = Convert.ToByte(textBox1.Text);
            }
            catch (Exception Excep)
            {
                listBox1.Items.Add(Excep.Message);
            }
        }

        private void textBox2_TextChanged(object sender, EventArgs e)
        {
            try
            {
                m_nGreenToFind = Convert.ToByte(textBox1.Text);
            }
            catch (Exception Excep)
            {
                listBox1.Items.Add(Excep.Message);
            }
        }

        private void textBox3_TextChanged(object sender, EventArgs e)
        {
            try
            {
                m_nBlueToFind = Convert.ToByte(textBox1.Text);
            }
            catch (Exception Excep)
            {
                listBox1.Items.Add(Excep.Message);
            }
        }

        private void textBox4_TextChanged(object sender, EventArgs e)
        {
            try
            {
                timer1.Interval = Convert.ToInt32(textBox4.Text);
            }
            catch (Exception Excep)
            {
                listBox1.Items.Add(Excep.Message);
            }
        }

        private void textBox5_TextChanged(object sender, EventArgs e)
        {
            try
            {
                m_nRange = Convert.ToInt32(textBox5.Text);
            }
            catch (Exception Excep)
            {
                listBox1.Items.Add(Excep.Message);
            }
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            /*
            Bitmap Window = CCapture.GetBmpFromWindowHandle();

            Window.Save("C:\\1.bmp");

            Color c = new Color();

            Color[,] Pixels = new Color[Window.Size.Width, Window.Size.Height];

            for (int y = 0; y < Window.Size.Height; y += m_nRange)
            {
                for (int x = 0; x < Window.Size.Width; x += m_nRange)
                {
                    Pixels[x, y] = Window.GetPixel(x, y);
                    byte Red = Pixels[x, y].R;
                    byte Green = Pixels[x, y].G;
                    byte Blue = Pixels[x, y].B;
                    if (Red == m_nRedToFind && Green == m_nGreenToFind && Blue == m_nBlueToFind)
                    {
                        Click(x + m_SearchRect.left, y + m_SearchRect.top);
                    }

                
                }
            }*/
            Click(m_nX, m_nY);

        }

        private void timer2_Tick(object sender, EventArgs e)
        {
            textBox6.Text = System.Windows.Forms.Cursor.Position.X.ToString();
            textBox7.Text = System.Windows.Forms.Cursor.Position.Y.ToString();
            Win32.RECT rect = new Win32.RECT();
            m_nX = System.Windows.Forms.Cursor.Position.X;
            m_nY = System.Windows.Forms.Cursor.Position.Y;

            /*
            Bitmap Window = CCapture.GetBmpFromWindowHandle();
            Color Point = Window.GetPixel(x, y);
            textBox1.Text = Point.R.ToString();
            textBox2.Text = Point.G.ToString();
            textBox3.Text = Point.B.ToString();*/
        }

        private void textBox8_TextChanged(object sender, EventArgs e)
        {
            try
            {
                m_SearchRect.left = Convert.ToInt32(textBox8.Text);
                m_SearchRect.top = Convert.ToInt32(textBox9.Text);
                m_SearchRect.right = Convert.ToInt32(textBox10.Text);
                m_SearchRect.bottom = Convert.ToInt32(textBox11.Text);
            }
            catch (Exception Excep)
            {
                listBox1.Items.Add(Excep.Message);
            }
        }

        private void textBox9_TextChanged(object sender, EventArgs e)
        {
            try
            {
                m_SearchRect.left = Convert.ToInt32(textBox8.Text);
                m_SearchRect.top = Convert.ToInt32(textBox9.Text);
                m_SearchRect.right = Convert.ToInt32(textBox10.Text);
                m_SearchRect.bottom = Convert.ToInt32(textBox11.Text);
            }
            catch (Exception Excep)
            {
                listBox1.Items.Add(Excep.Message);
            }
        }

        private void textBox10_TextChanged(object sender, EventArgs e)
        {
            try
            {
                m_SearchRect.left = Convert.ToInt32(textBox8.Text);
                m_SearchRect.top = Convert.ToInt32(textBox9.Text);
                m_SearchRect.right = Convert.ToInt32(textBox10.Text);
                m_SearchRect.bottom = Convert.ToInt32(textBox11.Text);
            }
            catch (Exception Excep)
            {
                listBox1.Items.Add(Excep.Message);
            }
        }

        private void textBox11_TextChanged(object sender, EventArgs e)
        {
            try
            {
                m_SearchRect.left = Convert.ToInt32(textBox8.Text);
                m_SearchRect.top = Convert.ToInt32(textBox9.Text);
                m_SearchRect.right = Convert.ToInt32(textBox10.Text);
                m_SearchRect.bottom = Convert.ToInt32(textBox11.Text);
            }
            catch (Exception Excep)
            {
                listBox1.Items.Add(Excep.Message);
            }
        }

    }
}
