#include "stdAfx.h"
#include "SR2000Dev.h"
#include "conversions.h"


const int RECV_DATA_MAX	= 10240;
bool gMessageBoxDisplayed = false;
 

/***************************************************************************
	Socket Class for usability with WinSock 
***************************************************************************/
class SOCKADDR4 : public sockaddr_in
{
public:
	SOCKADDR4(char* ip, int port)
	{
		sin_family = AF_INET;
		int conversion_result = inet_pton(AF_INET, ip, &(sin_addr));
		if (conversion_result <= 0) {
			if (conversion_result == 0) {
				// Check if a message box is already displayed
				if (gMessageBoxDisplayed) {
					return; // Skip displaying the message box
				}

				// Set the flag to indicate a message box is displayed
				gMessageBoxDisplayed = true;

				// Show the message box
				MessageBoxA(NULL, "inet_pton: Invalid address format", "Error", MB_OK | MB_ICONERROR);

				// Reset the flag after the message box is closed
				gMessageBoxDisplayed = false;
			}
			else {
				// Check if a message box is already displayed
				if (gMessageBoxDisplayed) {
					return; // Skip displaying the message box
				}

				// Set the flag to indicate a message box is displayed
				gMessageBoxDisplayed = true;

				// Show the message box
				MessageBoxA(NULL, "inet_pton: Error occurred", "Error", MB_OK | MB_ICONERROR);

				// Reset the flag after the message box is closed
				gMessageBoxDisplayed = false;
			}
			exit(1);
		}
		sin_port = htons(port);
	}
};



/***************************************************************************
	Ctor/Dtor
***************************************************************************/
SR2000DEV::SR2000DEV() {
	if (WSAStartup(MAKEWORD(2, 0), &wsaData) == NO_ERROR) {
		socketsLoaded = true;
	}
}


SR2000DEV::~SR2000DEV() {
	//upon deletion of object
	//free up sockets
	if (connected) {
		closesocket(s_commandSocket);
		closesocket(s_dataSocket);
		s_commandSocket = INVALID_SOCKET;
		s_dataSocket = INVALID_SOCKET;
	}
	//and release winsock dll if startup successful
	if (socketsLoaded) {
		WSACleanup();
	}

}


//Function to initialize sockets to ready state
void SR2000DEV::SrClientSocket_Init() {
	s_commandSocket = INVALID_SOCKET;
	s_dataSocket = INVALID_SOCKET;
}


