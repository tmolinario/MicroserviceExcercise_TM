
#include <iostream>
#include <cpprest/http_listener.h>
#include <cpprest/filestream.h>
#include <cpprest/uri.h>
#include <cpprest/json.h>

using namespace utility; // Common utilities like string conversions
using namespace web; // Common features like URIs.
using namespace web::http; // Common HTTP functionality
using namespace web::http::experimental::listener; // HTTP listener
using namespace concurrency::streams; // Asynchronous streams
using namespace std;


void handle_post(http_request message){
	//cout << "Handle post: " << message.to_string() << endl;
	web::json::value jsonObject;
	try{
		message.extract_json()
			.then([&jsonObject](web::json::value jo){
				cout << "Val: " << jo.serialize() << endl; // Serialize transform jsonobject to a c++ string
				jsonObject = jo;
			});
	}
	catch (const std::exception & e) {
		printf("Error exception:%s\n", e.what());
	}
	jsonObject[U("Json Received and processed correctly")] = web::json::value::string(U("C"));
	message.reply(status_codes::OK,jsonObject);
}

void send_json() {

}


int main(int argc, char* argv[])
{
	http_listener 
		listener(U("http://localhost:8080/user"));

	//std::function< void(http_request) > handle_post; // in listener.support should be this parameter or simply "handle_post"

	listener.support(methods::POST, handle_post); // Add an handler for HTTP request POST (handle_post is a function pointer probably to void handle_post(http_request message))
	try{
		listener.open() 											// Asynchronously open the listener (create a task), i.e. start accepting requests
			.then([](){printf("Wait for POST request\n");});	// Use callback in order to call functions. then() make sure that those functions are assigned to the same thread.
			
		while(true);
	}
	catch (const std::exception & e) {
		printf("Error exception:%s\n", e.what());
	}
	return 0;
}