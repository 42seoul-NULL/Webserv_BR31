#include "webserv.hpp"

void		Response::makeRedirectionResponse()
{
	this->raw_response.clear();
    
    addFirstLine(this->location->getRedirectReturn());
    addDate();
    addServer();
    addLocation();
	
    this->client->setStatus(RESPONSE_MAKE_DONE);
    this->is_disconnect_immediately = true;
    return ;
}