//sets information needed from .ini file. True if loaded successfully.  
//One fail condition is if sockets did not load properly
BOOL SR2000DEV::init(WORD idc, HWND parent, LPSTR iniWide, LPSTR sectionWide)
{

	emsg[0] = '\0'; // Make sure it is cleared first

	this->idc = idc;
	this->parent = parent;

	// Attempt to get DeviceName
	if (GetPrivateProfileString(sectionWide, "DeviceName", "", DeviceName, sizeof(DeviceName), iniWide) <= 0)
	{
		strncpy_s(emsg, sizeof(emsg), "Device Name not found", _TRUNCATE);
		
		// Show the message box
		//MessageBoxA(NULL, emsg, "Error", MB_OK | MB_ICONERROR);

		return false;
	}

	// Attempt to get CommandPort
	if (GetPrivateProfileString(sectionWide, "CommandPort", "", CommandPort, sizeof(CommandPort), iniWide) <= 0)
	{
		strncpy_s(emsg, sizeof(emsg), "Command Port not found", _TRUNCATE);
		
		// Show the message box
		//MessageBoxA(NULL, emsg, "Error", MB_OK | MB_ICONERROR);

		return false;
	}

	// Attempt to get DataPort
	if (GetPrivateProfileString(sectionWide, "DataPort", "", DataPort, sizeof(DataPort), iniWide) <= 0)
	{
		strncpy_s(emsg, sizeof(emsg), "Data Port not found", _TRUNCATE);
		
		// Show the message box
		//MessageBoxA(NULL, emsg, "Error", MB_OK | MB_ICONERROR);

		return false;
	}

	// Attempt to get DeviceIP
	if (GetPrivateProfileString(sectionWide, "DeviceIP", "", DeviceIP, sizeof(DeviceIP), iniWide) <= 0)
	{
		strncpy_s(emsg, sizeof(emsg), "Device IP address not found", _TRUNCATE);
		
		// Show the message box
		//MessageBoxA(NULL, emsg, "Error", MB_OK | MB_ICONERROR);

		return false;
	}

	// Attempt to get SettlingTime
	if ((timeoutSeconds = GetPrivateProfileInt(sectionWide, "SettlingTime", 0, iniWide)) <= 0)
	{
		strncpy_s(emsg, sizeof(emsg), "Settling time not found", _TRUNCATE);
		
		// Show the message box
		//MessageBoxA(NULL, emsg, "Error", MB_OK | MB_ICONERROR);

		return false;
	}

	//// Attempt to get MaterialID
	//if (GetPrivateProfileString(sectionWide, "MaterialID", "", materialID, sizeof(materialID), iniWide) <= 0)
	//{
	//	strncpy_s(emsg, sizeof(emsg), "Material ID not found", _TRUNCATE);
	//
	//	// Show the message box
	//	//MessageBoxA(NULL, emsg, "Error", MB_OK | MB_ICONERROR);

	//	return false;
	//}

	//// Attempt to get BatchID
	//if (GetPrivateProfileString(sectionWide, "BatchID", "", batchID, sizeof(batchID), iniWide) <= 0)
	//{
	//	strncpy_s(emsg, sizeof(emsg), "Batch ID not found", _TRUNCATE);
	//	
	//	// Show the message box
	//	//MessageBoxA(NULL, emsg, "Error", MB_OK | MB_ICONERROR);

	//	return false;
	//}

	// Open and test TCP connection
	if (!socketsLoaded)
	{
		strncpy_s(emsg, sizeof(emsg), "Sockets not loaded", _TRUNCATE);
	
		// Show the message box
		//MessageBoxA(NULL, emsg, "Error", MB_OK | MB_ICONERROR);

		return false;
	}

	/*
	// Display message box with saved information
	std::wstring message = L"Device Name: " + std::wstring(DeviceName) + L"\n"
		+ L"Command Port: " + std::wstring(CommandPort) + L"\n"
		+ L"Data Port: " + std::wstring(DataPort) + L"\n"
		+ L"Device IP Address: " + std::wstring(DeviceIP) + L"\n";

	
	// Check if a message box is already displayed
	if (gMessageBoxDisplayed) {
		return; // Skip displaying the message box
	}

	// Set the flag to indicate a message box is displayed
	gMessageBoxDisplayed = true;

	// Show the message box
	MessageBox(NULL, message.c_str(), L"Initialization Successful", MB_OK | MB_ICONINFORMATION);

	// Reset the flag after the message box is closed
	gMessageBoxDisplayed = false;

	// Add a delay to prevent consecutive message boxes
	std::this_thread::sleep_for(std::chrono::milliseconds(200)); // Adjust the sleep duration as needed
		
	*/

	return true;
}


