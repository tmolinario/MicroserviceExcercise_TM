
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

#include <iomanip>
#include <nlohmann/json-schema.hpp>

using nlohmann::json;
using nlohmann::json_schema::json_validator;

// Create json_schema for the validation
static nlohmann::json person_schema = R"(			
{
  "title": "PersonSchema",
  "description": "Validation for users",
  "type": "object",
  "properties": {
    "username": {
      "description": "Username",
      "type": "string",
	  "minLength": 2,
	  "maxLength": 20
    },
    "firstname": {
      "description": "Firstame",
      "type": "string",
	  "minLength": 2,
	  "maxLength": 20
    },
    "lastname": {
      "description": "Lastname",
      "type": "string",
	  "minLength": 2,
	  "maxLength": 20
    },
    "email": {
      "description": "Email",
      "type": "string",
	  "minLength": 10,
	  "maxLength": 40
    },
    "password": {
      "description": "Password",
      "type": "string",
	  "minLength": 8,
	  "maxLength": 25
    },
    "age": {
      "description": "Age",
      "type": "integer",
      "minimum": 1,
      "maximum": 150
    }
  },
  "required": [
    "username",
    "firstname",
    "lastname",
    "email",
    "password",
    "age"
  ]
}
)"_json;


nlohmann::json web_to_nlhomann (web::json::value wj){
	auto username = wj.at(U("username")).as_string();
	auto firstname = wj.at(U("firstname")).as_string();
	auto lastname = wj.at(U("lastname")).as_string();
	auto email = wj.at(U("email")).as_string();
	auto password = wj.at(U("password")).as_string();
	auto age = wj.at(U("age")).as_integer();

	nlohmann::json nj = {{"username", username}, {"firstname", firstname}, {"lastname", lastname}, 
	{"email", email}, {"password", password}, {"age", age}};

	return nj;
}

void handle_post(http_request request){
	
	request.extract_json()											    // Extracts the body of the request message into a json value
		.then([&request](pplx::task<web::json::value> task){
			web::json::value web_json;		
			web_json = task.get();

			nlohmann::json_schema::json_validator validator; 			// create validator
			validator.set_root_schema(person_schema); 					// insert root-schema

			try {
				nlohmann::json wtn_json = web_to_nlhomann(web_json);  	// In order to validate json, validate() need a nlohmann instead of a web
				validator.validate(wtn_json); 							// validate the json - uses the default throwing error-handler
				std::cout << "Validation succeeded\n";
			} catch (const std::exception &e) {
				std::cerr << "Validation failed, here is why: " << e.what() << "\n";
			}	

		});

}

int main(int argc, char* argv[])
{
	http_listener listener(U("http://localhost:8080/user"));
	listener.support(methods::POST, handle_post); 				// Add an handler for HTTP request POST (handle_post is a function pointer

	try{
		listener.open() 										// Asynchronously open the listener (create a task), i.e. start accepting requests
			.then([](){cout << "Wait for POST request\n";});	// Use callback in order to call functions. then() make sure that those functions are assigned to the same thread.

		std::mutex mtx;											// Use double mutex block to lock the execution
		mtx.lock();
		mtx.lock();
	}
	catch (const std::exception & e) {
		printf("Error exception:%s\n", e.what());
	}
	return 0;
}