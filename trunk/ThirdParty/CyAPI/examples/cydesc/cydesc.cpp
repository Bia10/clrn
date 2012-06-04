
/*Summary
The application cydesc is used to open the device with cypress GUID and get the device descriptor
*/



#include <windows.h>

#include <stdio.h>
#include "cyapi.h"
#include <conio.h>
//#include <iostream.h>

void main()
{
    CCyUSBDevice *USBDevice;
    USB_DEVICE_DESCRIPTOR descr;



    USBDevice = new CCyUSBDevice(NULL);   // Create an instance of CCyUSBDevice

    printf("device count = %d \n",USBDevice->DeviceCount());

    for (int i=0; i < USBDevice->DeviceCount(); i++)
    {
        if (USBDevice->Open(i))
        {
            USBDevice->GetDeviceDescriptor(&descr);

            printf("bLength \t\t 0x%02x\n",descr.bLength);
            printf("bDescriptorType \t 0x%02x\n",descr.bDescriptorType);
            printf("bcdUSB \t\t\t 0x%04x\n",descr.bcdUSB);
            printf("bDeviceClass \t\t 0x%02x\n",descr.bDeviceClass);
            printf("bDeviceSubClass \t 0x%02x\n",descr.bDeviceSubClass);
            printf("bDeviceProtocol \t 0x%02x\n",descr.bDeviceProtocol);
            printf("bMaxPacketSize0 \t 0x%02x\n",descr.bMaxPacketSize0);
            printf("idVendor \t\t 0x%04x\n",descr.idVendor);
            printf("idProduct \t\t 0x%04x\n",descr.idProduct);
            printf("bcdDevice \t\t 0x%04x\n",descr.bcdDevice);
            printf("iManufacturer \t\t 0x%02x\n",descr.iManufacturer);
            printf("iProduct \t\t 0x%02x\n",descr.iProduct);
            printf("iSerialNumber \t\t 0x%02x\n",descr.iSerialNumber);
            printf("bNumConfigurations \t 0x%02x\n\n",descr.bNumConfigurations);

            USBDevice->Close();
        }
        else
        {
            printf("unable to open device\n");
        }
    }

    //USBDevice->GetDeviceDescriptor(&descr);

    USBDevice->Close();
	_getch();

}