//This function connects us to the scanner.  It makes a connection
//and manages the sockets
BOOL SR2000DEV::SrClientSocket_Connect() {

	//convert Command and Data Port to an int
	int commandPort = strtol(CommandPort, NULL, 10);
	int dataPort = strtol(DataPort, NULL, 10);
	if ((commandPort < 1) || (dataPort < 1)) {
		strncpy_s(emsg, sizeof(emsg), "Ports Cannot Be Used, Check for Typos...", _TRUNCATE);
		return FALSE;
	}

	//convert DeviceIP to char* to use in SOCKADDR4
	size_t len = strlen(DeviceIP) + 1;
	char* cDeviceIP = new char[len];
	std::size_t converted_chars = 0;
	errno_t err = strncpy_s(cDeviceIP, len, DeviceIP, _TRUNCATE);
	if (err != 0) {
		delete[] cDeviceIP;
		cDeviceIP = nullptr;
	}

	SOCKADDR4 servAddr(cDeviceIP, commandPort);
	
	//close command socket if open already
	if (s_commandSocket != INVALID_SOCKET) {
		closesocket(s_commandSocket);
		s_commandSocket = INVALID_SOCKET;
	}

	//close data socket if open already
	if (s_dataSocket != INVALID_SOCKET) {
		closesocket(s_dataSocket);
		s_dataSocket = INVALID_SOCKET;
	}

	// create a new command socket
	s_commandSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);	//May need to set third argument to 0 instead of IPPROTO_TCP
	if (s_commandSocket != INVALID_SOCKET) {
		if (connect(s_commandSocket, (SOCKADDR*)&servAddr, sizeof(servAddr)) != SOCKET_ERROR) {
			connected = true;
		}	
	}
	if (!connected) {
		strncpy_s(emsg, sizeof(emsg), "Device connection failed", _TRUNCATE);
		closesocket(s_commandSocket);
		s_commandSocket = INVALID_SOCKET;
		return FALSE;
	}

	//if Data and Command Port are not identical, connect to the Data Port
	if (strcmp(DataPort, CommandPort) != 0) {
		connected = false;
		servAddr.sin_port = (dataPort);

		//create a new socket
		s_dataSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);	//May need to set third argument to 0 instead of IPPROTO_TCP

		if (s_dataSocket != INVALID_SOCKET) {
			DWORD timeout = 15000; // ms (allow long time for inspects)
			if (setsockopt(s_dataSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout)) == 0) {
				if (connect(s_dataSocket, (SOCKADDR*)&servAddr, sizeof(servAddr)) != SOCKET_ERROR) {
					connected = true;
				}
			}
		}

		if (!connected) {
			strncpy_s(emsg, sizeof(emsg), "Device connection failed when Data Port and Command Port are different", _TRUNCATE);
			closesocket(s_dataSocket);
			s_dataSocket = INVALID_SOCKET;
			return FALSE;
		}
	}
	else {
		s_dataSocket = s_commandSocket;
	}
	
	delete[] cDeviceIP; //will delete at the end in case it is needed elsewhere
	return TRUE;
}


//This function closes and resets the command and data sockets
void SR2000DEV::SrClientSocket_Disconnect() {
	//close and reset command socket
	if (s_commandSocket != INVALID_SOCKET) {
		closesocket(s_commandSocket);
		s_commandSocket = INVALID_SOCKET;
	}

	//close and reset data socket
	if (s_dataSocket != INVALID_SOCKET) {
		closesocket(s_commandSocket);
		s_dataSocket = INVALID_SOCKET;
	}

	WSACleanup();
}


//This function triggers the scanner to set the scan timing on
void SR2000DEV::SrClientSocket_Lon() {

	// Send "LON" command 
	char command[] = "LON\r";	// CR is the terminator

	if (s_commandSocket != INVALID_SOCKET) {
		send(s_commandSocket, command, strlen(command), 0);
	}
	else {
		strncpy_s(emsg, sizeof(emsg), "Device is disconnected", _TRUNCATE);
	}
}


//This function turns off the trigger for the scanner
//should be a manual option for the operator
void SR2000DEV::SrClientSocket_Loff() {

	//send "LOFF" command
	char command[] = "LOFF\r";	//CR is the terminator

	if (s_commandSocket != INVALID_SOCKET) {
		send(s_commandSocket, command, strlen(command), 0);
	}
	else {
		strncpy_s(emsg, sizeof(emsg), "Device is disconnected", _TRUNCATE);
	}
}


//This function triggers the scanner to continuously trigger
void SR2000DEV::SrClientSocket_Test() {

	// Send "TEST1" command 
	char command[] = "TEST1\r";	// CR is the terminator

	if (s_commandSocket != INVALID_SOCKET) {
		send(s_commandSocket, command, strlen(command), 0);
	}
	else {
		strncpy_s(emsg, sizeof(emsg), "Device is disconnected", _TRUNCATE);
	}
}


//This function triggers the scanner to turn off continuous trigger
void SR2000DEV::SrClientSocket_Quit_Test() {

	// Send "QUIT" command 
	char command[] = "QUIT\r";	// CR is the terminator

	if (s_commandSocket != INVALID_SOCKET) {
		send(s_commandSocket, command, strlen(command), 0);
	}
	else {
		strncpy_s(emsg, sizeof(emsg), "Device is disconnected", _TRUNCATE);
	}
}


