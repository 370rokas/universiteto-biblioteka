#include "include/ub/api.hpp"
#include "include/ub/structs.hpp"

#include <cpr/cpr.h>
#include <cpr/cprtypes.h>
#include <nlohmann/json.hpp>
#include <stdexcept>

using namespace ub;

cpr::Url Connection::_getEndpointUrl(const std::string &path) {
	return cpr::Url{_url + path};
}

void Connection::_throwIfNotLoggedIn() {
	if (_token.empty()) {
		throw std::runtime_error("Not logged in.");
	}
};

cpr::Header Connection::_getHeaders() {
	cpr::Header h = {
		{"Accept", "application/json"},
		{"Content-Type", "application/json"},
		{"User-Agent", "ub-api-wrapper-cpp/1.0"}};

	if (!_token.empty()) {
		h.insert({"Authorization", "Bearer " + _token});
	}

	return h;
};

ServerInfoResp Connection::getApiInfo() {
	ServerInfoResp resp;

	auto headers = _getHeaders();
	auto r = cpr::Get(_getEndpointUrl(""), headers);

	auto json = nlohmann::json::parse(r.text);

	resp = json.get<ServerInfoResp>();
	return resp;
};

RegisterResp Connection::authRegister(const AuthCredsBody &body) {
	RegisterResp resp;

	auto headers = _getHeaders();
	auto r = cpr::Post(_getEndpointUrl("auth/register"), headers,
					   cpr::Body{nlohmann::json(body).dump()},
					   cpr::Header{{"Content-Type", "application/json"}});

	auto json = nlohmann::json::parse(r.text);

	resp = json.get<RegisterResp>();
	return resp;
};

LoginResp Connection::authLogin(const AuthCredsBody &body) {
	LoginResp resp;

	auto headers = _getHeaders();
	auto r = cpr::Post(_getEndpointUrl("auth/login"), headers,
					   cpr::Body{nlohmann::json(body).dump()});

	auto json = nlohmann::json::parse(r.text);

	resp = json.get<LoginResp>();

	if (resp.ok && resp.token.has_value()) {
		_token = resp.token.value();
	}

	return resp;
};

SessionInfoResp Connection::getSessionInfo(const std::string &token) {
	SessionInfoResp resp;

	auto headers = _getHeaders();
	auto r = cpr::Get(_getEndpointUrl("auth/"), headers);

	auto json = nlohmann::json::parse(r.text);

	resp = json.get<SessionInfoResp>();
	return resp;
};

SearchResp Connection::bookSearch(const std::string &query) {
	SearchResp resp;

	_throwIfNotLoggedIn();

	auto headers = _getHeaders();
	auto r = cpr::Get(_getEndpointUrl("books/search"),
					  headers,
					  cpr::Parameters{{"q", query}});

	auto json = nlohmann::json::parse(r.text);

	resp = json.get<SearchResp>();
	return resp;
};

GetBookByIdResp Connection::bookGetById(const std::string &bookId) {
	GetBookByIdResp resp;

	_throwIfNotLoggedIn();

	auto headers = _getHeaders();
	auto r = cpr::Get(_getEndpointUrl("books/" + bookId), headers);

	auto json = nlohmann::json::parse(r.text);

	resp = json.get<GetBookByIdResp>();
	return resp;
};