VISSR200032 DLL Documentation


Overview

The VISSR200032 DLL is a software component developed for interfacing with the Keyence SR2000 barcode scanner via Ethernet. It is designed to work in conjunction with the IMS5000 software installed on a Windows 10 operating system. This documentation provides an overview of the DLL, its installation requirements, functionality, and usage instructions.


Installation

To use the VISSR200032 DLL, ensure the following:

•	IMS5000 software is installed and operational on a Windows 10 machine.
•	Copy the VISSR200032.dll, VISSR200032.ini, and INIT.ini files to the bin directory of IMS5000.


Functionality

The VISSR200032 DLL provides the following core functions:

1.	Reading a Scan: The DLL triggers the Keyence SR2000 barcode scanner to scan a barcode.
2.	Receiving Data: The DLL receives the scanned barcode data from the Keyence SR2000 scanner.
3.	Test Mode Toggling: The DLL allows toggling between test mode and normal mode of the scanner.
4.	Sending an Output Report: The DLL sends the Date, Time, Material ID, Batch ID, Data, Alphabetical Grade, and Numerical Grade to the spectrolab_barcode_data.csv located on the desktop directory.
Command Usage

The DLL communicates with the Keyence SR2000 scanner using the following commands:

1.	"LON\r": Triggers the scanner to perform a barcode scan.
2.	"LOFF\r": Turns off the scanner trigger.
3.	"TEST1\r": Activates test mode, where the scanner's diffused LED lights indicate the scanning status (RED for no scan, GREEN for successful scan).
4.	"QUIT\r": Terminates the test mode of the scanner.

Please note that the "FTUNE\r" and "TUNE,01\r" commands, used for tuning the scanner, are not utilized in the VISSR200032 DLL.  They are used in the SR2000TUNING standalone program created for this DLL.


Troubleshooting

When using the VISSR200032 DLL, common troubleshooting errors may include:

1.	Output Report File Open: Ensure that the output report file is not open when inspecting or running the DLL.
2.	API Version Compatibility: Verify that the correct API versions are used to ensure compatibility between the DLL and IMS5000.
3.	Proper Connection: Ensure that the Keyence SR2000 barcode scanner is properly connected to the PC.
4.	Configuration Values: Double-check the configuration values and ensure they are correct, as mistypes can cause issues.
5.	Timeout Errors: If timeout errors occur, review the network connection and adjust timeout settings if necessary.


 
Usage

The VISSR200032 DLL serves the objective of reading a 2D barcode, comparing it to the Prep-string, and performing the following actions if a match occurs:

- Write the date, time, BatchID (from INIT.ini), MaterialID (from INIT.ini), DATA, Alphabetical grade, and Numeric grade to an output file.

In addition to the core objectives, the DLL provides the following features:

1.	Read Test: This feature allows users to test the DLL's barcode reading functionality.
2.	Live Test: The Live Test feature provides a real-time view of the scanner's diffused LED lights to monitor scanning status.

Conclusion

The VISSR200032 DLL enables seamless communication with the Keyence SR2000 barcode scanner and enhances the functionality of the IMS5000 software. This documentation provides an overview of the DLL's installation, core functions, command usage, troubleshooting tips, and usage instructions.
