#pragma once

namespace Flask {
enum class HttpMethods { GET, POST, HEAD, PUT, DELETE, CONNECT, OPTIONS, TRACE, PATCH, UNKNOWN };

enum class StatusCodes : int {
  Continue = 100,
  Switching_Protocols = 101,
  Processing = 102,

  OK = 200,
  Created = 201,
  Accepted = 202,
  Non_Authoritative_Information = 203,
  No_Content = 204,
  Reste_Content = 205,
  Partial_Content = 206,
  Multi_Status = 207,
  Already_Reported = 208,

  Moved_Parmanently = 301,
  Found = 302,
  See_Other = 303,
  Not_Modified = 304,
  Use_Proxy = 305,
  Teporary_Redirect = 307,

  Bad_Request = 400,
  UNauthorized = 401,
  Payment_Required = 402,
  Forbidden = 403,
  Not_Found = 404,
  Method_Not_Allowed = 405,
  Not_Acceptable = 406,
  Proxy_Authentication_Required = 407,
  RequestTimeout = 408,
  Conflict = 409,
  Gone = 410,
  length_Required = 411,
  Precondition_Failed = 412,
  Request_Entity_Too_Large = 413,
  Request_URL_Too_Large = 414,
  Unsopported_Media_Type = 415,
  Request_Range_Not_Satisfiable = 416,
  Expectation_Failed = 417,
  Im_A_Teapot = 418,
  Unprocessable_Entity = 422,
  Locked = 423,
  Failed_Dependency = 424,
  No_Code = 425,
  Upgrade_Required = 426,
  Precondition_Required = 428,
  Too_Many_Requests = 429,
  Request_Header_Too_Large = 431,
  Retry_With = 449,

  Internal_Server_Error = 500,
  Not_Implemented = 501,
  Bad_Gateway = 502,
  Service_Unavailable = 503,
  Gateway_Timeout = 504,
  HTTP_Version_Not_Supported = 505,
  Vatiant_Also_Negotiates = 506,
  InsufficientStorage = 507,
  Bandwidth_Limit_Exceeded = 509,
  Not_Extended = 510,
  Network_Authentication_Required = 511
};
} // namespace Flask



#define FLASK_RESPONSE_CONTENT_TYPE_TEXT "text/plain; charset=utf-8"
#define FLASK_RESPONSE_CONTENT_TYPE_HTML "text/html; charset=utf-8"
#define FLASK_RESPONSE_CONTENT_TYPE_JS "text/javascript"
#define FLASK_RESPONSE_CONTENT_TYPE_JSON "text/json"
#define FLASK_RESPONSE_CONTENT_TYPE_CSS "text/css"
#define FLASK_RESPONSE_CONTENT_TYPE_PNG "image/png"
#define FLASK_RESPONSE_CONTENT_TYPE_JPG "image/jpg"
#define FLASK_RESPONSE_CONTENT_TYPE_JPEG "image/jpeg"
#define FLASK_RESPONSE_CONTENT_TYPE_GIF "image/gif"

#define FLASK_404_ERROR_HTML_TEXT "<html><body><h1>Page Not Found</h1><p>Sorry, page not found error! something is missing...</p></body></html>\r\n"
#define FLASK_401_ERROR_HTML_TEXT ""

#define FLASK_HEADER_MAX_SIZE 8192
