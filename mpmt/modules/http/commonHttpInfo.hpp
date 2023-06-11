#ifndef COMMONHTTPINFO_H
#define COMMONHTTPINFO_H

#include <iostream>
#include <string>

#define MAX_MAX_HEADER_LIST_SIZE 16777215u
#define DEFAULT_MAX_HEADER_LIST_PAIRS 128u
#define DEFAULT_MAX_HEADER_LENGTH 8192

#define HTTP_SPECIAL_HEADERS(V)                                               \
  V(STATUS, ":status")                                                        \
  V(METHOD, ":method")                                                        \
  V(AUTHORITY, ":authority")                                                  \
  V(SCHEME, ":scheme")                                                        \
  V(PATH, ":path")                                                            \
  V(PROTOCOL, ":protocol")

#define HTTP_REGULAR_HEADERS(V)                                                \
  V(ACCEPT_ENCODING, "accept-encoding")                                        \
  V(ACCEPT_LANGUAGE, "accept-language")                                        \
  V(ACCEPT_RANGES, "accept-ranges")                                            \
  V(ACCEPT, "accept")                                                          \
  V(ACCESS_CONTROL_ALLOW_CREDENTIALS, "access-control-allow-credentials")      \
  V(ACCESS_CONTROL_ALLOW_HEADERS, "access-control-allow-headers")              \
  V(ACCESS_CONTROL_ALLOW_METHODS, "access-control-allow-methods")              \
  V(ACCESS_CONTROL_ALLOW_ORIGIN, "access-control-allow-origin")                \
  V(ACCESS_CONTROL_EXPOSE_HEADERS, "access-control-expose-headers")            \
  V(ACCESS_CONTROL_REQUEST_HEADERS, "access-control-request-headers")          \
  V(ACCESS_CONTROL_REQUEST_METHOD, "access-control-request-method")            \
  V(AGE, "age")                                                                \
  V(AUTHORIZATION, "authorization")                                            \
  V(CACHE_CONTROL, "cache-control")                                            \
  V(CONNECTION, "connection")                                                  \
  V(CONTENT_DISPOSITION, "content-disposition")                                \
  V(CONTENT_ENCODING, "content-encoding")                                      \
  V(CONTENT_LENGTH, "content-length")                                          \
  V(CONTENT_TYPE, "content-type")                                              \
  V(COOKIE, "cookie")                                                          \
  V(DATE, "date")                                                              \
  V(ETAG, "etag")                                                              \
  V(FORWARDED, "forwarded")                                                    \
  V(HOST, "host")                                                              \
  V(IF_MODIFIED_SINCE, "if-modified-since")                                    \
  V(IF_NONE_MATCH, "if-none-match")                                            \
  V(IF_RANGE, "if-range")                                                      \
  V(LAST_MODIFIED, "last-modified")                                            \
  V(LINK, "link")                                                              \
  V(LOCATION, "location")                                                      \
  V(RANGE, "range")                                                            \
  V(REFERER, "referer")                                                        \
  V(SERVER, "server")                                                          \
  V(SET_COOKIE, "set-cookie")                                                  \
  V(STRICT_TRANSPORT_SECURITY, "strict-transport-security")                    \
  V(TRANSFER_ENCODING, "transfer-encoding")                                    \
  V(TE, "te")                                                                  \
  V(UPGRADE_INSECURE_REQUESTS, "upgrade-insecure-requests")                    \
  V(UPGRADE, "upgrade")                                                        \
  V(USER_AGENT, "user-agent")                                                  \
  V(VARY, "vary")                                                              \
  V(X_CONTENT_TYPE_OPTIONS, "x-content-type-options")                          \
  V(X_FRAME_OPTIONS, "x-frame-options")                                        \
  V(KEEP_ALIVE, "keep-alive")                                                  \
  V(PROXY_CONNECTION, "proxy-connection")                                      \
  V(X_XSS_PROTECTION, "x-xss-protection")                                      \
  V(ALT_SVC, "alt-svc")                                                        \
  V(CONTENT_SECURITY_POLICY, "content-security-policy")                        \
  V(EARLY_DATA, "early-data")                                                  \
  V(EXPECT_CT, "expect-ct")                                                    \
  V(ORIGIN, "origin")                                                          \
  V(PURPOSE, "purpose")                                                        \
  V(TIMING_ALLOW_ORIGIN, "timing-allow-origin")                                \
  V(X_FORWARDED_FOR, "x-forwarded-for")                                        \
  V(PRIORITY, "priority")

