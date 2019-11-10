#include <hackadayio_api.h>

#if (HACKADAYIO_DEBUG == 1)
#define DEBUG_PRINTF(...) { Serial.print(__LINE__); Serial.print(":"); Serial.print(__FUNCTION__); Serial.print(": "); Serial.printf(__VA_ARGS__); }
#else
#define DEBUG_PRINTF(...)
#endif
/*!
 *  @brief  Instantiates a Hackaday API class
 */
HackadayioApi::HackadayioApi(Client &client, const char *API_key)
    : _client(&client), _apikey(API_key)
{
    _client->setTimeout(CLIENT_TIMEOUT);
}

/*!
 *  @brief  Connect to host
 *  @param  projectid Hackaday project id
 *  @return Requested project info
 */
HackadayioApi::HProject HackadayioApi::GetProject(int projectid)
{
DEBUG_PRINTF("projectid = %d\n", projectid);

    HProject hproject;
    hproject.api_error = API_Error_Empty;

    char request[75];
    sprintf(request, HACKADAYIO_PROJECTS_ID, projectid, _apikey);

    // do fetch
    API_Error response = fetchURL(request);
    if (response)
    {
DEBUG_PRINTF("response error = %d\n", response);
        _client->stop();
        hproject.api_error = response;
        return hproject;
    }

    if (_client->peek() != '{')
    {
        hproject.api_error = API_Error_NoJSON;
DEBUG_PRINTF("response error = %d\n", hproject.api_error);
        return hproject;
    }

    // json
    const size_t capacity = JSON_ARRAY_SIZE(5) + JSON_OBJECT_SIZE(19) + MAX_SIZE_RETURN_JSON;
    DynamicJsonDocument doc(capacity);
DEBUG_PRINTF("JSON doc capacity: %d\n", capacity);

    DeserializationError error = deserializeJson(doc, *_client);
    if (!error)
    {
        hproject.api_error = API_Error_None;
        hproject.id = doc["id"].as<int>();
        hproject.url = doc["url"].as<char *>();
        hproject.owner_id = doc["owner_id"].as<int>();
        hproject.name = doc["name"].as<char *>();
        hproject.summary = doc["summary"].as<char *>();
        hproject.description = doc["description"].as<char *>();
        hproject.image_url = doc["image_url"].as<char *>();
        hproject.views = doc["views"].as<int>();
        hproject.comments = doc["comments"].as<int>();
        hproject.followers = doc["followers"].as<int>();
        hproject.skulls = doc["skulls"].as<int>();
        hproject.logs = doc["logs"].as<int>();
        hproject.details = doc["details"].as<int>();
        hproject.instruction = doc["instruction"].as<int>();
        hproject.components = doc["components"].as<int>();
        hproject.images = doc["images"].as<int>();
        hproject.created = doc["created"].as<time_t>();
        hproject.updated = doc["updated"].as<time_t>();
        hproject.tags_count = doc["tags"].size();
    }

    response = close();
    return hproject;
}

/*!
 *  @brief  Connect to host
 */
HackadayioApi::API_Error HackadayioApi::connect()
{
    if (!_client->connect(HACKADAYIO_API_HOST, HACKADAYIO_API_PORT))
    {
        return API_Error_NoConnection;
    }
    return API_Error_None;
}

/*!
 *  @brief  Close connection to host
 */
HackadayioApi::API_Error HackadayioApi::close()
{
    if (_client->connected())
    {
        _client->stop();
    }
    return API_Error_None;
}

HackadayioApi::API_Error HackadayioApi::fetchURL(const char *request)
{
DEBUG_PRINTF("request %s\n", request);
    uint32_t contentLength = 0;
    API_Error response;

    // connect with https
    if (!_client->connected())
    {
        response = connect();
        if (response)
        {
DEBUG_PRINTF("connect error = %d\n", response);
            return response;
        }
    }

    // request
    _client->print(F("GET "));
    _client->print(request);
    _client->print(F(" HTTP/1.1\nHost: "));
    _client->print(HACKADAYIO_API_HOST);
    _client->print(F("\nCache-Control: no-cache\n\n"));

    while (_client->available())
    {
        String headerline = _client->readStringUntil('\n');
DEBUG_PRINTF("headerline %s\n", headerline.c_str());
        if (headerline.indexOf("Content-Length") > -1)
        {
            // Content-Length
            contentLength = headerline.substring(16, headerline.length() - 1).toInt();
DEBUG_PRINTF("contentLength %d\n", contentLength);
            if (contentLength > MAX_SIZE_RETURN_JSON)
            {
                return API_Error_ResponseToLong;
            }
        }
        else if (headerline == "\r")
        {
            return API_Error_None;
        }
    }
DEBUG_PRINTF("Error %d\n", API_Error_NoResponse);
    return API_Error_NoResponse;
}
