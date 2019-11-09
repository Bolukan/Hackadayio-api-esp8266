#ifndef HACKADAYIO_API_H
#define HACKADAYIO_API_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <Client.h>

// root key for api.hackaday.io
// Common Name: DigiCert Global Root CA
// Organization: DigiCert Inc
// Organization Unit: www.digicert.com
// Country: US
// Valid From: November 9, 2006
// Valid To: November 9, 2031
// Issuer: DigiCert Global Root CA, DigiCert Inc
// Serial Number: 083be056904246b1a1756ac95991c74a
const char DIGICERTGLOBALROOTCA[] = R"=EOF=(
-----BEGIN CERTIFICATE-----
MIIDrzCCApegAwIBAgIQCDvgVpBCRrGhdWrJWZHHSjANBgkqhkiG9w0BAQUFADBh
MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3
d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBD
QTAeFw0wNjExMTAwMDAwMDBaFw0zMTExMTAwMDAwMDBaMGExCzAJBgNVBAYTAlVT
MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j
b20xIDAeBgNVBAMTF0RpZ2lDZXJ0IEdsb2JhbCBSb290IENBMIIBIjANBgkqhkiG
9w0BAQEFAAOCAQ8AMIIBCgKCAQEA4jvhEXLeqKTTo1eqUKKPC3eQyaKl7hLOllsB
CSDMAZOnTjC3U/dDxGkAV53ijSLdhwZAAIEJzs4bg7/fzTtxRuLWZscFs3YnFo97
nh6Vfe63SKMI2tavegw5BmV/Sl0fvBf4q77uKNd0f3p4mVmFaG5cIzJLv07A6Fpt
43C/dxC//AH2hdmoRBBYMql1GNXRor5H4idq9Joz+EkIYIvUX7Q6hL+hqkpMfT7P
T19sdl6gSzeRntwi5m3OFBqOasv+zbMUZBfHWymeMr/y7vrTC0LUq7dBMtoM1O/4
gdW7jVg/tRvoSSiicNoxBN33shbyTApOB6jtSj1etX+jkMOvJwIDAQABo2MwYTAO
BgNVHQ8BAf8EBAMCAYYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4EFgQUA95QNVbR
TLtm8KPiGxvDl7I90VUwHwYDVR0jBBgwFoAUA95QNVbRTLtm8KPiGxvDl7I90VUw
DQYJKoZIhvcNAQEFBQADggEBAMucN6pIExIK+t1EnE9SsPTfrgT1eXkIoyQY/Esr
hMAtudXH/vTBH1jLuG2cenTnmCmrEbXjcKChzUyImZOMkXDiqw8cvpOp/2PV5Adg
06O/nVsJ8dWO41P0jmP6P6fbtGbfYmbW0W5BjfIttep3Sp+dWOIrWcBAI+0tKIJF
PnlUkiaY4IBIqDfv8NZ5YBberOgOzW6sRBc4L0na4UU+Krk2U886UAb3LujEV0ls
YSEY1QSteDwsOoBrp+uvFRTp2InBuThs4pFsiv9kuXclVzDAGySj4dzp30d8tbQk
CAUw7C29C79Fv1C5qfPrmAESrciIxpg0X40KPMbp1ZWVbd4=
-----END CERTIFICATE-----
)=EOF=";

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
        time_t created;
        time_t updated;
        int tags_count;
        // char** tags;
    };

    static const uint32_t MAX_SIZE_RETURN_JSON = 2000;
    static const uint32_t MAX_SIZE_RETURN_STRING = 0x00004000; // 16KB
    static const uint32_t MAX_SIZE_RETURN_SPIFFS = 0x00080000; // 512KB

    /*!
   *  @brief  Instantiate a Hackaday API class
   */
    HackadayioApi(Client &client, const char* API_key);

    HProject GetProject(int projectid);

private:
    Client* _client;
    const char* _apikey;

    /*!
     * Connect to host
     */
    API_Error connect();
    API_Error close();
    API_Error fetchURL(const char* request);

};

#endif
