#include "webserv.hpp"

void		Response::makeOptionsResponse()
{
	if (this->client->getStatus() == RESPONSE_MAKING)
	{
		addFirstLine(200);
		addAllow();
		addServer();
		addDate();
		addContentLanguage();
		addContentLength(0);
		addEmptyline();
		this->client->setStatus(RESPONSE_MAKE_DONE);
	}
}
