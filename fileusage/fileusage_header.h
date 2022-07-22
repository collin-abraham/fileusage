/* fileusage_header.cpp
* Collin Abraham
* 2022-07-22
* Originally wrote in 2017, updated in 2022 for github push
* 
* contians standard libraries used throughout
* function declarations
*/

#if !defined (GUARD_FILEUSAGE_HEADER_H)
#define GUARD_FILEUSAGE_HEADER_H

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING

#include<iostream>
#include<iomanip>
#include<vector>
#include<locale>
#include<regex>
#include<experimental/filesystem>	// originally written in a cpp11 compiler when this was still experimental 
#include<map>
#include<iterator>
#include<sstream>
#include<thread>
#include<stdexcept>


using namespace std::experimental::filesystem;

using extension_map = std::map<std::string, std::pair<unsigned, uintmax_t>>;
using filesize_map = std::map<uintmax_t, std::pair<std::string, unsigned>>;

//function signatures
void recursive_scan(path const& f, extension_map& files);
void recursive_scan_regex(path const& f, extension_map& files, std::regex reg);
void scan_regex(path const& f, extension_map& files, std::regex reg);
void scan(path const& f, extension_map& files);
void map_printer(extension_map& files, bool reverseCheck);
void map_printer_filesizes(filesize_map& files, bool reverseCheck);
void map_builder(filesize_map& storageMap, extension_map& files);
template <class T>
std::string comma_format(T value);
void help_menu();


#endif