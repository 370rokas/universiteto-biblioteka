#ifndef UB_API_HPP
#define UB_API_HPP

#include "structs.hpp"

#include <cpr/cprtypes.h>
#include <cpr/session.h>

namespace ub {

class Connection {
	std::string _url;
	std::string _token;

	void _throwIfNotLoggedIn();
	cpr::Header _getHeaders();

	cpr::Url _getEndpointUrl(const std::string &path);

  public:
	Connection(const std::string &url) {
		if (!url.empty() && url.back() == '/') {
			_url = url;
		} else {
			_url = url + "/";
		}
	}

	// Base Endpoints
	ServerInfoResp getApiInfo();

	// Auth Endpoints
	RegisterResp authRegister(const AuthCredsBody &body);
	LoginResp authLogin(const AuthCredsBody &body);
	SessionInfoResp getSessionInfo(const std::string &token);

	// Book Endpoints
	SearchResp bookSearch(const std::string &query);
	GetBookByIdResp bookGetById(const std::string &bookId);
};

} // namespace ub

#endif // UB_API_HPP