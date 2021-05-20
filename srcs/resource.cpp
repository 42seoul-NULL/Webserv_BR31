#include "resource.hpp"

Resource::Resource(int fd, std::string& raw_data, Client *client, e_direction direction, e_nextcall next_call) : raw_data(raw_data), client(client), direction(direction), next_call(next_call)
{
	this->fd = fd;
	this->pid = -1;
	this->response_error_num = -1;
}

Resource::~Resource()
{

}

///////////////////////// method ///////////////////////// 
e_resource_is_ready_status Resource::isReady()
{
	int 	status;

	if (pid == -1) // 자식이 없다.
		return (READY);
	else
	{
		if (waitpid(pid, &status, WNOHANG) != 0) // 자식이 종료되지 않았다!.
			return (NOT_YET); //
		else
		{
			if (WIFEXITED(status) == 0) // 자식이 비정상으로 종료되었다!
			{
				return (CGI_CRASH);
			}
		}
	}
}

void		Resource::doNext()
{
	if (isFdToRawData())
		this->client->setStatus(FILE_READ_DONE);
	else
		this->client->setStatus(FILE_WRITE_DONE);

	if (this->next_call == MAKE_RESPONSE)
	{
		// client->getResponse().makeResponse()
	}		
	else
	{
		// client->getResponse().makeErrorResponse(this->response_error_num);
	}
}

bool		Resource::isFdToRawData()
{
	if (this->direction == FD_TO_RAW_DATA)
		return (true);
	else
		return (false);
}

bool		Resource::isRawDataToFd()
{
	if (this->direction == RAW_DATA_TO_FD)
		return (true);
	else
		return (false);
}


////////////////////////// setter ////////////////////////////
void		Resource::setResponseErrorNum(int response_error_num)
{
	this->response_error_num = response_error_num;
}

//////////////////////// getter ////////////////////////
std::string &Resource::getRawData()
{
	return (this->raw_data);
}

Client		*Resource::getClient()
{
	return (this->client);
}