/*
This is a local function used to find the INIT ini file
inside of whatever ims5000 program has the program saved
	Ex: if the IMP program is running from desktop, it looks 
		for the INIT file inside of that IMS5000 directory
NOTE:This function ended up not being needed, but will remain for other use cases - 9/15/23
*/
std::string findIniPath() {
	char buffer[MAX_PATH];
	DWORD length = GetModuleFileNameA(NULL, buffer, MAX_PATH);

	if (length != 0) {
		std::string path(buffer, length);
		size_t pos = path.find("ims5000") - 1;
		
		if (pos != std::wstring::npos) {
			std::string folder = path.substr(0, pos);
			std::string iniPath = folder + "\\ims5000\\bin\\INIT.ini";
	
			if (GetFileAttributesA(iniPath.c_str()) != INVALID_FILE_ATTRIBUTES) {
				return iniPath;
			}
		}
	}

	return "";
}


//Takes the data received from the scanner and compares it with the prep string
void SR2000DEV::SrClientSocket_Compare(const std::string& data) {
	// lamdba function to trim the data and matchCode
	auto trim = [](const std::string& s) {
		size_t start = s.find_first_not_of(" \t\r\n");
		size_t end = s.find_last_not_of(" \t\r\n");
		return (start == std::string::npos) ? "" : s.substr(start, end - start + 1);
	};

	std::string trimmedData = trim(data);
	std::string trimmedMatchCode = trim(matchCode);

	/*Used to evaluate match code*/
	//char path[MAX_PATH];
	//SHGetFolderPathA(NULL, CSIDL_DESKTOP, NULL, 0, path);  // SHGetFolderPathA is defined in <Shlobj.h>

	//std::string desktopPath = std::string(path);
	//std::ofstream csvFile(desktopPath + "\\output.csv", std::ios::app);  // open in append mode
	//if (csvFile.is_open()) {
	//	csvFile << trimmedMatchCode << "\n";
	//	csvFile.close();
	//}

	//Parse data from match code
	/*Format: "_JOB=....,_DAT=....,_MID=...,_BID=...." */
	size_t datIndex = trimmedMatchCode.find("DAT=");
	size_t mIDIndex = trimmedMatchCode.find(",_MID", datIndex + 4);
	std::string prepstringData = trimmedMatchCode.substr(datIndex + 4, mIDIndex - (datIndex + 4));

	

	// Compare the received message with matchCode
	size_t colonPos = trimmedData.find(':');
	if (colonPos != std::string::npos) {
		std::string receivedCode = trimmedData.substr(0, colonPos);

		//For Debugging
		//MessageBox(NULL, ("prepStringData from Compare = " + prepstringData + "\n receivedCode = " + receivedCode).c_str(), "Inspect", MB_ICONEXCLAMATION | MB_OK);

		if (receivedCode == prepstringData) {
			Notify(VISN_PASS);
			// Call SrClientSocket_Receive with the received data
			SrClientSocket_Receive(trimmedData, trimmedMatchCode);
		}
		else {
			Notify(VISN_FAIL);
		}
	}
	else {
		Notify(VISN_ERROR);
	}
}


