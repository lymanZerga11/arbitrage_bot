#ifndef RESTAPI_H
#define RESTAPI_H

#include "curl/curl.h"
#include <iostream>
#include <memory>
#include <string>

struct json_t;
class RestApi {
private:
  struct CURL_destructor {
    void operator () (CURL *);
    void operator () (curl_slist *); //List of CURL COMMANDS to run before SFTP/FTP
  };

  typedef std::unique_ptr<CURL, CURL_destructor> unique_curl;
  typedef std::unique_ptr<curl_slist, CURL_destructor> unique_slist;

  unique_curl C;
  const std::string host;
  std::ostream &log;

public:
  RestApi              (std::string host, const char *cacert = nullptr,
                        std::ostream &log = std::cerr);
  RestApi              (const RestApi &) =delete;
  RestApi& operator=   (const RestApi &) =delete;

  json_t* get_request  (const std::string &uri, unique_slist headers = nullptr);
  json_t* post_request (const std::string &uri, unique_slist headers = nullptr,
                        const std::string &post_data = "");
  json_t* post_request (const std::string &uri, const std::string &post_data);
};

template <typename T>
RestApi::unique_slist make_slist(T begin, T end) {
  RestApi::unique_slist::pointer res;
  for (res = nullptr; begin != end; ++begin)
    res = curl_slist_append(res, begin->c_str());
  return RestApi::unique_slist(res);  // unique_ptr constructor is explicit
}

#endif
