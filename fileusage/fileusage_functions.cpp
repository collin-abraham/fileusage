/* fileusage_functions.cpp
* Collin Abraham
* 2022-07-22
* Originally wrote in 2017, updated in 2022 for github push
* 
* Contains all needed functions to iterate through directories 
*/
#include "fileusage_header.h"

using namespace std;
using extension_map = std::map<std::string, std::pair<unsigned, uintmax_t>>;
using filesize_map = std::map<uintmax_t, std::pair<std::string, unsigned>>;

/*
accepts: a path object, a reference to the map storing file details, a regex object
returns: nothing, but populates the map with file details
purpose: this function recursively iterates through drectories to populate the map object 'files'

-> create an iterator of the same type as the referenced map
-> declare a recursive directory iterator and loop through the desired path 'f'
-> if we encounter a file that is a regular file and is not a directory
-> check if the extension of this file matches the regex argument
-> set the extension of the path as a string to avoid unneccessary path() calls
-> if the map is not empty
	-> use the map iterator to .find() the extension
	-> if the iterator didn't reach the end, it found a match
		-> push that match into the map, incrementing the file counter and file size counter
	-> if the iterator reached the end, there was no match
		-> push that new extension and details into the map
-> if the map is empty, insert it as a new key entry 
*/
void recursive_scan_regex(path const& f, extension_map& files, regex reg)
{
	map<string, pair<unsigned, uintmax_t>>::iterator iter;

	for (recursive_directory_iterator d(f), e; d != e; ++d)
	{
			if (is_regular_file(d->status()) && !is_directory(d->status()))
			{
				if (regex_match(d->path().extension().string(), reg))
				{
					string const extension = d->path().extension().string();

					if (!files.empty())
					{
						iter = files.find(extension);
						if (iter != files.end())
						{
							if (extension == iter->first)
							{
								iter->second.first += 1;
								iter->second.second += file_size(d->path());
							}

						}
						if (iter == files.end())
						{
							files.insert(make_pair(d->path().extension().string(), make_pair(1, file_size(d->path()))));
						}

					}

					if (files.empty())
					{
						files.insert(make_pair(d->path().extension().string(), make_pair(1, file_size(d->path()))));
					}
				}
			}
	}
}

/*
accepts: a path object, a reference to the map storing file details, a regex object
returns: nothing, but populates the map with file details
purpose: this function iterates through the path folder to populate the map object 'files' with extensions

This function does NOT iterate recursively and will only display extensions within the path folder provided
that are a regex_match with the regex object argument.

*** See the comments of recursive_scan_regex() for detailed logic breakdown ***
*/
void scan_regex(path const& f, extension_map& files, regex reg) {
	directory_iterator d(f);
	directory_iterator e;
	map<string, pair<unsigned, uintmax_t>>::iterator iter;

	while (d != e) {
			if (is_regular_file(d->status()) && !is_directory(d->status()))
			{

				if (regex_match(d->path().extension().string(), reg))
				{
					string const extension = d->path().extension().string();

					if (!files.empty())
					{
						iter = files.find(extension);
						if (iter != files.end())
						{
							if (extension == iter->first)
							{
								iter->second.first += 1;
								iter->second.second += file_size(d->path());
							}

						}
						if (iter == files.end())
						{
							files.insert(make_pair(d->path().extension().string(), make_pair(1, file_size(d->path()))));
						}

					}

					if (files.empty())
					{
						files.insert(make_pair(d->path().extension().string(), make_pair(1, file_size(d->path()))));
					}
				}
			}
	++d;
	}
}