//This function receives and handles the data from the scanner
void SR2000DEV::SrClientSocket_Receive(std::string code, std::string prepstring) {
	
	char desktopPath[MAX_PATH];
	std::string filePath;
	std::ofstream outFile;

	// Get the path to the desktop directory
	if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_DESKTOPDIRECTORY, NULL, 0, desktopPath))) {
		filePath = std::string(desktopPath) + "\\spectrolab_barcode_data.csv";
	// Attempt to open file
	outFile.open(filePath, std::ios_base::app);
	}

	//attempts at opening the file
	int attempts = 0;
	//program attempts to open the file 3 times, each time the user is prompted of this
	//and it given the option to try again or cancel
	while (!outFile.is_open() && attempts < 3) {
		
		// Check if a message box is already displayed
		if (gMessageBoxDisplayed) {
			return; // Skip displaying the message box
		}

		// Set the flag to indicate a message box is displayed
		gMessageBoxDisplayed = true;

		// Show the message box
		int msgBoxID = MessageBox(NULL,
			"The data file is currently in use by another program. Please close it and click 'Retry'. Click 'Cancel' to stop the operation.",
			"File Error",
			MB_RETRYCANCEL | MB_ICONWARNING);

		// Reset the flag after the message box is closed
		gMessageBoxDisplayed = false;

		// Add a delay to prevent consecutive message boxes
		std::this_thread::sleep_for(std::chrono::milliseconds(200)); // Adjust the sleep duration as needed
		

		if (msgBoxID == IDCANCEL) {
			strncpy_s(emsg, sizeof(emsg), "File operation cancelled by user", _TRUNCATE);
			return;
		}

		// Retry file opening
		outFile.open(filePath, std::ios_base::app);
		attempts++;
	}

	//If the file is still not open after 3 tries, we get this error
	if (!outFile.is_open()) {
		strncpy_s(emsg, sizeof(emsg), "Could not open data file", _TRUNCATE);
		return;
	}

	// Check if the file is empty or does not contain column names
	std::ifstream inFile(filePath);
	std::string firstLine;
	std::getline(inFile, firstLine);

	// Parse code
	std::string materialID;
	std::string batchID;
	std::string data;

	// Grading variables
	std::map<int, int> ISO_15415;
	std::istringstream iss;
	std::string token;
	std::string character;

	// Grading for ISO_16415 standard
	enum ISO_15415_Grades {
		OVERALL,
		DECODE,
		SYMBOL_CONTRAST,
		MODULATION,
		REFLECTANCE_MARGIN,
		FIXED_PATTERN_DMG,
		FORMAT_INFO_DMG,
		VERSION_INFO_DMG,
		AXIAL_NONUNIFORMITY,
		GRID_NONUNIFORMITY,
		UNUSED_ERR_CORRECTION,
		PRINT_GROWTH_HORIZONTAL,
		PRINT_GROWTH_VERTICAL,
		END
	};

	/***********************************************************************************************/
	//NOTE:
	//Add in other grading standards as needed, make sure to adjust code to use new grading standard
	//If enough variance is added to standards, reference to a variable in the ini to select grading
	//standards
	/***********************************************************************************************/


	// Check if the first line is empty
	if (firstLine.empty()) {

		//For Debugging
		//MessageBox(NULL, "Empty file detected!", "Receive", MB_ICONEXCLAMATION | MB_OK);
		
		// Write new column names
		outFile << std::left << std::setw(15) << "DATE" << ", "
			<< std::setw(15) << "TIME" << ", " 
			<< std::setw(15) << "MaterialID" << ", "
			<< std::setw(15) << "BatchID" << ", "
			<< std::setw(15) << "DATA" << ", "
			<< std::setw(15) << "Alpha Grade" << ", "
			<< std::setw(15) << "Num Grade" << std::endl;
	}

	//the prepstring is in the following format:
	/* "_JOB=....,_DAT=....,_MID=...,_BID=...."*/
	size_t materialIDIndex = prepstring.find(",_MID=");
	size_t batchIDIndex = prepstring.find(",_BID=", materialIDIndex + 6);
	if (materialIDIndex != std::string::npos) {
		materialID = prepstring.substr(materialIDIndex + 6, batchIDIndex - (materialIDIndex + 6));
		batchID = prepstring.substr(batchIDIndex + 6);
	}

	//find the colon and save the rest of the string in the string stream
	size_t colonIndex = code.find(":");
	if (colonIndex != std::string::npos) {
		data = code.substr(0, colonIndex);
		iss.str(code.substr(colonIndex + 1));
		
		//while there are values to extract, save them to the map as ints
		int index = 0;
		while (std::getline(iss, token, '/') && index < ISO_15415_Grades::END) {
			if (token != "-") {
				ISO_15415[index] = std::stoi(token);
			}
			else {
				ISO_15415[index] = 0;
			}
			index++;
		}
	}
	else {
		OutputDebugString("Error Reading Data\n");
	}

	// Extracting grades needed - can change this to specified grades in ini file
	int axial = ISO_15415[ISO_15415_Grades::AXIAL_NONUNIFORMITY];
	int growth_H = ISO_15415[ISO_15415_Grades::PRINT_GROWTH_HORIZONTAL];
	int growth_V = ISO_15415[ISO_15415_Grades::PRINT_GROWTH_VERTICAL];

	//set the grade to the minimum grade
	int grade = min(axial, min(growth_H, growth_V));

	//assign a letter grade
	if (grade == 4) character = "A";
	else if (grade == 3) character = "B";
	else if (grade == 2) character = "C";
	else if (grade == 1) character = "D";
	else if (grade == 0) character = "F";
	else character = "-"; // Using 0 to denote a blank space

	// Get current date and time
	auto t = std::time(nullptr);
	tm localTime;

	for (int attempt = 0; attempt < 3; ++attempt) {
		errno_t err = localtime_s(&localTime, &t);
		if (err == 0) {
			// Write data to file
			std::ostringstream oss;
			oss << std::put_time(&localTime, "%Y-%m-%d") << ", "
				<< std::put_time(&localTime, "%H:%M:%S") << ", "
				<< materialID << ", "
				<< batchID << ", "
				<< data << ", " << character;
			if (grade != 0) {
				oss << ", " << grade;
			}
			oss << "\n";
			outFile << oss.str();
			break;
		}
		else if (err == EOVERFLOW) {
		
		// Check if a message box is already displayed
		if (gMessageBoxDisplayed) {
			return; // Skip displaying the message box
		}

		// Set the flag to indicate a message box is displayed
		gMessageBoxDisplayed = true;

		// Show the message box
		int msgboxID = MessageBox(
			NULL,
			"An error occurred when getting the current date and time. Would you like to try again?",
			"Date and Time Error",
			MB_ICONWARNING | MB_YESNO
		);

		// Reset the flag after the message box is closed
		gMessageBoxDisplayed = false;

		// Add a delay to prevent consecutive message boxes
		std::this_thread::sleep_for(std::chrono::milliseconds(200)); // Adjust the sleep duration as needed
		

			if (msgboxID == IDNO) {
				outFile << "Date and time could not be acquired," << code << "\n";
				break;
			}
		}
		else {
			// Create error message
			char err_msg[256];
			strerror_s(err_msg, sizeof(err_msg), err);
			std::string errorMessage = "An unexpected error occurred: ";
			errorMessage += err_msg;

			
			// Check if a message box is already displayed
			if (gMessageBoxDisplayed) {
				return; // Skip displaying the message box
			}

			// Set the flag to indicate a message box is displayed
			gMessageBoxDisplayed = true;

			// Convert to a format suitable for MessageBox (LPCWSTR) - for unicode only
			//std::wstring wstr = std::wstring(errorMessage.begin(), errorMessage.end());

			LPCSTR finalMessage = errorMessage.c_str();
			// Show the message box
			MessageBox(NULL, finalMessage, "Error", MB_ICONERROR | MB_OK);
			break;

			// Reset the flag after the message box is closed
			gMessageBoxDisplayed = false;

			// Add a delay to prevent consecutive message boxes
			std::this_thread::sleep_for(std::chrono::milliseconds(200)); // Adjust the sleep duration as needed
		

		}
	}
	// If data is successfully written, close the file
	outFile.close();
}