#define HTTP_ADDITIONAL_HEADERS(V)                                            \
  V(ACCEPT_CHARSET, "accept-charset")                                         \
  V(ACCESS_CONTROL_MAX_AGE, "access-control-max-age")                         \
  V(ALLOW, "allow")                                                           \
  V(CONTENT_LANGUAGE, "content-language")                                     \
  V(CONTENT_LOCATION, "content-location")                                     \
  V(CONTENT_MD5, "content-md5")                                               \
  V(CONTENT_RANGE, "content-range")                                           \
  V(DNT, "dnt")                                                               \
  V(EXPECT, "expect")                                                         \
  V(EXPIRES, "expires")                                                       \
  V(FROM, "from")                                                             \
  V(IF_MATCH, "if-match")                                                     \
  V(IF_UNMODIFIED_SINCE, "if-unmodified-since")                               \
  V(MAX_FORWARDS, "max-forwards")                                             \
  V(PREFER, "prefer")                                                         \
  V(PROXY_AUTHENTICATE, "proxy-authenticate")                                 \
  V(PROXY_AUTHORIZATION, "proxy-authorization")                               \
  V(REFRESH, "refresh")                                                       \
  V(RETRY_AFTER, "retry-after")                                               \
  V(TRAILER, "trailer")                                                       \
  V(TK, "tk")                                                                 \
  V(VIA, "via")                                                               \
  V(WARNING, "warning")                                                       \
  V(WWW_AUTHENTICATE, "www-authenticate")                                     \
  V(HTTP2_SETTINGS, "http2-settings")

// Special and regular headers are handled specifically by the HTTP/2 (and
// later HTTP/3) implementation.
#define HTTP_KNOWN_HEADERS(V)                                                 \
  HTTP_SPECIAL_HEADERS(V)                                                     \
  HTTP_REGULAR_HEADERS(V)                                                     \
  HTTP_ADDITIONAL_HEADERS(V)

enum http_known_headers {
  HTTP_KNOWN_HEADER_MIN,
#define V(name, value) HTTP_HEADER_##name,
  HTTP_KNOWN_HEADERS(V)
#undef V
  HTTP_KNOWN_HEADER_MAX
};


#define V(name, code) HTTP_STATUS_##name = code,
#define HTTP_STATUS_CODES_ENUM(name, code) HTTP_STATUS_##name,
#define HTTP_STATUS_CODES_STRING(name, code) \
  if (status == name) return (code);

