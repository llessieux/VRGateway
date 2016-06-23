"# VRGateway" 

VRGateway.

A simple framework for displaying various applications in HTC Vive using the OpenVR framework.

Test applications (Selection with command line only for now):

* Taiko VR (-taiko_vr on the command line). Expects a custom version of the PPSSPP emulator running Taiko no Tatsujin.
Will display the game on a virtual screen and display a drum with sticks replacing the PSP controls.

* VLC VR: (-vlc_vr with optional -vlc_stereo_vr option)
Display the screen of VLC in VR.
With the stereo option it will show a side by side 3D Movie in 3D in VR :)

Inspired from 
https://github.com/JamesBear/directx11_hellovr

And using the Windows Desktop Duplication API.
Requires Windows 8.1 x64 min.
64 bit release only compiling.
Runs on Windows 10 x64 too.

Requires HTC VIVE.
Requires Custom PPSSPP Emulator and "Taiko no Tatsujin" Game for the Taiko hack.
	Clone the PPSSPP source, copy the RawInput.cpp file from PPSSPPPatch folder in my project into the PPSSPP Windows source directory.
	Compile that version.
	Update the TAIKOVR.INI to set the path to the installed PPSSPP folder. (Used to read key config).
	
Requires VLC for the VLC Hack.