using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Drawing;

namespace AutoClick
{
    public class CCapture
    {	
        
        //This structure shall be used to keep the size of the screen.
        public struct SIZE
        {
            public int cx;
            public int cy;
        }

        public static Bitmap GetBmpFromWindowHandle()
        {
            //In size variable we shall keep the size of the screen.
            SIZE size;

            //Variable to keep the handle to bitmap.
            IntPtr hBitmap;

            //Here we get the handle to the desktop device context.
            IntPtr hDC = Win32.GetWindowDC(Win32.GetDesktopWindow());

            //Here we make a compatible device context in memory for screen device context.
            IntPtr hMemDC = Win32.CreateCompatibleDC(hDC);

            //We pass SM_CXSCREEN constant to GetSystemMetrics to get the X coordinates of screen.
            size.cx = Win32.GetSystemMetrics(Win32.SM_CXSCREEN);

            //We pass SM_CYSCREEN constant to GetSystemMetrics to get the Y coordinates of screen.
            size.cy = Win32.GetSystemMetrics(Win32.SM_CYSCREEN);

            //We create a compatible bitmap of screen size using screen device context.
            hBitmap = Win32.CreateCompatibleBitmap(hDC, size.cx, size.cy);

            //As hBitmap is IntPtr we can not check it against null. For this purspose IntPtr.Zero is used.
            if (hBitmap != IntPtr.Zero)
            {
                //Here we select the compatible bitmap in memeory device context and keeps the refrence to Old bitmap.
                IntPtr hOld = (IntPtr)Win32.SelectObject(hMemDC, hBitmap);
                //We copy the Bitmap to the memory device context.
                Win32.BitBlt(hMemDC, 0, 0, size.cx, size.cy, hDC, 0, 0, Win32.SRCCOPY);
                //We select the old bitmap back to the memory device context.
                Win32.SelectObject(hMemDC, hOld);
                //We delete the memory device context.
                Win32.DeleteDC(hMemDC);
                //We release the screen device context.
                Win32.ReleaseDC(Win32.GetDesktopWindow(), hDC);
                //Image is created by Image bitmap handle and stored in local variable.
                Bitmap bmp = System.Drawing.Image.FromHbitmap(hBitmap);
                //Release the memory to avoid memory leaks.
                Win32.DeleteObject(hBitmap);
                //This statement runs the garbage collector manually.
                GC.Collect();
                //Return the bitmap 
                return bmp;
            }

            //If hBitmap is null retunrn null.
            return null;
        }
    }
}
