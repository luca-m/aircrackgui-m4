# Aircrack-GUI-M4 Version #
# The ultimate GUI for aircrack-ng and wireless tools #

**To execute the program, just decompress the packet, and execute start.sh FROM THE PROGRAM FOLDER (i.e: open terminal, cd PROGRAM\_FOLDER, sudo ./aircrack-GUI-Start.sh)**

# NECESSARY DEPENDENCIES FOR EXECUTE #

DO A SYMBOLIC LINK from libssl0.9.8 to libssl1.0.0 and do the same for libcrypto.

or


---

**apt-get install libssl0.9.8:i386**

---



# LATEST VERSION: #
# [{-]1.0.0 BETA 2  -ULTIMATE EDITION-[-}] #

```
[*CHANGELOG*]
1.0.0 BETA 2 -ULTIMATE EDITION-
*******************************************
[thx to USUARIONUEVO for feed-back) (http://foro.seguridadwireless.net/)
* Fix in deauthentication process
* Fix in arp replay status
* New structure of subversion. SOURCE CODE 1.0.0 UPLOADED

1.0.0 BETA 1 -ULTIMATE EDITION-
*******************************************
* 50% of code TOTALLY REBUILD. EXTREME OPTIMIZATION
* Reaver attack implemented (WPA-WPS)
* All WEP attacks have been rebuild
* GUI Airodump totally rebuild. You can view in real time STATUS of ALL ATTACKS
* Monitoring in WPA mode enabled
* New log system: thread that handles all information for debug. Stored in log folder
* Option to view log in real time in a different window
* Libs updated to 4.8.0
* Around 10-12 bugs Fixed
* Historical network STORES SESSIONS OF REAVER.
* Rebuild update system
* Reinjection arp x3 ipv6 (experimental)
* Forced authentication
* Macchanger big modifications
* Main binaries included in packet (aircrack, reaver, libs)
* Its necessary that you INSTALL IN YOUR SYSTEM aircrack-ng from your repository.

0.7.0 Final WEP Edition
*******************************************
[spa]
Es *NECESARIO* descargar el paquete completo
desde la pagina de googlecode.
http://code.google.com/p/aircrackgui-m4/
Si actualizas a esta version desde una previa,
llegaras a la version 0.6.8 RC, no a la 0.7.0 Final.
[eng]
Its *NECESSARY* to download complete
packet from googlecode page.
http://code.google.com/p/aircrackgui-m4/
If you update from previous version,
you will get version 0.6.8 RC, not 0.7.0 Final.
*******************************************
* Libs Updated
* Segmentation fault fixed in some systems (due to updating libs)
* DEBUG MODE COMPLETELY BUILD! (option -d when launch the program)
* Export keys added in historicalNetwork
* Added start script to project svn
* Modified start script. Now is more powerfull
* Some optimizations

0.6.8 RC
***********************************************************
IMPORTANT: STOP ALL PROCESSES OF AIRCRACK-GUI-M4 BEFORE UPDATING!!
IF you update the program while any process is running, the update
could corrupt any bin
***********************************************************
* Super Bug Fixed: Now, the station associated SELECTED
is wich do the attack (before, any station associated could)
* Bug Fixed when closing the program.
* Now attacks could inform to status bar when succesfull

0.6.7 v4 RC
***********************************************************
IMPORTANT: STOP ALL PROCESSES OF AIRCRACK-GUI-M4 BEFORE UPDATING!!
IF you update the program while any process is running, the update
could corrupt any bin
***********************************************************
* Another fucking critical bug in NULL pointer in airodump (deauth)

0.6.7 v3 RC
***********************************************************
IMPORTANT: STOP ALL PROCESSES OF AIRCRACK-GUI-M4 BEFORE UPDATING!!
IF you update the program while any process is running, the update
could corrupt any bin
***********************************************************
* Fix Null Pointer In infoConnectionEssid from airodump [thx to sanson]

0.6.7 v2 RC
***********************************************************
IMPORTANT: STOP ALL PROCESSES OF AIRCRACK-GUI-M4 BEFORE UPDATING!!
IF you update the program while any process is running, the update
could corrupt any bin
***********************************************************
* Fix in number of stations associated while capturing

0.6.7 RC
*************************************
IMPORTANT: STOP ALL PROCESSES OF AIRCRACK-GUI-M4 BEFORE UPDATING!!
IF you update the program while any process is running, the update
could corrupt any bin
*************************************
* While monitoring, now all stations associated are selected
* Added progressBar to chopChop Attack
* Added warning if we launch one process that is already running
* Super Bug Fixed In Aircrack
* Now Aircrack will restart by himself when fails. No time.
* Added Method to stop all attacks
* Added Method to show additional info about BSSIDs
* Advanced Optimizacions in airodump process.
* Other little BugFixes And Optimizations

0.6.5 RC
***********************************************************
IMPORTANT: STOP ALL PROCESSES OF AIRCRACK-GUI-M4 BEFORE UPDATING!!
IF you update the program while any process is running, the update
could corrupt any bin
***********************************************************
* CHOPCHOP ATTACK IMPLEMENTED!! [Experimental]
* New method to forge arp in chopchop attack that INCREASE X 3 INJECTION RATE
* Fix bug when reached 200 ESSID [thx to drvalium]
* Max ESSID increased to 500
* Now captures from attack broadcast are stored in broadcast captures
* Option Interface Rebuild
* Added options to fragmentation and chopcop attack
* Other optimizations

0.6.0 RC
***********************************************************
IMPORTANT: STOP ALL PROCESSES OF AIRCRACK-GUI-M4 BEFORE UPDATING!!
IF you update the program while any process is running, the update
could corrupt any bin
***********************************************************
* FRAGMENTATION attack IMPLEMENTED! [Experimental]
* aireplay.c from original aircrack-ng Modifided - then bin changes [IMPORTANT!!]
* SUPER-BUG FIXED: Now, ALL STATIONS ASSOCIATED will appear in the below table when capturing
* Send rate fixed to 512 as standard
* Implement method to do all attacks in the same time.
* Method to forge arp request implemented

0.5.1 v2
 Now it is not necessary to unrar anything to update program.

0.5.1
 Attack Interactive ARP IMPLEMENTED.
 New directory to store replays from ARP reinjection. Automatic update from old versions =D.
 Fix Bug macchanger : in some distributions is necessary to install packet macchanger.
 Fix CRITICAL bug when try to attack without row selected
 Fix in airmon window when resolution is lower than 1024x768 [Thx to pac0 (seguridadwireless.net)]
 Add splash screen when loading the program

0.5.0 Beta2
 Fix CRITICAL Bug when trying crack in historical network without row selected

0.5.0 Beta
 AUTO-UPDATE IMPLEMENTED!!!!
 Main Table Of Airodump Rebuild
 Macchanger Fix Bug
 ARP replay Fix bug
 Aircrack Fix Bug
 Autodetection of interface in monitor mode
 Autosetting of interface after execute airmon
 Key autoformatted in Historical Network
 Crack from Historical Network
 Code Optimized
[/*CHANGELOG*]
```


Front-end of Aircrack-ng suite. Includes others wireless tools like macchanger.
This program has been created to increase performance and accesibility to the controlling and scanning wireless networks.

DEMO OF PROGRAM :)

CODE + ATTACK http://www.youtube.com/watch?v=lFpYvhHbHWw

0.1.x Alfa version http://www.youtube.com/watch?v=MIbxwBXt1Ks

0.5.0 Beta version http://www.youtube.com/watch?v=e5OYPsqUqLo

0.6.0 RC http://www.youtube.com/watch?v=exE2sN37aQY


---

> ## NOTES ##

---


# IF YOU HAVE PROBLEMS, DUE TO THAT CAPTURE FILES ARE NOT CREATED, JUST REMOVE WHITE SPACES IN THE PATH OF THE PROGRAM #
(thx to juanfurox@gmail.com)


---