#define HTTP_STATUS_CODES(V)                                                  \
  V(CONTINUE, 100)                                                            \
  V(SWITCHING_PROTOCOLS, 101)                                                 \
  V(PROCESSING, 102)                                                          \
  V(EARLY_HINTS, 103)                                                         \
  V(OK, 200)                                                                  \
  V(CREATED, 201)                                                             \
  V(ACCEPTED, 202)                                                            \
  V(NON_AUTHORITATIVE_INFORMATION, 203)                                       \
  V(NO_CONTENT, 204)                                                          \
  V(RESET_CONTENT, 205)                                                       \
  V(PARTIAL_CONTENT, 206)                                                     \
  V(MULTI_STATUS, 207)                                                        \
  V(ALREADY_REPORTED, 208)                                                    \
  V(IM_USED, 226)                                                             \
  V(MULTIPLE_CHOICES, 300)                                                    \
  V(MOVED_PERMANENTLY, 301)                                                   \
  V(FOUND, 302)                                                               \
  V(SEE_OTHER, 303)                                                           \
  V(NOT_MODIFIED, 304)                                                        \
  V(USE_PROXY, 305)                                                           \
  V(TEMPORARY_REDIRECT, 307)                                                  \
  V(PERMANENT_REDIRECT, 308)                                                  \
  V(BAD_REQUEST, 400)                                                         \
  V(UNAUTHORIZED, 401)                                                        \
  V(PAYMENT_REQUIRED, 402)                                                    \
  V(FORBIDDEN, 403)                                                           \
  V(NOT_FOUND, 404)                                                           \
  V(METHOD_NOT_ALLOWED, 405)                                                  \
  V(NOT_ACCEPTABLE, 406)                                                      \
  V(PROXY_AUTHENTICATION_REQUIRED, 407)                                       \
  V(REQUEST_TIMEOUT, 408)                                                     \
  V(CONFLICT, 409)                                                            \
  V(GONE, 410)                                                                \
  V(LENGTH_REQUIRED, 411)                                                     \
  V(PRECONDITION_FAILED, 412)                                                 \
  V(PAYLOAD_TOO_LARGE, 413)                                                   \
  V(URI_TOO_LONG, 414)                                                        \
  V(UNSUPPORTED_MEDIA_TYPE, 415)                                              \
  V(RANGE_NOT_SATISFIABLE, 416)                                               \
  V(EXPECTATION_FAILED, 417)                                                  \
  V(TEAPOT, 418)                                                              \
  V(MISDIRECTED_REQUEST, 421)                                                 \
  V(UNPROCESSABLE_ENTITY, 422)                                                \
  V(LOCKED, 423)                                                              \
  V(FAILED_DEPENDENCY, 424)                                                   \
  V(TOO_EARLY, 425)                                                           \
  V(UPGRADE_REQUIRED, 426)                                                    \
  V(PRECONDITION_REQUIRED, 428)                                               \
  V(TOO_MANY_REQUESTS, 429)                                                   \
  V(REQUEST_HEADER_FIELDS_TOO_LARGE, 431)                                     \
  V(UNAVAILABLE_FOR_LEGAL_REASONS, 451)                                       \
  V(INTERNAL_SERVER_ERROR, 500)                                               \
  V(NOT_IMPLEMENTED, 501)                                                     \
  V(BAD_GATEWAY, 502)                                                         \
  V(SERVICE_UNAVAILABLE, 503)                                                 \
  V(GATEWAY_TIMEOUT, 504)                                                     \
  V(HTTP_VERSION_NOT_SUPPORTED, 505)                                          \
  V(VARIANT_ALSO_NEGOTIATES, 506)                                             \
  V(INSUFFICIENT_STORAGE, 507)                                                \
  V(LOOP_DETECTED, 508)                                                       \
  V(BANDWIDTH_LIMIT_EXCEEDED, 509)                                            \
  V(NOT_EXTENDED, 510)                                                        \
  V(NETWORK_AUTHENTICATION_REQUIRED, 511)

enum http_status_codes {
  HTTP_STATUS_CODES(HTTP_STATUS_CODES_ENUM)
};

#define HTTP_STATUS_MSG(V) 	                                                    \
  V(100, "Continue")                                                            \
  V(101, "Switching Protocols")                                                 \
  V(102, "Processing")                                                          \
  V(103, "Early Hints")                                                         \
  V(200, "OK")                                                                  \
  V(201, "Created")                                                             \
  V(202, "Accepted")                                                            \
  V(203, "Non Authoritative Information")                                       \
  V(204, "No Content")                                                          \
  V(205, "Reset Content")                                                       \
  V(206, "Partial Content")                                                     \
  V(207, "Multi Status")                                                        \
  V(208, "Already Reported")                                                    \
  V(226, "Im Used")                                                             \
  V(300, "Multiple Choices")                                                    \
  V(301, "Moved Permanently")                                                   \
  V(302, "Found")                                                               \
  V(303, "See Other")                                                           \
  V(304, "Not Modified")                                                        \
  V(305, "Use Proxy")                                                           \
  V(307, "Temporary Redirect")                                                  \
  V(308, "Permanent Redirect")                                                  \
  V(400, "Bad Request")                                                         \
  V(401, "Unauthorized")                                                        \
  V(402, "Payment Required")                                                    \
  V(403, "Forbidden")                                                           \
  V(404, "Not Found")                                                           \
  V(405, "Method Not Allowed")                                                  \
  V(406, "Not Acceptable")                                                      \
  V(407, "Proxy Authentication Required")                                       \
  V(408, "Request Time-out")                                                     \
  V(409, "Conflict")                                                            \
  V(410, "Gone")                                                                \
  V(411, "Length Required")                                                     \
  V(412, "Precondition_Failed")                                                 \
  V(413, "Request Entity Too Large")                                                   \
  V(414, "Request-URI Too Large")                                                        \
  V(415, "Unsupported Media Type")                                              \
  V(416, "Request Range Not Satisfiable")                                               \
  V(417, "Expectation Failed")                                                  \
  V(418, "Teapot")                                                              \
  V(421, "Misdirected Request")                                                 \
  V(422, "Unprocessable Entity")                                                \
  V(423, "Locked")                                                              \
  V(424, "Failed Dependency")                                                   \
  V(425, "Too Early")                                                           \
  V(426, "Upgrade Required")                                                    \
  V(428, "Precondition Required")                                               \
  V(429, "Too Many Requests")                                                   \
  V(431, "Request Header Fields Too Large")                                     \
  V(451, "Unavailable For Legal Reasons")                                       \
  V(500, "Internal Server Error")                                               \
  V(501, "Not Implemented")                                                     \
  V(502, "Bad Gateway")                                                         \
  V(503, "Service Unavailable")                                                 \
  V(504, "Gateway Time-out")                                                     \
  V(505, "Http Version Not Supported")                                          \
  V(506, "Variant Also Negotiates")                                             \
  V(507, "Insufficient Storage")                                                \
  V(508, "Loop Detected")                                                       \
  V(509, "Bandwidth Limit Exceeded")                                            \
  V(510, "Not Extended")                                                        \
  V(511, "Network Authentication Required")

