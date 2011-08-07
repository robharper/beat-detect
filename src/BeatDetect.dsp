# Microsoft Developer Studio Project File - Name="BeatDetect" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=BeatDetect - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "BeatDetect.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "BeatDetect.mak" CFG="BeatDetect - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "BeatDetect - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "BeatDetect - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "BeatDetect - Win32 Release"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x1009 /d "NDEBUG"
# ADD RSC /l 0x1009 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 winmm.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "BeatDetect - Win32 Debug"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x1009 /d "_DEBUG"
# ADD RSC /l 0x1009 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 winmm.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "BeatDetect - Win32 Release"
# Name "BeatDetect - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AudioStream.cpp
# End Source File
# Begin Source File

SOURCE=.\BDIOIStatCollector.cpp
# End Source File
# Begin Source File

SOURCE=.\BDNode.cpp
# End Source File
# Begin Source File

SOURCE=.\BDNodeControl.cpp
# End Source File
# Begin Source File

SOURCE=.\BDNodeCSN.cpp
# End Source File
# Begin Source File

SOURCE=.\BDNodeTimingNet.cpp
# End Source File
# Begin Source File

SOURCE=.\BDNodeVarSampler.cpp
# End Source File
# Begin Source File

SOURCE=.\BDOnsetDetect.cpp
# End Source File
# Begin Source File

SOURCE=.\BDOnsetStage.cpp
# End Source File
# Begin Source File

SOURCE=.\BDRealTimeStage.cpp
# End Source File
# Begin Source File

SOURCE=.\BDUtils.cpp
# End Source File
# Begin Source File

SOURCE=.\BeatDetect.cpp
# End Source File
# Begin Source File

SOURCE=.\BeatDetect.rc
# End Source File
# Begin Source File

SOURCE=.\BeatDetectDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\BeatDetectView.cpp
# End Source File
# Begin Source File

SOURCE=.\DataStream.cpp
# End Source File
# Begin Source File

SOURCE=.\DSP.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AudioStream.h
# End Source File
# Begin Source File

SOURCE=.\BDIOIStatCollector.h
# End Source File
# Begin Source File

SOURCE=.\BDNode.h
# End Source File
# Begin Source File

SOURCE=.\BDNodeControl.h
# End Source File
# Begin Source File

SOURCE=.\BDNodeCSN.h
# End Source File
# Begin Source File

SOURCE=.\BDNodeTimingNet.h
# End Source File
# Begin Source File

SOURCE=.\BDNodeVarSampler.h
# End Source File
# Begin Source File

SOURCE=.\BDOnsetDetect.h
# End Source File
# Begin Source File

SOURCE=.\BDOnsetStage.h
# End Source File
# Begin Source File

SOURCE=.\BDRealTimeStage.h
# End Source File
# Begin Source File

SOURCE=.\BDUtils.h
# End Source File
# Begin Source File

SOURCE=.\BeatDetect.h
# End Source File
# Begin Source File

SOURCE=.\BeatDetectDoc.h
# End Source File
# Begin Source File

SOURCE=.\BeatDetectView.h
# End Source File
# Begin Source File

SOURCE=.\DataStream.h
# End Source File
# Begin Source File

SOURCE=.\DSP.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\MFileWriter.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\BeatDetect.ico
# End Source File
# Begin Source File

SOURCE=.\res\BeatDetect.rc2
# End Source File
# Begin Source File

SOURCE=.\res\BeatDetectDoc.ico
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# End Group
# End Target
# End Project
