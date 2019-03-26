#include <fstream>
#include <atomic>
#include <iostream>
#include <pthread.h>
#include <string>
#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/core/macro/codegen.hpp"

using namespace std;

class MyController : public oatpp::web::server::api::ApiController {
private:
	pthread_mutex_t mutex;

protected:
    MyController(const std::shared_ptr<ObjectMapper>& objectMapper)
        : oatpp::web::server::api::ApiController(objectMapper)
    {
        pthread_mutex_init(&mutex, NULL);
    }

public:
    static std::shared_ptr<MyController> createShared(const std::shared_ptr<ObjectMapper>& objectMapper = nullptr){
        return std::shared_ptr<MyController>(new MyController(objectMapper));
    }

    /**
     *  Begin ENDPOINTs generation ('ApiController' codegen)
     */
#include OATPP_CODEGEN_BEGIN(ApiController)


	ENDPOINT("GET", "/FScounter", fscounter) {
        int i=0;
        pthread_mutex_lock(&mutex);
        std::ifstream myfile;
        myfile.open("counter.txt");
        if (myfile.is_open())
        {
            std::string line;
            while ( std::getline(myfile,line) )
            {
                i =  std::stoi(line);
            }
        }
        else
        {
            std::cout << "Unable to open file";
        }
        i++;
        std::ofstream myfile2;
        myfile2.open("counter.txt");
        if (myfile2.is_open())
        {
            myfile2 << i;
            myfile2.close();
        }
        else
        {
            std::cout << "Unable to open file";
        }
        pthread_mutex_unlock(&mutex);

        std::string s = std::to_string(i);
        char const *pchar = s.c_str();
        auto response = createResponse(Status::CODE_200, pchar);
        response->putHeader(Header::CONTENT_TYPE, "text/html");
        return response;
    }

    ENDPOINT("GET", "/", root) {
        return createResponse(Status::CODE_200, "Hello World!");
    }

    ENDPOINT("GET", "/MEMcounter", memcounter) {
        static std::atomic<unsigned long long> i{0};
        i++;
        char s[20];
        sprintf(s, "%llu", i.load());
        return createResponse(Status::CODE_200, s);
    }

    /* put your endpoints here */

    /**
     *  Finish ENDPOINTs generation ('ApiController' codegen)
     */
#include OATPP_CODEGEN_END(ApiController)

};