BOOL SR2000DEV::open(WORD idc, HWND parent, LPSTR ini, LPSTR section)
{

	if (!isOpen)
		isOpen = init(idc, parent, ini, section);

	return isOpen;
}


/****************************************************************************
	Prep

	Handles the prepstring in accordance with the requirements of the
	specific VIS DLL.
	In this case, we are saving the prepstring for the call to inspect.

	Arguments: char* prepstring - prep string entered by the user
****************************************************************************/
void SR2000DEV::prep(char* prepstring) {

	if (prepstring == nullptr) {
		Notify(VISN_ERROR); // Notify error if prepstring is null
		return;
	}

	strcpy(matchCode, prepstring);

	// Check for errors during strcpy
	if (errno != 0) {
		Notify(VISN_ERROR); // Notify error if an error occurred during strcpy
	}
	
}


/***************************************************************************
	Notify(n)

	Sends a notification message to the parent of this dev.

	LPARAM n = Notification code.
***************************************************************************/
void SR2000DEV::Notify(WORD n)
{
	PostMessage(parent, WM_COMMAND, MAKELONG(idc, n), idc);
	/////////////////////////////////////////////////////
	///
	if (DebugMode)
	{
		OutputDebugString("VIS200032 POST ");

		OutputDebugString(
			n == VISN_SNAP ? "SNAP " :
			n == VISN_BUSY ? "BUSY " :
			n == VISN_PASS ? "PASS " :
			n == VISN_FAIL ? "FAIL " :
			n == VISN_READ ? "READ " :   // ReadData
			n == VISN_ERROR ? "ERROR " : // ErrorText
			n == VISN_LOCATE ? "LOCATE " : // LocatePoints
			"UNKNOWN " // n
		);

		OutputDebugString(
			dev.mode == xRead ? "xRead" :
			dev.mode == xInspect ? "xInspect" :
			dev.mode == xLocate ? "xLocate" :
			dev.mode == xLiveLocate ? "xLiveLocate" :
			dev.mode == xLocatePostJog ? "xLocatePostJog" :
			dev.mode == xSnap ? "xSnap" :
			"?"
		);
		OutputDebugString("\r\n");
	}
	//////////////////////////////////////////////////////////////////////////////
	
}


