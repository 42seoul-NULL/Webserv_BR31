#include "response.hpp"
#include "client.hpp"

void		Response::makeRedirectionResponse()
{
	int ret;
	initResponse();
    
    addFirstLine(this->location->getRedirectReturn());
    addDate();
    addServer();
    addLocation();
	
    this->client->setStatus(RESPONSE_MAKE_DONE);
    return ;
}
