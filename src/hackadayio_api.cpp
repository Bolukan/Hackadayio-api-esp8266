#include <hackadayio_api.h>

/*!
 *  @brief  Instantiates a Hackaday API class
 */
HackadayioApi::HackadayioApi(BearSSL::WiFiClientSecure &client, const char *API_key)
    : _client(&client), _apikey(API_key)
{
    static BearSSL::X509List cert(CERTIFICATE_ROOT);
    _client->setTrustAnchors(&cert);
}

/*!
 *  @brief  Connect to host
 *  @param  projectid Hackaday project id
 *  @return Requested project info
 */
HackadayioApi::HProject HackadayioApi::GetProject(int projectid)
{
#if (HACKADAYIO_DEBUG == 1)
    Serial.printf("%s:%d projectid %d\n", __FUNCTION__, __LINE__, projectid);
#endif

    HProject hproject;
    hproject.api_error = API_Error_Empty;

    char request[75];
    sprintf(request, HACKADAYIO_PROJECTS_ID, projectid, _apikey);

    // do fetch
    API_Error response = fetchURL(request);
#if (HACKADAYIO_DEBUG == 1)
    Serial.printf("%s:%d fetchurl returned: %d\n", __FUNCTION__, __LINE__, response);
#endif

    if (response)
    {
        _client->stop();
        hproject.api_error = response;
        return hproject;
    }

#if (HACKADAYIO_DEBUG == 1)
    Serial.printf("%s:%d peek\n", __FUNCTION__, __LINE__);
#endif
    if (_client->peek() != '{')
    {
        hproject.api_error = API_Error_NoJSON;
        return hproject;
    }

#if (HACKADAYIO_DEBUG == 1)
    Serial.printf("%s:%d JSON\n", __FUNCTION__, __LINE__);
#endif
    // json
    const size_t capacity = JSON_ARRAY_SIZE(5) + JSON_OBJECT_SIZE(19) + MAX_SIZE_RETURN_JSON;
    DynamicJsonDocument doc(capacity);
#if (HACKADAYIO_DEBUG == 1)
    Serial.printf("%s:%d JSON doc capacity: %d\n", __FUNCTION__, __LINE__, capacity);
#endif
    DeserializationError error = deserializeJson(doc, *_client);

    if (!error)
    {
        hproject.api_error = API_Error_None;
        hproject.id = doc["id"].as<int>();
        hproject.url = (char *)doc["url"].as<char *>();
        hproject.owner_id = doc["owner_id"].as<int>();
        hproject.name = (char *)doc["name"].as<char *>();
        hproject.summary = (char *)doc["summary"].as<char *>();
        hproject.description = (char *)doc["description"].as<char *>();
        hproject.image_url = (char *)doc["image_url"].as<char *>();
        hproject.views = doc["views"].as<int>();
        hproject.comments = doc["comments"].as<int>();
        hproject.followers = doc["followers"].as<int>();
        hproject.skulls = doc["skulls"].as<int>();
        hproject.logs = doc["logs"].as<int>();
        hproject.details = doc["details"].as<int>();
        hproject.instruction = doc["instruction"].as<int>();
        hproject.components = doc["components"].as<int>();
        hproject.images = doc["images"].as<int>();
        hproject.created = doc["created"].as<long>();
        hproject.updated = doc["updated"].as<long>();
        hproject.tags_count = 0; // doc["tags"].size();
    }
    _client->stop();
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
#if (HACKADAYIO_DEBUG == 1)
    Serial.printf("%s:%d request %s\n", __FUNCTION__, __LINE__, request);
#endif
    uint32_t contentLength = 0;

    API_Error response;

    // connect with https
    if (!_client->connected())
    {
        response = connect();
#if (HACKADAYIO_DEBUG == 1)
        Serial.printf("%s:%d connect %d\n", __FUNCTION__, __LINE__, response);
#endif
        if (response)
            return response;
    }

    // request
    _client->setTimeout(4000);
    _client->printf("GET %s HTTP/1.1\nHost: %s\nCache-Control: no-cache\n\n", request, HACKADAYIO_API_HOST);

    while (_client->available())
    {
        String headerline = _client->readStringUntil('\n');
#if (HACKADAYIO_DEBUG == 1)
        Serial.printf("%s:%d headerline %s\n", __FUNCTION__, __LINE__, headerline.c_str());
#endif
        if (headerline.indexOf("Content-Length") > -1)
        {
            // Content-Length
            contentLength = headerline.substring(16, headerline.length() - 1).toInt();
#if (HACKADAYIO_DEBUG == 1)
            Serial.printf("%s:%d contentLength %d\n", __FUNCTION__, __LINE__, contentLength);
#endif
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
#if (HACKADAYIO_DEBUG == 1)
    Serial.printf("%s:%d API_Error_NoResponse\n", __FUNCTION__, __LINE__);
#endif
    return API_Error_NoResponse;
}
