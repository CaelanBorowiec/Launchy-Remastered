# Launchy 2.0 Plugin API Documentation

### Josh Karlin

[Setting up the build environment](#_Setting_up_the)

[Building your first plugin](#_Building_your_first)

[Editing the plugin](#_Editing_the_plugin)

[Distributing your plugin](#_Distributing_your_plugin)

[Skeleton Plugin](http://www.launchy.net/api2.0/plugin.zip)

[Doxygen Documentation](doxydoc/plugin__interface_8h.html)

### Introduction

By itself, Launchy can only index regular windows files such as mp3's, documents, links etc. There are infinitely more things that the simple Launchy interface could be used for, e.g. as a calculator, browser launcher, directory walker, etc.

Launchy's interface is broken down into a user input window and a program output window. As the user enters input, the items are matched against Launchy's index of objects and the best matching object's display name is displayed in the output window. If the user presses the enter key then the object is launched by the object's owner. If the user presses the TAB key then the owner is asked to provide a new set of objects.

In this document, I describe how plugins can be developed to add new types of objects to the catalog and take control of the execution of existing ones.

<a id="_Setting_up_the"></a>

### Setting up the build environment

For this setup, we require Visual Studio (preferably 2005 as that is what I use) and QT 4.3.3.  I will assume that you already have Visual Studio installed but not the QT developer environment.

*Installing QT*

Download the open source windows version of QT version here: [QT download](http://trolltech.com/developer/downloads/qt/windows)  Don’t get the mingw build, you just want the source zip file.  I recommend finding version 4.3.3 as that is compatible with Launchy 2.0+  All Launchy’s 2.1+ are compatible with 4.4.0+.

Unzip the file and rename the directory to 4.x.x  (replace the x’s with your downloaded version number).  Then place 4.x.x into c:\\qt\\ so that you have a c:\\qt\\4.x.x\\ directory.

Add 2 environment variables (under control panel->system->advanced->environment variables)

1. QMAKESPEC = win32-msvc2005
2. QTDIR = c:\\qt\\4.x.x

From Launchy or your start menu, run the “Visual Studio 2005 Command Prompt”

From the command prompt type the following (this takes awhile to finish):

cd c:\\qt\\4.x.x

configure –debug-and-release –no-stl –no-qt3support

nmake

Okay, now QT is built.  Note in c:\\qt\\4.x.x\\bin there are a bunch of useful utilities.  The dll’s that comprise the QT library are in there as well as the excellent QT documentation in a program called “assistant.exe”.

<a id="_Building_your_first"></a>

### Building your first plugin

Download the plugin skeleton [here](http://www.launchy.net/api2.0/plugin.zip).  This includes a blank plugin, all of the necessary header files, and a visual studio project designed for compilation with QT’s qmake.  Note that you need to edit ‘myplugin.pro’ when adding or removing files from your project.

Please download and extract the plugin skeleton.  Ensure that your %QTDIR% variable is properly set and then restart visual studio.  Now open the plugin project.  It should build with some compile warnings (due to empty functions).  Your compiled Launchy plugin is in debug\\myplugin.dll

<a id="_Editing_the_plugin"></a>

### Editing the plugin

Launchy deals solely with the plugin’s “msg” function.  Msg then dispatches incoming events to appropriate event handlers.  The “msg” function should return true if the event was handled, and false otherwise.

The first thing to change about your new plugin is its name.  In “myplugin.h” there is a line: #define PLUGIN\_NAME "MyPlugin".  You should replace “MyPlugin” with the name that you desire.  Next, you should proceed to fill out any of the necessary event functions that you require.  To do this, you need example code and documentation about the specifics events.

For example plugins, please refer to the source code of the plugins that are shipped with Launchy.  The code can be browsed via the svn repository [here](http://launchy.svn.sourceforge.net/viewvc/launchy/trunk/Launchy_QT/plugins/).  When browsing the code, don’t click on the file name, click on the revision number.  That will show you all of the file’s contents for the given revision.

For event documentation, please refer to the doxygen [documentation](doxydoc/plugin__interface_8h.html).

<a id="_Distributing_your_plugin"></a>

### Running your plugin

First, make sure your plugin has an icon!  This icon should go into Launchy\\plugins\\icons\\.  Next, make sure your code points to this icon in your “getIcon” function.  Finally, copy your plugin’s compiled .dll file to the Launchy\\plugins directory and restart Launchy.  Good luck!
