#include "webserv.hpp"
// 아마도 명령을 성공적으로 수행할 것 같으나 아직은 실행하지 않은 경우 202 (Accepted) 상태 코드.
// 명령을 수행했고 더 이상 제공할 정보가 없는 경우 204 (No Content) 상태 코드.
// 명령을 수행했고 응답 메시지가 이후의 상태를 설명하는 경우 200 (OK) 상태 코드.

// DELETE /api/book/122 - The server successfully processed the request, but is not returning any content
// 204 No Content
// DELETE /api/book/122 - Resource does not exist
// 404 Not Found
// DELETE /api/book/122 - Resource already deleted
// 410 Gone
// DELETE /api/book/122 - Users does not have permission
// 403 Forbidden
// DELETE /api/book/122 - Method Not Allowed

// 405 Method Not Allowed
// DELETE /api/book/122 - Conflict (User can resolve the conflict and delete)

// 409 Conflict

// 디렉토리는 못지운다. / 리소스 존재하면 204 때리면서 성공
// 실패하면 404,

void		Response::makeDeleteResponse()
{
    if (this->client->getStatus() == RESPONSE_MAKING)
    {
		if (isExist(this->resource_path)) // 존재하고 폴더가 아니면
		{
			if (isDirectory(this->resource_path))
				makeErrorResponse(403);
			else
			{
				int unlink_ret;

				unlink_ret = unlink(this->resource_path.c_str());
				if (unlink_ret < 0)
					makeErrorResponse(403);
				addFirstLine(204);
				addAllow();
				addServer();
				addDate();
				addContentLanguage();
				addContentLength(0);
				addEmptyline();
			}
		}
		else // 존재하지 않으면
			makeErrorResponse(404);
		this->client->setStatus(RESPONSE_MAKE_DONE);
    }
}
