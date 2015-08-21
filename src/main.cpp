/*
* write a file of URLs pointing to the images in haydens game
*/

#include <iostream>
#include <cstdio>
#include <string>
#include <vector>
#include <fstream>
#include <regex>
#include <curl/curl.h>

// compile with:
//		clang++ -std=c++11 main.cpp $(curl-config --libs)

using namespace std;

const string smite_url = "http://www.smitegame.com/gods/";
const string temp_file = "tmp.txt";

void cleanup(CURL* curl) {
	curl_easy_cleanup(curl);
	curl_global_cleanup();
}

size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    size_t written = fwrite(ptr, size, nmemb, stream);
    return written;
}

bool save_image(CURL* curl, const string url, const string filename){
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

bool load_vector(CURL* curl, const string url, vector<string>& vec ){
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

bool load_pantheon(const vector<string>& main_page, vector<string>& pantheon){
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

bool load_page_urls(const vector<string>& pantheon, vector<string>& urls){
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

bool find_god_name(const string url, string& name) {
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

bool find_image_url(const vector<string>& page_lines, string& image_url){
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

void show_vector(const vector<string> vec) {
	for(int i = 0; i < vec.size(); ++i) {
		cout<<vec[i]<<endl;
	}
}

int main (int argc, char  *argv[])
{
	//get curl resources set up
	CURL* curl;
	curl_global_init(CURL_GLOBAL_DEFAULT);
	curl = curl_easy_init();
	if(!curl) {
		fprintf(stderr, "Unable to initialize curl\n");
		return 1;
	}
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
	
	// curl the main smite page
	vector<string> main_page;
	cout<<"Attempting to load the gods page on smite.com\n";
	if (load_vector(curl, smite_url, main_page))
		cout<<"\tLoaded smite main page\n";
	else cout<<"\tLoad failed!\n";

	// create a vector<string> of the pantheon lines in main page
	vector<string> pantheon;
	cout<<"Attempting to load the pantheon of gods from the main page\n";
	if(load_pantheon(main_page, pantheon) && main_page.size()>0)
		cout<<"\tLoaded the pantheon of gods\n";
	else cout<<"\tLoad failed!\n";
	
	// find the actual url in each pantheon line
	vector<string> page_urls;
	cout<<"Attempting to load the page urls for each god\n";
	if(load_page_urls(pantheon, page_urls) && pantheon.size()>0)
		cout<<"\tLoaded the URLs for each god\n";
	else cout<<"\tLoad failed!\n";
	
	// iterate through the page_urls:
	//		1. curl each page into a vector
	//		2. find the image url in the page
	//		3. add the url to the list of urls
	vector<string> image_urls;
	for(int i = 0; i < page_urls.size(); ++i) {
		vector<string> god_page_lines;			//get a new vector each iteration
		string url = page_urls[i];
		string god_name = "";
		if (find_god_name(url, god_name)){		
			cout<<"Attempting to load the page for "<<god_name<<endl;	
			if (load_vector(curl, url, god_page_lines)) {
				cout<<"\tloaded the god page for "<<god_name<<endl;;
				show_vector(god_page_lines);
				string image_url;
				if(find_image_url(god_page_lines, image_url)){
					cout<<"\timage url is "<<image_url<<endl;
					image_urls.push_back(image_url);
				}
			}
		}
	}
	
	// curl the images and save them to the images folder
	for(int i = 0; i < image_urls.size(); ++i) {
		string url = image_urls[i];
		string god_name = "";
		if (find_god_name(url, god_name)){
			string filename = "../images/"+god_name;
			if(save_image(curl, url, filename))
				cout<<"Saved image for "<<god_name<<endl;
		}
	}
	
	cleanup(curl);
	return 0;
}
