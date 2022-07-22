/* fileusage_main.cpp
* Collin Abraham
* 2022-07-22
* Originally wrote in 2017, updated in 2022 for github push
*/

#include"fileusage_header.h"

using namespace std;

int main(int argc, char* argv[])
{

	// common objects 
	extension_map files;															// extension, number of files, file sizes
	path directoryPath;																// path object

	// string analysis flags
	string switchArgs = "";															// to store the string of switches
	bool cppFlag = false;															// flags for switches
	bool javaFlag = false;
	bool mediaFlag = false;
	bool regexFlag = false;
	bool recursiveFlag = false;
	bool fileSizeSortFlag = false;
	bool reverseFlag = false;
	bool helpFlag = false;
	bool hyphenFlag = false;
	
	// regex storage/building 
	vector<string> regexMatches;													// to store pushed arrays for error checking
	const string cppArray[5] = { ".cpp",".c",".h",".hpp",".hxx" };						// arrays pushed into regexMatches if the flags are triggered, for error checking
	const string javaArray[2] = { ".java",".class" };
	const string mediaArray[5] = { ".avi",".mp3",".mp4",".mpeg",".mkv" };
	const regex reg_cpp(".*\\.cpp$|.*\\.c$|.*\\.h$|.*\\.hpp$|.*\\.hxx$");					// default regex extensions
	const regex reg_java(".*\\.java$|.*\\.class$");
	const regex reg_media(".*\\.avi$|.*\\.mkv$|.*\\.mpeg$|.*\\.mp4$|.*\\.mp3$");
	regex reg_userReg("");															// will get .assign() depending if argc == 4

	// prompt user and complete early error checking
	cout << "fileusage.exe, (C) Collin Little August, 2017" << endl << endl;

	if (argc > 4 || argc < 2)
	{
		cerr << "-> ERROR - Incorrect number of arguments. Proper usage is:" << endl
			<< "\t-> fileusage.exe -switchcharacters path" << endl
			<< "\t-> fileusage.exe -switchcharacters regexExpression path" << endl << endl
			<< "-> to display the help menu, run the program with the h switch as follows:" << endl
			<< "\t-> fileusage.exe -h" << endl;
		return EXIT_FAILURE;
	}

	
	if (argc == 4)
	{
		reg_userReg.assign(argv[2], regex::ECMAScript);
		directoryPath = (argv[3]);
		switchArgs = (argv[1]);
	}

	if (argc == 3)
	{
		directoryPath = (argv[2]);
		switchArgs = (argv[1]);
	}

	// no flags given, scan of current folder only 
	if (argc == 2)
	{
		if (switchArgs == "-h")
		{
			help_menu();
			return EXIT_SUCCESS;
		}
		else 
			directoryPath = (argv[1]);
	}

	// check that the first character of the string IS a hyphen
	// if there were flags given 
	if (argc > 2 && switchArgs[0] != '-')
	{
		cerr << "-> ERROR - The first character of the switches MUST be a hyphen '-' " << endl
			<< "-> If you are intending to view the path folder with no switches you still must provide a hyphen" << endl << endl
			<< "-> to display the help menu, run the program with the h switch as follows:" << endl
			<< "\t-> fileusage.exe -h" << endl;
		return EXIT_FAILURE;
	}

	// if flags were given, switch args will not be empty - so have a look inside:
	// loop through the switchArgs string and check each character
	// switches to set bool flags were used in an effort to cut down on user error
	//		if, for example... the user enters fileusage.exe -rrrRrccjjmR ..\..\
	//		it will still only flag the recursive, reverse, cpp, java, media and reverse flags since they remain true regardless
	// when the cpp, java or media flags are triggered then an array of their extensions is pushed into 
	// a vector to store them for user regex inputted error checking
	if (!switchArgs.empty())
	{
		for (char const& c : switchArgs)
		{
			switch (c)
			{
			case '-':
				hyphenFlag = true;	// this a dummy flag added in to satisfy the switch
				break;
			case 'c':
				cppFlag = true;
				regexMatches.insert(regexMatches.end(), cppArray, cppArray + (sizeof(cppArray) / sizeof(cppArray[0])));
				break;
			case 'j':
				javaFlag = true;
				regexMatches.insert(regexMatches.end(), javaArray, javaArray + (sizeof(javaArray) / sizeof(javaArray[0])));
				break;
			case 'm':
				mediaFlag = true;
				regexMatches.insert(regexMatches.end(), mediaArray, mediaArray + (sizeof(mediaArray) / sizeof(mediaArray[0])));
				break;
			case 'x':
				regexFlag = true;
				break;
			case 'r':
				recursiveFlag = true;
				break;
			case 'R':
				reverseFlag = true;
				break;
			case 's':
				fileSizeSortFlag = true;
				break;
			case 'h':
				helpFlag = true;
				break;
			default:
				cerr << "-> ERROR - The character '" << c << "' is an incorrect switch argument" << endl << endl
					<< "-> to display the help menu, run the program with the h switch as follows:" << endl
					<< "\t-> fileusage.exe -h" << endl;
				return EXIT_FAILURE;
			}
		}
	}
	


	// Check that the path is good
	if (!exists(directoryPath))
	{
		cerr << "-> ERROR - Path has not been provided or does not exist!" << endl
			<< "-> You must enter a path!" << endl
			<< "-> If you are intending to scan the working directory of fileusage.exe, provide a path of -> . " << endl
			<< "-> fileusage.exe working directory is: " << canonical(directoryPath) << endl << endl
			<< "-> to display the help menu, run the program with the h switch as follows:" << endl
			<< "\t-> fileusage.exe -h" << endl;
		return EXIT_FAILURE;
	}

	// prompt the user that we are scanning the directory
	cout << "Scanning... " << canonical(directoryPath);
	cout << endl;

	// Check if the user inputted a regex command that is already a triggered expression through c, j or m switches
	// compare each inner regex with the user regex 
	// if a match is found.. reset the regex flag to false
	// this is done to prevent duplicate results within the result set
	if (regexFlag)
	{
		for (const auto& x : regexMatches)
			if (regex_match(x, reg_userReg))
				regexFlag = false;
	}
	
	// contained in a try/catch just to be careful.. sometimes calling file_size on protected files will throw
	// an exception, stop exectuion and crash the program. Especially on C:/Windows
	try
	{
		// Are we processing recursively? 
		if (recursiveFlag)
		{
			// if none of the specific extensions were picked
			// just process the folder as usual recursively
			if (!cppFlag && !javaFlag && !mediaFlag && !regexFlag)
				recursive_scan(directoryPath, files);

			// check for the extension flags
			// if they're checked, push them into the files map
			if (cppFlag)
				recursive_scan_regex(directoryPath, files, reg_cpp);
			if (javaFlag)
				recursive_scan_regex(directoryPath, files, reg_java);
			if (mediaFlag)
				recursive_scan_regex(directoryPath, files, reg_media);
			if (regexFlag)
				recursive_scan_regex(directoryPath, files, reg_userReg);
		}

		// If the user did NOT choose to scan recursively
		if (!recursiveFlag)
		{
			// non recursively process
			if (!cppFlag && !javaFlag && !mediaFlag && !regexFlag)
				scan(directoryPath, files);

			if (cppFlag)
				scan_regex(directoryPath, files, reg_cpp);
			if (javaFlag)
				scan_regex(directoryPath, files, reg_java);
			if (mediaFlag)
				scan_regex(directoryPath, files, reg_media);
			if (regexFlag)
				scan_regex(directoryPath, files, reg_userReg);
		}
	}
	catch (exception& e)
	{
		cerr << "\nERROR: Exception caught -> " << e.what() << "\n\n";
	}



	// Check if there was nothing pushed into the map
	if (files.empty())
	{
		cerr << "-> ERROR - The scan returned no extensions. Check your switch arguments and/or path." << endl 
			<< "-> To view all directories within your path you must use the recursive switch -> '-r' "<< endl
			<< "-> to display the help menu, run the program with the h switch as follows:" << endl
			<< "\t-> fileusage.exe -h" << endl;
		return EXIT_FAILURE;

	}
	
	// if the user wants the values sorted by file size
	// populate the storageMap that will use the file sizes as a key instead of the strings
	// the bool 'reverseFlag' determines if it is printed in reverse or not for both following if statements
	map<uintmax_t, pair<string, unsigned>> storageMap;
	if (fileSizeSortFlag)
	{
		map_builder(storageMap,files);
		map_printer_filesizes(storageMap, reverseFlag);
	}

	// if the user wants the file sorted by extension name
	if (!fileSizeSortFlag)
		map_printer(files,reverseFlag);


	// check if the help flag was triggered, if true call help_menu() after printing results to the screen 
	if (helpFlag)
		help_menu();

	return EXIT_SUCCESS; 
}


