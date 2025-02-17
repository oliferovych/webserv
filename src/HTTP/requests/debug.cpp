#include "../../../include/HTTP/requests/Request.hpp"

void Request::debug_print() const
{
    std::cout << "\n=== Request Debug Information ===" << std::endl;

    // Print request line
    std::cout << "  Request Line:" << std::endl;
    std::cout << "      Method: " << request_line.method << std::endl;
    std::cout << "      Path: " << request_line.path << std::endl;
    std::cout << "      Version: " << request_line.version << std::endl;

    // Print headers
    std::cout << "  Headers:" << std::endl;
    for (const auto& [key, values] : headers) {
        std::cout << "    " << key << ": ";
        for (size_t i = 0; i < values.size(); ++i) {
            std::cout << values[i];
            if (i != values.size() - 1) std::cout << ", ";
        }
        std::cout << std::endl;
    }

    std::cout << "  query vars: " <<  query_vars << std::endl;
    // Print body
    // std::cout << "  Body (as text, might contain binary data):" << std::endl;
	// if (body.empty())
	// 	std::cout << "  Body wasnt read" << std::endl;
	// else
    // 	std::cout << "    " << std::string(body.begin(), body.end()) << std::endl;

    // Print buffer (truncated if too large for readability)
    std::cout << "  Buffer (size " << buffer.size() << ")" << std::endl;
	if (buffer.empty())
		std::cout << "  buffer is empty" << std::endl;
    // std::cout << "  ";
    // size_t max_display = 80;
    // for (size_t i = 0; i < buffer.size() && i < max_display; ++i) {
    //     std::cout << buffer[i];
    // }
    // if (buffer.size() > max_display) std::cout << "...";
    // std::cout << std::endl;

    // Print content length
    std::cout << "  Content Length: " << content_length << std::endl;



    std::cout << "==================================\n" << std::endl;
}

void Request::debug_state(void) const
{
    // Print state
    std::cout << "Parse State: ";
    switch (state) {
        case PARSE_REQUEST_LINE: std::cout << "PARSE_REQUEST_LINE"; break;
        case PARSE_HEADERS: std::cout << "PARSE_HEADERS"; break;
        case PARSE_BODY: std::cout << "PARSE_BODY"; break;
        case PARSE_CHUNKED_BODY: std::cout << "PARSE_CHUNKED_BODY"; break;
        case COMPLETE: std::cout << "COMPLETE"; break;
        default: std::cout << "UNKNOWN_STATE"; break;
    }
    std::cout << std::endl;
}
