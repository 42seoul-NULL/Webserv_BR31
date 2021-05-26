#include "webserv.hpp"

Resource::Resource(int fd, std::string& raw_data, Client *client, e_direction direction, e_nextcall next_call, int response_error_num) : raw_data(raw_data), client(client), direction(direction), next_call(next_call), response_error_num(response_error_num)
{
	this->fd = fd;
	this->pid = -1;
	this->type = FD_RESOURCE;
	this->is_seeked = false;
	this->write_index = 0;
	this->unlink_path.clear();
}

Resource::~Resource()
{

}

///////////////////////// method /////////////////////////
e_resource_is_ready_status Resource::isReady()
{
	int 	status;

	if (pid == -1) // 자식이 없다.
	{
		return (READY);
	}
	else
	{
		//기다리는 PID가 종료되지 않아서 즉시 종료 상태를 회수 할 수 없는 상황에서 호출자는 차단되지 않고 반환값으로 0을 받음
		if (waitpid(pid, &status, WNOHANG) == 0) // 자식이 종료되지 않았다!.
			return (NOT_YET); //
		else
		{
			if (WIFEXITED(status) == 0) // 자식이 비정상으로 종료되었다!
				return (CGI_CRASH);
			else
			{
				if (this->is_seeked == false)
				{
					lseek(this->fd, 0, SEEK_SET); // 자식이 쓴것이기 때문에 가장 앞으로 다시 이동.
					this->is_seeked = true;
				}
				return (READY); // 자식이 정상 종료 되었다!
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
		client->getResponse().makeResponse();
	else
	{
		this->client->setStatus(RESPONSE_MAKE_DONE);
		//client->getResponse().makeErrorResponse(this->response_error_num); // 다시 불러주지 않는다.
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

void		Resource::setPid(int pid)
{
	this->pid = pid;
}

void		Resource::setUnlinkPath(const std::string &unlink_path)
{
	this->unlink_path = unlink_path;
}

void		Resource::setWriteIndex(size_t write_index)
{
	this->write_index = write_index;
}

void		Resource::setClient(Client *client)
{
	this->client = client;
}

//////////////////////// getter ////////////////////////
int			Resource::getPid()
{
	return (this->pid);
}

const std::string &Resource::getUnlinkPath()
{
	return (this->unlink_path);
}

std::string &Resource::getRawData()
{
	return (this->raw_data);
}

Client		*Resource::getClient()
{
	return (this->client);
}

size_t		Resource::getWriteIndex()
{
	return (this->write_index);
}