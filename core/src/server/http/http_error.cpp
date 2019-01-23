#include <server/http/http_error.hpp>

#include <unordered_map>

namespace server {
namespace http {

namespace {

using handlers::HandlerErrorCode;

const std::unordered_map<handlers::HandlerErrorCode, HttpStatus,
                         handlers::HandlerErrorCodeHash>
    kCustomHandlerStatusToHttp{
        {HandlerErrorCode::kClientError, HttpStatus::kBadRequest},
        {HandlerErrorCode::kUnauthorized, HttpStatus::kUnauthorized},
        {HandlerErrorCode::kServerSideError, HttpStatus::kInternalServerError}};

}  // namespace

HttpStatus GetHttpStatus(handlers::HandlerErrorCode code) noexcept {
  if (auto f = kCustomHandlerStatusToHttp.find(code);
      f != kCustomHandlerStatusToHttp.end()) {
    return f->second;
  }
  if (code < handlers::HandlerErrorCode::kServerSideError)
    return HttpStatus::kBadRequest;
  return HttpStatus::kInternalServerError;
}

}  // namespace http
}  // namespace server
