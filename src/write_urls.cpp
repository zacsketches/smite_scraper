#include <regex>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

int main (int argc, char  *argv[])
{
	//open the file and read all lines into a vector	
	ifstream ifs;
	vector<string> lines;
	ifs.open("god_lines.txt", ios_base::in);
	if(!ifs) {
		cout<<"unable to open god_lines.txt\n";
		return 1;
	}
	string line;
	getline(ifs, line);
	while(ifs.good()){
		lines.push_back(line);
		getline(ifs, line);
	}
	
		
	//define the regex that matches the desired pattern
	string pattern =".*(http.*/)\".*";
	regex pat (pattern);
	smatch matches;		//static implementation for string matches
	
	//iterate over the file writing the regex results to an output file
	vector<string> urls;
	for(int i = 0; i < lines.size(); ++i) {
		lines[i].erase(remove(lines[i].begin(), lines[i].end(), ' '), lines[i].end());
		string strip = lines[i];
		cout<<"line "<<i<<": "<<strip<<endl;
		if(regex_match(strip, matches, pat) ) {
			urls.push_back(matches[1]);
		}
	}
	
	
	for(int i = 0; i < urls.size(); ++i) {
		cout<<"urls "<<i<<": "<<urls[i]<<endl;
	}
		
	return 0;
}