/***************************************************************************
	SrClientSocket_Inspect()

	Triggers a scan by calling SrClientSocket_Lon.  Function then waits for
	a message back. When it receives a messages back, it compares it to the 
	matchCode.  If it matches, call Notify(VISN_PASS) and then call
	SrClientSocket_Receive(code) to handle the data to send to a file

	
***************************************************************************/
void SR2000DEV::SrClientSocket_Inspect() {

	//if an empty string is returned, return function
	std::string receivedData = socketCommunication();	//helper function to handle comms with socket
	if (receivedData.empty()) {
		SrClientSocket_Loff();
		Notify(VISN_ERROR);
		return;
	}

	//Process the received data for a match
	SrClientSocket_Compare(receivedData);
}


/***************************************************************************
	SrClientSocket_Inspect()

	Triggers a scan by calling SrClientSocket_Lon.  Function then waits for
	a message back. When it receives a messages back, it notifies outputs 
	a VIS_READ

***************************************************************************/
void SR2000DEV::SrClientSocket_Read() {
	
	//if an empty string is returned, return function
	std::string receivedData = socketCommunication();	//helper function to handle comms with socket
	if (receivedData.empty()) {
		SrClientSocket_Loff();
		Notify(VISN_ERROR);
		return;
	}

	//If we haven't timed out, Notify that we read
	Notify(VISN_READ);
}


DWORD WINAPI _LiveLocateProc(LPSTR lpData)
{
	return ((SR2000DEV*)lpData)->LiveLocateProc();
}


// ok = startLiveLocate()
// Starts the live locate thread
BOOL SR2000DEV::startLiveLocate()
{
	if (hLiveLocateThread)
		return !killLive;
	killLive = false;
	hLiveLocateThread = CreateThread
	((LPSECURITY_ATTRIBUTES)NULL
		, 0
		, (LPTHREAD_START_ROUTINE)_LiveLocateProc
		, (LPVOID)this
		, 0
		, &dwLiveLocateThreadId
	);
	if (!hLiveLocateThread)
		return false;
	return true;
}


BOOL SR2000DEV::killLiveLocate()
{
	HANDLE h = hLiveLocateThread;
	if (h)
	{
		killLive = true;
		return WaitForSingleObject(h, 1000) == WAIT_OBJECT_0;
	}
	return true;
}


DWORD SR2000DEV::LiveLocateProc()
{
	BOOL progSelected = false;
	while (!killLive)
	{
		Sleep(100);
		//if (!progSelected)
			//progSelected = selProg();
		//else
			//trigAndParse();
	}
	hLiveLocateThread = 0;
	killLive = false;
	return 0;
}


void SR2000DEV::getXYPos()
{
	// Get current xyz locate point
	SendMessage(xywnd, XYM_GETXYPOS, 0, (LPARAM)(LPDOUBLE)&xypos.x);
	SendMessage(xywnd, XYM_GETZPOS, 0, (LPARAM)(LPDOUBLE)&xypos.z);
}