/*
accepts: a path object, a reference to the map storing file details, a regex object
returns: nothing, but populates the map with file details
purpose: this function iterates through the directory to populate the map object 'files'

This function opeartes in the same way as recursive_scan_regex(), BUT does not take a regex object to compare with.
This function will display ALL extensions within the path folder, recursively. 

*** See the comments of recursive_scan_regex() for detailed logic breakdown ***
*/
void recursive_scan(path const& f, extension_map& files)
{
	map<string, pair<unsigned, uintmax_t>>::iterator iter;

		for (recursive_directory_iterator d(f), e; d != e; ++d)
		{
				if (is_regular_file(d->status()) && !is_directory(d->status()))
				{
					string const extension = d->path().extension().string();

					if (!files.empty())
					{
						iter = files.find(extension);
						if (iter != files.end())
						{
							if (extension == iter->first)
							{
								iter->second.first += 1;
								iter->second.second += file_size(d->path());
							}
						}
						if (iter == files.end())
						{
							files.insert(make_pair(d->path().extension().string(), make_pair(1, file_size(d->path()))));
						}
					}
					if (files.empty())
					{
						files.insert(make_pair(d->path().extension().string(), make_pair(1, file_size(d->path()))));
					}
				}

		}
}

/*
accepts: a path object, a reference to the map storing file details
returns: nothing, but populates the map with file details
purpose: this function iterates through the path folder to populate the map object 'files' with extensions

NOTE: This function does NOT iterate recursively and will only display extensions within the path folder provided.

*** See the comments of recursive_scan_regex() for detailed logic breakdown ***
*/
void scan(path const& f, extension_map& files) {
	directory_iterator d(f); 
	directory_iterator e;   
	map <string, pair<unsigned, uintmax_t>>::iterator iter = files.begin();

	while (d != e) {
			if (is_regular_file(d->status()) && !is_directory(d->status()))
			{
				string const extension = d->path().extension().string();

				if (!files.empty())
				{
					iter = files.find(extension);
					if (iter != files.end())
					{
						if (extension == iter->first)
						{
							iter->second.first += 1;
							iter->second.second += file_size(d->path());
						}

					}
					if (iter == files.end())
					{
						files.insert(make_pair(d->path().extension().string(), make_pair(1, file_size(d->path()))));
					}

				}

				if (files.empty())
				{
					files.insert(make_pair(d->path().extension().string(), make_pair(1, file_size(d->path()))));
				}
			}
			++d;
	}
}

/*
accepts: a templated type of T which will typically be a numberical variable
returns: a formatted string object
purpose: to format a numerical argument with locale, giving it thousands comma seperators and then returning the string stream as a string 

This function will only work properly with an appropriate location. If this program was intended to be run globally a different approach would be needed. 
*/
template<class T>
string comma_format(T value)
{
	stringstream ss;
	ss.imbue(locale(""));
	ss << fixed << value;
	return ss.str();
}

/* constructs a column line based on pre determined column widths, called by map_printer_* funcions */
void column_builder(int leftColWidth, int middleColWidth, int rightColWidth) {

	for (unsigned i = 0; i < leftColWidth; ++i)
		cout << "-";

	cout << " : ";

	for (unsigned i = 0; i < middleColWidth; ++i)
		cout << "-";

	cout << " : ";

	for (unsigned i = 0; i < rightColWidth; ++i)
		cout << "-";

	cout << endl;
}

