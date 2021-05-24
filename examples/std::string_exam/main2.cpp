#include <string>
#include <iostream>

int     main()
{
    // size_t idx;

    // std::string cgi_extension = ".bla";
    // std::string request_uri = "www.anisef.com/abcdef.bla/asefaef/efefe?query_string!!";

	// std::string path_info = request_uri.substr(request_uri.find(cgi_extension) + (cgi_extension.size()));
	// std::string query_string;
    // std::string path_translated = request_uri.substr(0, request_uri.find(cgi_extension) + cgi_extension.size());

	// idx = path_info.find('?');
	// if (idx != std::string::npos)
	// {
	// 	query_string = path_info.substr( idx + 1);
	// 	path_info = path_info.substr(0, idx);
	// }
    // std::cout << path_info << std::endl;
    // std::cout << query_string << std::endl;
    // std::cout << path_translated << std::endl;

	std::string temp = "\r\n";
	std::cout << temp.size() << std::endl;
}