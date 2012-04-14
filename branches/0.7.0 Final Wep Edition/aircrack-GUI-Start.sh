#***********************************************************"
#VARIABLES
#***********************************************************"
debug="true"


echo "Checking libs..."
librerias="
lib/libQtCore.so.4
lib/libQtGui.so.4
"
correctas=1
echo "***********************************************************"
echo "This program requires:"
echo "libqt4-core version 4.7.3"
echo "libqt4-gui version 4.7.3"
echo "***********************************************************"

for i in $librerias; do
if [ ! -f "$i" ]; then
        echo "* ¡ERROR! Lib $i not found"
        correctas=-1
else
        echo "Lib $i correct"

fi
done

if [ $correctas = -1 ]; then
        echo "***********************************************************"
        echo "Libs are not in the lib folder, if you dont have installed the libs program will fail"
        echo "***********************************************************"
        echo "Checking dependencies..."
        echo "***********************************************************"

        ldd aircrack-GUI
fi

echo "***********************************************************"
echo "Exporting LD_LIBRARY_PATH (To include actual folder)..."
echo "***********************************************************"
LD_LIBRARY_PATH="$LD_LIBRARY_PATH:./lib"
export LD_LIBRARY_PATH
echo "***********************************************************"
echo "Executing Aircrack M4!"
echo "***********************************************************"
if [ $debug = "true" ]; then
    ./aircrack-GUI -d
else
    ./aircrack-GUI
fi

echo "***********************************************************"
echo "PULSE ENTER TO EXIT"
echo "***********************************************************"
read ENTER