class http_status_msg {
 public:
  http_status_msg(int code) : code_(code) {}

  std::string getStatusMsg() const {
    int status = code_;
    HTTP_STATUS_MSG(HTTP_STATUS_CODES_STRING);
    return "UNKNOWN";
  }
 private:
  int code_;
};
#undef V

// Unlike the HTTP/1 implementation, the HTTP/2 implementation is not limited
// to a fixed number of known supported HTTP methods. These constants, therefore
// are provided strictly as a convenience to users and are exposed via the
// require('node:http2').constants object.
#define HTTP_KNOWN_METHODS(V)                                                 \
  V(ACL, "ACL")                                                               \
  V(BASELINE_CONTROL, "BASELINE-CONTROL")                                     \
  V(BIND, "BIND")                                                             \
  V(CHECKIN, "CHECKIN")                                                       \
  V(CHECKOUT, "CHECKOUT")                                                     \
  V(CONNECT, "CONNECT")                                                       \
  V(COPY, "COPY")                                                             \
  V(DELETE, "DELETE")                                                         \
  V(GET, "GET")                                                               \
  V(HEAD, "HEAD")                                                             \
  V(LABEL, "LABEL")                                                           \
  V(LINK, "LINK")                                                             \
  V(LOCK, "LOCK")                                                             \
  V(MERGE, "MERGE")                                                           \
  V(MKACTIVITY, "MKACTIVITY")                                                 \
  V(MKCALENDAR, "MKCALENDAR")                                                 \
  V(MKCOL, "MKCOL")                                                           \
  V(MKREDIRECTREF, "MKREDIRECTREF")                                           \
  V(MKWORKSPACE, "MKWORKSPACE")                                               \
  V(MOVE, "MOVE")                                                             \
  V(OPTIONS, "OPTIONS")                                                       \
  V(ORDERPATCH, "ORDERPATCH")                                                 \
  V(PATCH, "PATCH")                                                           \
  V(POST, "POST")                                                             \
  V(PRI, "PRI")                                                               \
  V(PROPFIND, "PROPFIND")                                                     \
  V(PROPPATCH, "PROPPATCH")                                                   \
  V(PUT, "PUT")                                                               \
  V(REBIND, "REBIND")                                                         \
  V(REPORT, "REPORT")                                                         \
  V(SEARCH, "SEARCH")                                                         \
  V(TRACE, "TRACE")                                                           \
  V(UNBIND, "UNBIND")                                                         \
  V(UNCHECKOUT, "UNCHECKOUT")                                                 \
  V(UNLINK, "UNLINK")                                                         \
  V(UNLOCK, "UNLOCK")                                                         \
  V(UPDATE, "UPDATE")                                                         \
  V(UPDATEREDIRECTREF, "UPDATEREDIRECTREF")                                   \
  V(VERSION_CONTROL, "VERSION-CONTROL")

#endif