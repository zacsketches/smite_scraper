/*
* write a file of URLs pointing to the images in haydens game
*/
#include "smite.h"

using namespace std;

const string smite_url = "http://www.smitegame.com/gods/";
const string temp_file = "tmp.txt";

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
