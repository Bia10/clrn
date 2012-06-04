Streamer example support Visual studio 2008 and 2010. 

Please note that the source code for the Visual Studio 2008 and 2010 projects is not common due to known issue in the GUI resource file(the icon file in VS2010 raise exception so we have commented code specific to the Icon drawing).

If you have Visual studio 2008 installed on your system , please use solution : \Streamer\Vs2008_sln\StreamerVS2008.sln to open the project. This solution will load the Streamer.vcproj project file. The source file for this project is available under the \Streamer  directory.

If you have Visual studio 2010 installed on your system , please use solution : \Streamer\Vs2010_sln\StreamerVS2010.sln to open the project. This solution will load the Streamer.vcxproj project file.  The source file for Visual Studio 2010 is available under the \Streamer\Vs2010_sln

========================================================================
    APPLICATION : Streams Project Overview
========================================================================



This file contains a summary of what you will find in each of the files that
make up your Streams application which uses cypress C++ library CyAPI.lib.

Streamer.vcproj
    This is the main project file for VC++ projects generated using an Application Wizard. 
    It contains information about the version of Visual C++ that generated the file, and 
    information about the platforms, configurations, and project features selected with the
    Application Wizard.

Form1.cpp
    This is the main application source file.
    Contains the code to display the form.

Form1.h
    Contains the implementation of your form class and InitializeComponent() function.

AssemblyInfo.cpp
    Contains custom attributes for modifying assembly metadata.

/////////////////////////////////////////////////////////////////////////////
Other standard files:

StdAfx.h, StdAfx.cpp
    These files are used to build a precompiled header (PCH) file
    named Win32.pch and a precompiled types file named StdAfx.obj.

/////////////////////////////////////////////////////////////////////////////
