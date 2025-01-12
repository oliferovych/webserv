#include "HTTPrequest.hpp"

int main(void)
{
	std::vector<char> post_message_chunked = {
        'P', 'O', 'S', 'T', ' ', '/', 'u', 'p', 'l', 'o', 'a', 'd', ' ', 'H', 'T', 'T', 'P', '/', '1', '.', '1', '\r', '\n',
        'H', 'o', 's', 't', ':', ' ', 'e', 'x', 'a', 'm', 'p', 'l', 'e', '.', 'c', 'o', 'm', '\r', '\n',
        'T', 'r', 'a', 'n', 's', 'f', 'e', 'r', '-', 'E', 'n', 'c', 'o', 'd', 'i', 'n', 'g', ':', ' ', 'c', 'h', 'u', 'n', 'k', 'e', 'd', '\r', '\n',
        'C', 'o', 'n', 't', 'e', 'n', 't', '-', 'T', 'y', 'p', 'e', ':', ' ', 'a', 'p', 'p', 'l', 'i', 'c', 'a', 't', 'i', 'o', 'n', '/', 'j', 's', 'o', 'n', '\r', '\n',
        '\r', '\n',
        '1', '2', '\r', '\n', 
        'n', 'a', 'm', 'e', ':', ' ', 'J', 'o', 'h', 'n', ',', ' ', '\r', '\n',
        '9', '\r', '\n', 
        'a', 'g', 'e', ':', ' ', '3', '0', ',', ' ', '\r', '\n',
        '1', '4', '\r', '\n', 
        'c', 'i', 't', 'y', ':', ' ', 'N', 'e', 'w', ' ', 'Y', 'o', 'r', 'k', '\r', '\n',
        '0', '\r', '\n',
        '\r', '\n'
    };

	std::vector<char> post_message_normal = {
    'P', 'O', 'S', 'T', ' ', '/', 'u', 'p', 'l', 'o', 'a', 'd', ' ', 'H', 'T', 'T', 'P', '/', '1', '.', '1', '\r', '\n',
    'H', 'o', 's', 't', ':', ' ', 'e', 'x', 'a', 'm', 'p', 'l', 'e', '.', 'c', 'o', 'm', '\r', '\n',
    'C', 'o', 'n', 't', 'e', 'n', 't', '-', 'T', 'y', 'p', 'e', ':', ' ', 'a', 'p', 'p', 'l', 'i', 'c', 'a', 't', 'i', 'o', 'n', '/', 'j', 's', 'o', 'n', '\r', '\n',
    'C', 'o', 'n', 't', 'e', 'n', 't', '-', 'L', 'e', 'n', 'g', 't', 'h', ':', ' ', '3', '5', '\r', '\n',
    '\r', '\n',
    'n', 'a', 'm', 'e', ':', ' ', 'J', 'o', 'h', 'n', ',', ' ',
    'a', 'g', 'e', ':', ' ', '3', '0', ',', ' ',
    'c', 'i', 't', 'y', ':', ' ', 'N', 'e', 'w', ' ', 'Y', 'o', 'r', 'k'
	};

    std::vector<char> get_message_normal = {
        'G', 'E', 'T', ' ', '/', 'i', 'n', 'd', 'e', 'x', '.', 'h', 't', 'm', 'l', ' ', 'H', 'T', 'T', 'P', '/', '1', '.', '1', '\r', '\n',
        'H', 'o', 's', 't', ':', ' ', 'e', 'x', 'a', 'm', 'p', 'l', 'e', '.', 'c', 'o', 'm', '\r', '\n',
        'A', 'c', 'c', 'e', 'p', 't', '-', 'L', 'a', 'n', 'g', 'u', 'a', 'g', 'e', ':', ' ', 'e', 'n', '-', 'U', 'S', ',', ' ', 'd', 'e', ',', ' ', 'f', 'r', '\r', '\n',
        // 'C', 'o', 'n', 't', 'e', 'n', 't', '-', 'L', 'e', 'n', 'g', 't', 'h', ':', ' ', '3', '5', '\r', '\n',
        '\r', '\n'
    };


	std::vector<char> message = post_message_chunked;

	std::cout << "message: " << std::endl;
    for (char c : message)
        std::cout << c;
	std::cout << std::endl;
    
	Request req;

	req.updateBuffer(message);
	// for (int i = 0; i < 5; i++)
    while(1)
	{
		req.parse();
		if (req.is_complete())
			break;
	}
	req.debug_print();

	return (0);
}