/*
accepts: a reference to a map object to store file extensions, a bool variable
returns: nothing, but prints the contents of the map 
purpose: to determine how to correctly print the map to the screen late in the program

-> declare several variables to store dynamic differences in column widths for later use
-> declare variables to store the 'totals' of each component of the map after printing
-> examine the map through a ranged-for and examine the size of each element
	-> take the size, increase it by one and use that as the width for the dynamic columns
-> build the beginning formatting setup for the columns
-> preform a check to see if the 'reverseBool' variable was flagged
	-> begin looping through the map
		-> if it's not flagged print the contents, ordered alphabetically by extension
		-> if it's flagged print the contents, ordered alphabetically by extension, in reverse
		-> use the dynamic width variables decided from earlier to shape the columns depending on the longest values 
		-> keep track of the totals for total extensions, total file size count and total file number count
-> when the loop is finished, print the totals of each column
*/
void map_printer(extension_map& files, bool reverseCheck)
{
	// default widths for each column
	// column totals 
	unsigned leftColWidth = 0;
	unsigned middleColWidth = 5;
	unsigned rightColWidth = 0;
	unsigned leftTotal = 0;
	unsigned middleTotal = 0;
	uintmax_t rightTotal = 0; 

	// loop through the map and reset the widths to dynamically adapt
	for (const auto& x : files)
	{
		if (x.first.size() > leftColWidth)
			leftColWidth = (x.first.size() + 1);

		if (comma_format(x.second.first).size() > middleColWidth)
			middleColWidth = (comma_format(x.second.first).size() + 1);

		if (comma_format(x.second.second).size() > rightColWidth)
			rightColWidth = (comma_format(x.second.second).size() + 1);
	}

		const string space = " : ";
		cout << right << setw(leftColWidth) << "Ext" << space << right << setw(middleColWidth) << "#" << space << right << setw(rightColWidth) << "Total" << endl;

		// build the columns 
		column_builder(leftColWidth, middleColWidth, rightColWidth);

		// loop through the map
		// print the values within the map, formatted
		// increment the counting variables
		if (!reverseCheck)
		{
			for (const auto& x : files)
			{
				cout << right << setw(leftColWidth) << x.first << space << right << setw(middleColWidth) << x.second.first << space << right << setw(rightColWidth) << comma_format(x.second.second) << endl;
				++leftTotal;
				middleTotal += x.second.first;
				rightTotal += x.second.second;
			}
		}
		if (reverseCheck)
		{
			map< string, pair<unsigned, uintmax_t >>::const_reverse_iterator it;
			for (it = files.rbegin(); it != files.rend(); ++it)
			{
				cout << right << setw(leftColWidth) << it->first << space << right << setw(middleColWidth) << it->second.first << space << right << setw(rightColWidth) << comma_format(it->second.second) << endl;
				++leftTotal;
				middleTotal += it->second.first;
				rightTotal += it->second.second;
			}
		}

		// build the columns 
		column_builder(leftColWidth, middleColWidth, rightColWidth);

		cout << right << setw(leftColWidth) << leftTotal << space << right << setw(middleColWidth) << middleTotal << space << right << setw(rightColWidth) << comma_format(rightTotal) << endl << endl;
}

/*
accepts: a reference to a map object to store file extensions, a bool variable
returns: nothing, but prints the contents of the map
purpose: to determine how to correctly print the map to the screen late in the program

The logic used in this function is identical to the map_printer() function
... however a different approach was needed to print the values of the map by file size
Due to the nature of how maps store keys, the filesizes needed to be used as the key, instead of the strings in the typical map 
the function map_builder() populates the second map that is used for this function
*/
void map_printer_filesizes(filesize_map& files, bool reverseCheck)
{
	// default widths for each column
	// column totals 
	unsigned leftColWidth = 0;
	unsigned middleColWidth = 5;
	unsigned rightColWidth = 0;
	unsigned leftTotal = 0;
	unsigned middleTotal = 0;
	uintmax_t rightTotal = 0;

	// loop through the map and reset the widths to dynamically adapt 
	for (const auto& x : files)
	{
		if (x.second.first.size() > leftColWidth)
			leftColWidth = (x.second.first.size() + 1);

		if (comma_format(x.second.second).size() > middleColWidth)
			middleColWidth = (comma_format(x.second.second).size() + 1);

		if (comma_format(x.first).size() > rightColWidth)
			rightColWidth = (comma_format(x.first).size() + 1);
	}

	const string space = " : ";
	cout << right << setw(leftColWidth) << "Ext" << space << right << setw(middleColWidth) << "#" << space << right << setw(rightColWidth) << "Total" << endl;

	// build the columns 
	column_builder(leftColWidth, middleColWidth, rightColWidth);

	// loop through the map
	// print the values within the map, formatted
	// increment the counting variables
	if (!reverseCheck)
	{
		for (const auto& x : files)
		{
			cout << right << setw(leftColWidth) << x.second.first << space << right << setw(middleColWidth) << comma_format(x.second.second) << space << right << setw(rightColWidth) << comma_format(x.first) << endl;
			++leftTotal;
			middleTotal += x.second.second;
			rightTotal += x.first;
		}
	}
	if (reverseCheck)
	{
		map<uintmax_t, pair<string, unsigned>>::reverse_iterator it;
		for (it = files.rbegin(); it != files.rend(); ++it)
		{
			cout << right << setw(leftColWidth) << it->second.first << space << right << setw(middleColWidth) << comma_format(it->second.second) << space << right << setw(rightColWidth) << comma_format(it->first) << endl;
			++leftTotal;
			middleTotal += it->second.second;
			rightTotal += it->first;
		}
	}
	
	// build the columns 
	column_builder(leftColWidth, middleColWidth, rightColWidth);

	cout << right << setw(leftColWidth) << leftTotal << space << right << setw(middleColWidth) << middleTotal << space << right << setw(rightColWidth) << comma_format(rightTotal) << endl << endl;
}

