// #include "../include/HTTP/requests/Request.hpp"
// #include "../include/HTTP/response/Response.hpp"

// int main(void)
// {
// 	std::vector<char> post_message_chunked = {
//         'P', 'O', 'S', 'T', ' ', '/', 'i', 'n', 'd', 'e', 'e', '.', 't', 'x', 't', ' ', 'H', 'T', 'T', 'P', '/', '1', '.', '1', '\r', '\n',
//         'H', 'o', 's', 't', ':', ' ', 'e', 'x', 'a', 'm', 'p', 'l', 'e', '.', 'c', 'o', 'm', '\r', '\n',
//         'T', 'r', 'a', 'n', 's', 'f', 'e', 'r', '-', 'E', 'n', 'c', 'o', 'd', 'i', 'n', 'g', ':', ' ', 'c', 'h', 'u', 'n', 'k', 'e', 'd', '\r', '\n',
//         'C', 'o', 'n', 't', 'e', 'n', 't', '-', 'T', 'y', 'p', 'e', ':', ' ', 'a', 'p', 'p', 'l', 'i', 'c', 'a', 't', 'i', 'o', 'n', '/', 'j', 's', 'o', 'n', '\r', '\n',
//         '\r', '\n',
//         'c', '\r', '\n', 
//         'n', 'a', 'm', 'e', ':', ' ', 'J', 'o', 'h', 'n', ',', ' ', '\r', '\n',
//         '9', '\r', '\n', 
//         'a', 'g', 'e', ':', ' ', '3', '0', ',', ' ', '\r', '\n',
//         'e', '\r', '\n', 
//         'c', 'i', 't', 'y', ':', ' ', 'N', 'e', 'w', ' ', 'Y', 'o', 'r', 'k', '\r', '\n',
//         '0', '\r', '\n',
//         '\r', '\n'
//     };

// 	std::vector<char> post_message_normal = {
//     'P', 'O', 'S', 'T', ' ', '/', 'i', 'n', 'd', 'e', 'e', '.', 't', 'x', 't', ' ', 'H', 'T', 'T', 'P', '/', '1', '.', '1', '\r', '\n',
//     'H', 'o', 's', 't', ':', ' ', 'e', 'x', 'a', 'm', 'p', 'l', 'e', '.', 'c', 'o', 'm', '\r', '\n',
//     'C', 'o', 'n', 't', 'e', 'n', 't', '-', 'T', 'y', 'p', 'e', ':', ' ', 't', 'e', 'x', 't', '/', 'p', 'l', 'a', 'i', 'n', '\r', '\n',
//     'C', 'o', 'n', 't', 'e', 'n', 't', '-', 'L', 'e', 'n', 'g', 't', 'h', ':', ' ', '3', '5', '\r', '\n',
//     '\r', '\n',
//     'n', 'a', 'm', 'e', ':', ' ', 'J', 'o', 'h', 'n', ',', ' ',
//     'a', 'g', 'e', ':', ' ', '3', '0', ',', ' ',
//     'c', 'i', 't', 'y', ':', ' ', 'N', 'e', 'w', ' ', 'Y', 'o', 'r', 'k'
// 	};

//     std::vector<char> get_message_normal = {
//         'G', 'E', 'T', ' ', '/', 'i', 'n', 'd', 'e', 'x', '.', 'h', 't', 'm', 'l', ' ', 'H', 'T', 'T', 'P', '/', '1', '.', '1', '\r', '\n',
//         'H', 'o', 's', 't', ':', ' ', 'e', 'x', 'a', 'm', 'p', 'l', 'e', '.', 'c', 'o', 'm', '\r', '\n',
//         'A', 'c', 'c', 'e', 'p', 't', '-', 'L', 'a', 'n', 'g', 'u', 'a', 'g', 'e', ':', ' ', 'e', 'n', '-', 'U', 'S', ',', ' ', 'd', 'e', ',', ' ', 'f', 'r', '\r', '\n',
//         // 'C', 'o', 'n', 't', 'e', 'n', 't', '-', 'L', 'e', 'n', 'g', 't', 'h', ':', ' ', '3', '5', '\r', '\n',
//         '\r', '\n'
//     };


// 	std::vector<char> message = post_message_chunked;

// 	std::cout << "message: " << std::endl;
//     for (char c : message)
//         std::cout << c;
// 	std::cout << std::endl;
    
// 	Request req;

// 	req.updateBuffer(message);
// 	// for (int i = 0; i < 4; i++)
//     while(1)
// 	{
// 		try
//         {
//             req.parse();
//         }
//         catch(const Error& e)
//         {
//             err_msg("Parsing failed (for client on FD | reason: " + std::string(e.what()) + " | error code: " + std::to_string(e.code()));
// 			Response response(req);
// 			response.setCode(e.code());
// 			response.build_err();
// 			std::cout << "\nERROR Response:\n" << response.getResult() << std::endl;
//             break ;
//         }
//         if (req.is_complete())
//         {
// 			Response response(req);
// 			response.doMethod();
// 			response.build();
// 			std::cout << "\nResponse:\n" << response.getResult() << std::endl;
//             break;
//         }
//     }

// 	return (0);
// }