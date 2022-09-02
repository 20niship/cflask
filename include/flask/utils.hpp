#pragma once

#include <flask/pageio.hpp>
#include <iostream>

namespace Flask {

std::string get_statuscode_desc(const int);
HttpMethods cvt_str_to_http_method(const std::string&);
std::string cvt_method_to_str(const HttpMethods&);

void set_log_file(const std::string&);
void INFO(const std::string& message);
void DEBUG(const std::string& message);
void WARNING(const std::string& message);
void ERROR(const std::string& message);

std::string html_encode(const std::string& value);

/**
 * @detail fnameのファイルを用いて200 OKのレスポンスデータを作成する関数。
 * @param fname [in]
 * レンダリングするファイルを指定します。実行ファイルの場所のtemplateフォルダの中を検索します。
 * @param values [in] HTMLファイルの中の{{ content
 * }}みたいなやつに代入する値を指定します
 * @retval
 * レンダリング後のレスポンスデータが返されます。ファイルが見つからなかった場合は404エラーのメッセージが返されます。
 */
std::string render_template_file(const std::string& fname, const RenderValues&);

/**
 * @detail fnameのファイルを用いて200 OKのレスポンスデータを作成する関数。
 * @param text [in]
 * レンダリングするファイルを指定します。実行ファイルの場所のtemplateフォルダの中を検索します。
 * @param values [in] HTMLファイルの中の{{ content
 * }}みたいなやつに代入する値を指定します
 * @retval
 * レンダリング後のレスポンスデータが返されます。ファイルが見つからなかった場合は404エラーのメッセージが返されます。
 */
std::string render_template_text(const std::string text, RenderValues values = {});

/**
 * @detail 302のレスポンスを返します。
 * @param new_url [in] 移動先のURLを指定
 * @retval よく分からんテキストが返されるので、
 */
// inline HTTPresponse createRedirectResponse(std::string new_url) {
//   return HTTPresponse(StatusCodes::Moved_Parmanently,
//   FLASK_RESPONSE_CONTENT_TYPE_TEXT, new_url);
// }
void read_file(const std::string& path, std::string& out);

} // namespace Flask
