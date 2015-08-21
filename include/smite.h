#ifndef SMITE_H
#define SMITE_H 20150821

/*
* provide facilities to hangle curling urls and storing them into files
* or vectors based on the results of a few regex's.
* 
* intended use here is in the smite scraper
*/

#include <iostream>
#include <cstdio>
#include <string>
#include <vector>
#include <fstream>
#include <regex>
#include <curl/curl.h>

extern const std::string temp_file;

using namespace std;

inline void cleanup(CURL* curl) {
	curl_easy_cleanup(curl);
	curl_global_cleanup();
}

inline size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    size_t written = fwrite(ptr, size, nmemb, stream);
    return written;
}

inline bool save_image(CURL* curl, const string url, const string filename){
	/*
	* curl the passed url and load the lines into the vector
	*/
	//get resource handles
	CURLcode res;
    FILE *fp;
	fp = fopen(filename.c_str(), "wb");
	if(!fp) {
		cout<<"Unable to open the image file: "<<filename<<endl;
		cleanup(curl);
		return false;
	}
	
	//set curl options
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
	
	//run the curl operation
	res = curl_easy_perform(curl);
	if(res != CURLE_OK){
		fprintf(stderr, "curl_easy_perform() failed: %s\n"
			, curl_easy_strerror(res) );
		cleanup(curl);
		fclose(fp);
	}
	
	//clean up
	fclose(fp);
	
	//res indicates the transfer was successful
	return (res==CURLE_OK) ? true : false;
	
}

inline bool load_vector(CURL* curl, const string url, vector<string>& vec ){
	/*
	* curl the passed url and load the lines into the vector
	*/
	//get resource handles
	CURLcode res;
    FILE *fp;
	fp = fopen(temp_file.c_str(), "wb");
	if(!fp) {
		cout<<"Unable to open the temp file\n";
		cleanup(curl);
		return false;
	}
	
	//set curl options
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
	
	//run the curl operation
	res = curl_easy_perform(curl);
	if(res != CURLE_OK){
		fprintf(stderr, "curl_easy_perform() failed: %s\n"
			, curl_easy_strerror(res) );
		cleanup(curl);
		fclose(fp);
	}
	
	//read the file back into the vector
	vec.clear();
	ifstream ifs(temp_file.c_str());
	string line;
	while(getline(ifs, line)){
		vec.push_back(line);
	}
	
	//clearn up & check to make sure we reached eof 
	fclose(fp);
	//ifs is closed as this functionleaves scope
	return (ifs.eof() && vec.size()>0) ? true : false;
}

inline bool load_pantheon(const vector<string>& main_page, vector<string>& pantheon){
	/*
	* run the regex against each line of the main_page and push back all the
	* lines that match
	*/
	string pattern = "pantheon";
	regex pat(pattern);
	smatch matches;
	
	bool result = false;
	
	//iterate over the main_page vector
	string line;
	for(int i = 0; i < main_page.size(); ++i) {
		line = main_page[i];
		if(regex_search(line, matches, pat)) {
			pantheon.push_back(line);
			result = true;
		}
	}
	return result;
}

inline bool load_page_urls(const vector<string>& pantheon, vector<string>& urls){
	/*
	* run the regex against each line of the pantheon and push back all the
	* lines that match
	*/
	string pattern = "http://.*/gods/.*/";
	regex pat(pattern);
	smatch matches;
	
	bool result = false;
	
	//iterate over the main_page vector
	string line;
	for(int i = 0; i < pantheon.size(); ++i) {
		line = pantheon[i];
		if(regex_search(line, matches, pat)) {
			urls.push_back(matches[0]);
			result = true;
		}
	}
	return result;
}

inline bool find_god_name(const string url, string& name) {
	/*
	* run the regex against each url of the pantheon and return the god name
	*/
	string pattern = ".*/gods/(.*)/";
	regex pat(pattern);
	smatch matches;
	
	bool result = false;
	
	if(regex_search(url, matches, pat)) {
		name = matches[1];
		result = true;
	}
	
	return result;
}

inline bool find_image_url(const vector<string>& page_lines, string& image_url){
	/*
	* iterate over the page lines to find the image_url
	*/
	string pattern = "http://hzweb.*_header_fallback.jpg";
	regex pat(pattern);
	smatch matches;
	
	bool result = false;
	for(int i = 0; i < page_lines.size(); ++i) {
		string line = page_lines[i];
		if(regex_search(line, matches, pat)) {
			image_url = matches[0];
			result = true;
			break;
		}		
	}
	
	return result;
	
}

inline void show_vector(const vector<string> vec) {
	for(int i = 0; i < vec.size(); ++i) {
		cout<<vec[i]<<endl;
	}
}

#endif