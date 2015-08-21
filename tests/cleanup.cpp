/*
* run this test under valgrind to ensure there no leaks on getting and
* releasing the curl resources.
* 
* When run with bad urls so no download occure there are no leak, but when
* run with real urls there are leaks.
*/
#include "smite.h"

using namespace std;

const string smite_url = "http://www.smitegame.com/gods/";
const string image_url = "http://hzweb.hi-rezgame.net/smite-web/wp-content/uploads/2015/07/ahpuch_header_fallback.jpg";

const string test_image_filename = "./tests/test_image.jpg";
const string temp_file = "tmp.txt";

int main (int argc, char  *argv[])
{
	//get curl resources set up
	CURL* curl;
	curl_global_init(CURL_GLOBAL_DEFAULT);
	curl = curl_easy_init();
	
	vector<string> test_vec;
	if(load_vector(curl, smite_url, test_vec))
		cout<<"Test vector was loaded by curl\n";
	else
		cout<<"Unable to load test vector!";
	
	if(save_image(curl, image_url, test_image_filename))
		cout<<"Test image was saved by curl\n";
	else
		cout<<"Unable to save the test image!\n";
	
	cleanup(curl);
	return 0;
}