/*
accepts: two different map objects that contain the same types of variables but one with a string as the key and the other as a uintmax_t as the key
returns: nothing 
purpose: populates a secondary map with the needed values

This function loops through the files map and pushes the values into the storageMap map
This was needed to satisfy the requirement of printing the values of the map sorted by file size
*/
void map_builder(filesize_map& storageMap, extension_map& files)
{
	for (const auto& s : files)
		storageMap.insert(make_pair(s.second.second, make_pair(s.first, s.second.first)));

	// clear up some memory 
	files.clear();

}

/*
accepts: nothing
returns: nothing
purpose: prints to the screen the details of how to operate the program properly
*/
void help_menu()
{
	cout << endl << "**********************************************************************************************************************";
	cout << endl << "HELP MENU" << endl;
	cout << "This program will display to the user:"
		<< endl << "\t-> the file types"
		<< endl << "\t-> how many of those files exist "
		<< endl << "\t-> what their sizes are (in bytes)";
	cout << endl << endl << "To use this program correctly, the desired arguments must be processed through the command line as follows:"
		<< endl << "\t-> fileusage.exe -switches path";
	cout << endl << endl << "If the user wants to input regex switches, the 'x' switch must be provided with the regex statement:"
		<< endl << "\t-> fileusage.exe -x \"regexexpression\" path"
		<< endl << endl << "If you attempt to use a regular expression and an extension switch and that regex matches the extension switch provided, " << endl
		<< "your regular expression will be removed." << endl
		<< "The following expression would disable the regex switch entirely: " << endl
		<< "\t-> fileusage.exe -srRcjx \"(.cpp|.java|.docx)\" ..\\..\\ " << endl; 

	cout << endl << endl << "The available switches to use are: "
		<< endl << "\t-> c: search for c++ files (c++ files extension: .c,.cpp,.h,.hpp,.hxx)"
		<< endl << "\t-> j: search for java files (java files extension: .java,.class)"
		<< endl << "\t-> m: search for multimedia files (multimedia files extension: .avi,.mkv,.mpeg,.mp4,.mp3)"
		<< endl << "\t-> x: search extensions using a regex expression " << R"(ie:  -x "\.(docx|xlsx)" << ")\""
		<< endl << "\t-> r: process all folders recursively."
		<< endl << "\t-> R: list the files in reverse order."
		<< endl << "\t-> s: sort the files by ascending file size or descending if the R switch is provided."
		<< endl << "\t-> h: shows the help usage for running the program";

	cout << endl << endl << "If you are intending to view ALL directories within a folder (recursively) you must provide the -r switch!"
		<< endl << "If you do not provide the recursive switch you will only view the file extensions within the path folder!";

	cout << endl << endl << "An example of the commands needed to view .cpp and .java files within the parent directory, recursively would be:"
		<< endl << "\t -> fileusage.exe -rcj .."; 

	cout << endl << endl << "To view the current folder containing fileusage.exe, provide a path of -> . ";
	cout << endl << "To view the parent directory of the folder containing fileusage.exe, provide a path of -> .. ";
	cout << endl << endl << "You may pass a literal path as well, such as ->" << R"("C:/User/Documents/Music")";
	cout << endl << "\t-> Your defined path must be sourrounded in double quotes(\") and use forward slashes (/) to work properly!";
	cout << endl << "**********************************************************************************************************************" << endl;
}
