#include "restapi.h"

#include "jansson.h"
#include <cassert>
#include <chrono>
#include <iostream>
#include <thread>

struct CurlStartup {
  CurlStartup()   { curl_global_init(CURL_GLOBAL_ALL); }
  ~CurlStartup()  { curl_global_cleanup(); }
} runC_curl_startup;

size_t recv_callback (void *contents, size_t size, size_t nmemb, void *userp) {
  auto &buffer = *static_cast<std::string *> (userp);
  auto n = size * nmemb;
  return buffer.append((char*) contents, n), n;
}

json_t* do_request(CURL *C,
                   const std::string &url,
                   const curl_slist *headers,
                   std::ostream &log) {
  std::string recv_buffer;
  curl_easy_setopt(C, CURLOPT_WRITEDATA, &recv_buffer);
  curl_easy_setopt(C, CURLOPT_URL, url.c_str());
  curl_easy_setopt(C, CURLOPT_HTTPHEADER, headers);
  curl_easy_setopt(C, CURLOPT_DNS_CACHE_TIMEOUT, 3600);

  goto curl_state;

retry_state:
  log << "Retry in 2 seconds..." << std::endl;
  std::this_thread::sleep_for(std::chrono::seconds(2));
  recv_buffer.clear();
  curl_easy_setopt(C, CURLOPT_DNS_CACHE_TIMEOUT, 0);

curl_state:
  CURLcode response_curl = curl_easy_perform(C);
  // log << "cURL response code: " << response_curl << '\n'
  //     << "URL: " << url << '\n';
  //error response code
  if (response_curl != CURLE_OK) {
    log << "Error with cURL: " << curl_easy_strerror(response_curl) << '\n'
        << "  URL: " << url << '\n';

    goto retry_state;
  }

  //error reading json
  json_error_t error;
  // log << "json text response: " << recv_buffer << '\n'
  //     << "URL: " << url << '\n';
  json_t *root = json_loads(recv_buffer.c_str(), 0, &error);
  if (!root) {
    long response_code;
    curl_easy_getinfo(C, CURLINFO_RESPONSE_CODE, &response_code);
    log << "Server Response: " << response_code << " - " << url << '\n'
        << "Error with JSON: " << error.text << '\n'
        << "Buffer:\n"         << recv_buffer << '\n';

    goto retry_state;
  }

  return root;
}

void RestApi::CURL_destructor::operator () (CURL *C) {
  curl_easy_cleanup(C);
}

void RestApi::CURL_destructor::operator () (curl_slist *slist) {
  curl_slist_free_all(slist);
}

RestApi::RestApi(std::string host, const char *cacert, std::ostream &log)
    : C(curl_easy_init()), host(std::move(host)), log(log) {
  assert(C != nullptr);

  if (cacert)
    curl_easy_setopt(C.get(), CURLOPT_CAINFO, cacert);
  else
    curl_easy_setopt(C.get(), CURLOPT_SSL_VERIFYPEER, false);

  curl_easy_setopt(C.get(), CURLOPT_CONNECTTIMEOUT, 10L);
  curl_easy_setopt(C.get(), CURLOPT_TIMEOUT, 20L);
  curl_easy_setopt(C.get(), CURLOPT_USERAGENT, "Bluebird");
  curl_easy_setopt(C.get(), CURLOPT_ACCEPT_ENCODING, "gzip");

  curl_easy_setopt(C.get(), CURLOPT_WRITEFUNCTION, recv_callback);
}

json_t* RestApi::get_request(const std::string &uri, unique_slist headers) {
  curl_easy_setopt(C.get(), CURLOPT_HTTPGET, true);
  return do_request(C.get(), host + uri, headers.get(), log);
}

json_t* RestApi::post_request (const std::string &uri,
                               unique_slist headers,
                               const std::string &post_data) {
  curl_easy_setopt(C.get(), CURLOPT_POSTFIELDS, post_data.data());
  curl_easy_setopt(C.get(), CURLOPT_POSTFIELDSIZE, post_data.size());
  return do_request(C.get(), host + uri, headers.get(), log);
}

json_t* RestApi::post_request (const std::string &uri, const std::string &post_data) {
  return post_request(uri, nullptr, post_data);
}
