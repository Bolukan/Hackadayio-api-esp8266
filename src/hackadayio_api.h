#ifndef HACKADAYIO_API_H
#define HACKADAYIO_API_H

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ca_certificate.h>
#include <secrets.h>
#include <ArduinoJson.h>

#define HACKADAYIO_API_HOST "api.hackaday.io"
#define HACKADAYIO_API_PORT 443
#define HACKADAYIO_PROJECTS_ID "/v1/projects/%d?api_key=%s"

#define HACKADAYIO_DEBUG 1

/*!
 *  @brief  Class to access hackaday api
 */
class HackadayioApi
{
public:
    /*! Response on API calls */
    enum API_Error
    {
        API_Error_None = 0,           /*!< No error */
        API_Error_NoConnection = 1,   /*!< Connection with host failed */
        API_Error_ResponseToLong = 2, /*!< Response of API call to large for memory */
        API_Error_NoResponse = 3,     /*!< No response on API call */
        API_Error_Empty = 4,          /*!< Empty reponse */
        API_Error_NoJSON = 5,
    };

    struct HProject
    {
        API_Error api_error;
        int id;
        const char *url;
        int owner_id;
        const char *name;
        const char *summary;
        const char *description;
        const char *image_url;
        int views;
        int comments;
        int followers;
        int skulls;
        int logs;
        int details;
        int instruction;
        int components;
        int images;
        long created;
        long updated;
        int tags_count;
        // char** tags;
    };

    static const uint32_t MAX_SIZE_RETURN_JSON = 2000;
    static const uint32_t MAX_SIZE_RETURN_STRING = 0x00004000; // 16KB
    static const uint32_t MAX_SIZE_RETURN_SPIFFS = 0x00080000; // 512KB

    /*!
   *  @brief  Instantiate a Hackaday API class
   */
    HackadayioApi(BearSSL::WiFiClientSecure &client, const char* API_key);

    HProject GetProject(int projectid);

private:
    BearSSL::WiFiClientSecure *_client;
    const char* _apikey;

    /*!
     * Connect to host
     */
    API_Error connect();
    API_Error close();
    API_Error fetchURL(const char* request);

};

#endif