//void SR2000DEV::moveXYRel(V2& v)
//{
//	getXYPos();
//	V2 pos = V2(xypos.x, xypos.y)+v;
//	SendMessage(xywnd, XYM_MOVE, 0, (LPARAM)(LPDOUBLE)&pos.v2);
//}



/**********************************************************************************
	Helper Functions

***********************************************************************************/

std::string SR2000DEV::socketCommunication() {
	// Trigger a scan by calling SrClientSocket_Lon
	SrClientSocket_Lon();
	
	// Wait for a non-empty message back with a timeout in seconds specified by the ini
	char responseBuffer[256];
	std::string receivedData;
	int bytesRead;

	struct timeval timeout;
	fd_set readfds;

	while (true) {
		//init the set of active sockets
		FD_ZERO(&readfds);
		FD_SET(s_commandSocket, &readfds);

		//set the timeout specified in the ini
		timeout.tv_sec = SettlingTime / 1000;
		timeout.tv_usec = (SettlingTime % 1000) * 1000;	// microseconds in conjunction with seconds

		int activity = select(s_commandSocket + 1, &readfds, NULL, NULL, &timeout);

		if (activity == SOCKET_ERROR) {
			handleError("Error getting a response, check the socket.");
			clearDeviceBuffer(s_commandSocket);  // Clear the buffer
			return "";
		}

		//if timeout, handle it
		if (activity == 0) {
			handleTimeout();
			clearDeviceBuffer(s_commandSocket);  // Clear the buffer
			return "";
		}

		if (activity == -1) {
			handleError("Errno: " + errno);
			clearDeviceBuffer(s_commandSocket); //Clear the buffer
		}

		//if the socket has data
		if (FD_ISSET(s_commandSocket, &readfds)) {
			bytesRead = recv(s_commandSocket, responseBuffer, sizeof(responseBuffer) - 1, 0);
			if (bytesRead == SOCKET_ERROR) {
				//gets the error code from "Windows Socket API"
				int errorCode = WSAGetLastError();

				responseBuffer[bytesRead] = '\0'; //Null-terminate the received data
				receivedData.assign(responseBuffer, bytesRead);

				//if an error is received, keep reading from the buffer until it is empty
				if (receivedData.compare("ERROR") == 0) {
					clearDeviceBuffer(s_commandSocket);
				}

				if (errorCode == WSAETIMEDOUT) {
					handleTimeout();
					return "";
				}
				else {
					//else error is not cause from a timeout
					handleError("Error receiving a response from the scanner!");
					return "";
				}
			}

			responseBuffer[bytesRead] = '\0'; //Null-terminate the received data
			receivedData.assign(responseBuffer, bytesRead);

			//if an error is received, keep reading from the buffer until it is empty
			if (receivedData.compare("ERROR") == 0) {
				clearDeviceBuffer(s_commandSocket);
			}

			if (!receivedData.empty() && !std::all_of(receivedData.begin(), receivedData.end(), ::isspace)) {
				break;	//Non-empty response received, break the loop and handle data
			}
		}
	}
	return receivedData;
}


void SR2000DEV::clearDeviceBuffer(SOCKET s) {
	char tempBuffer[256]; // Temporary buffer
	std::string tempData;
	int recvSize;

	// Optionally set the socket to non-blocking mode here

	while (true) {
		recvSize = recv(s, tempBuffer, sizeof(tempBuffer), 0);

		tempBuffer[recvSize] = '\0'; //Null-terminate the received data
		tempData.assign(tempBuffer, recvSize);

		if (tempData.compare("ERROR") == 0) {
			continue; 
		}
		else {
			break;
		}
	}
}


void SR2000DEV::handleTimeout() {
	SrClientSocket_Loff();
	showMessageBox("Timeout waiting for response"); // Refactored message box displaying into a function
	//Notify(VISN_ERROR);
}


void SR2000DEV::handleError(const char* message) {
	SrClientSocket_Loff();
	showMessageBox(message);
	//Notify(VISN_ERROR);
}


void SR2000DEV::showMessageBox(const char* message) {
	// Check if a message box is already displayed
	OutputDebugString(message);
}