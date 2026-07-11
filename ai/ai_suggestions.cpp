#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define _HAS_STD_BYTE 0

#include <windows.h>
#include <winhttp.h>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <fstream>

using namespace std;

static string escapeJSON(const string& s) {
    string result = "";
    for (char c : s) {
        if (c == '\n')      result += "\\n";
        else if (c == '\r') result += "\\r";
        else if (c == '\t') result += "\\t";
        else if (c == '"')  result += "\\\"";
        else if (c == '\\') result += "\\\\";
        else result += c;
    }
    return result;
}

static string getEnvValue(string keyName) {
    ifstream file(".env");
    if (!file.is_open()) return "";
    string line;
    while (getline(file, line)) {
        int pos = line.find('=');
        if (pos == (int)string::npos) continue;
        if (line.substr(0, pos) == keyName) {
            string value = line.substr(pos + 1);
            if (!value.empty() && value.back() == '\r')
                value.pop_back();
            return value;
        }
    }
    return "";
}

static wstring toWide(const string& s) {
    return wstring(s.begin(), s.end());
}

static string httpPost(const string& apiKey, const string& body) {
    string result = "";

    HINTERNET hSession = WinHttpOpen(
        L"ResumeIQ/1.0",
        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
        WINHTTP_NO_PROXY_NAME,
        WINHTTP_NO_PROXY_BYPASS, 0);
    if (!hSession) return "Error: session failed";

    HINTERNET hConnect = WinHttpConnect(
        hSession, L"api.groq.com",
        INTERNET_DEFAULT_HTTPS_PORT, 0);
    if (!hConnect) {
        WinHttpCloseHandle(hSession);
        return "Error: connect failed";
    }

    HINTERNET hRequest = WinHttpOpenRequest(
        hConnect, L"POST",
        L"/openai/v1/chat/completions",
        NULL, WINHTTP_NO_REFERER,
        WINHTTP_DEFAULT_ACCEPT_TYPES,
        WINHTTP_FLAG_SECURE);
    if (!hRequest) {
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return "Error: request failed";
    }

    wstring auth = toWide("Authorization: Bearer " + apiKey);
    WinHttpAddRequestHeaders(hRequest, auth.c_str(), -1L,
        WINHTTP_ADDREQ_FLAG_ADD);
    WinHttpAddRequestHeaders(hRequest,
        L"Content-Type: application/json",
        -1L, WINHTTP_ADDREQ_FLAG_ADD);

    BOOL ok = WinHttpSendRequest(hRequest,
        WINHTTP_NO_ADDITIONAL_HEADERS, 0,
        (LPVOID)body.c_str(),
        body.length(), body.length(), 0);
    if (!ok) {
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return "Error: send failed";
    }

    WinHttpReceiveResponse(hRequest, NULL);

    DWORD size = 0;
    do {
        size = 0;
        WinHttpQueryDataAvailable(hRequest, &size);
        if (size > 0) {
            char* buf = new char[size + 1];
            DWORD read = 0;
            WinHttpReadData(hRequest, buf, size, &read);
            buf[read] = '\0';
            result += string(buf);
            delete[] buf;
        }
    } while (size > 0);

    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);
    return result;
}

string getAISuggestions(string role,
                         vector<string> matched,
                         vector<string> missing) {

    if (missing.empty())
        return "Great! You match all required skills.";

    string apiKey = getEnvValue("GROQ_API_KEY");
    if (apiKey.empty())
        return "AI unavailable: API key not found in .env";

    // Build prompt
     stringstream prompt;
prompt << "Role: " << role << ". ";
prompt << "Missing skills: ";
for (auto s : missing) prompt << s << ", ";
prompt << "Give exactly 3 numbered tips to learn these skills for this role. ";
prompt << "Each tip is one sentence only. Plain text. No markdown. No asterisks.";


// Build JSON body
    string body = "{";
    body += "\"model\":\"llama-3.1-8b-instant\",";
    body += "\"messages\":[";
    body += "{\"role\":\"system\",\"content\":\"You are a career advisor. Give practical structured advice.\"},";
    body += "{\"role\":\"user\",\"content\":\"" + escapeJSON(prompt.str()) + "\"}";
    body += "],";
    body += "\"max_tokens\":150,";
    body += "\"temperature\":0.7}";

    // Make API call
    string raw = httpPost(apiKey, body);

    // Find choices section — skip system message
    size_t choicesPos = raw.find("\"choices\"");
    if (choicesPos == string::npos) {
        cout << "RAW: " << raw << "\n";
        return "AI unavailable: no choices found.";
    }

    // Find content after choices
    string marker = "\"content\":\"";
    size_t start = raw.find(marker, choicesPos);
    if (start == string::npos)
        return "AI unavailable: no content found.";

    start += marker.length();

    // Find end of content
    size_t endMarker = raw.find("\",\"finish_reason\"", start);
    if (endMarker == string::npos)
        endMarker = raw.find("\"}", start);
    if (endMarker == string::npos)
        return "AI unavailable: parse error.";

    string rawText = raw.substr(start, endMarker - start);

    // Unescape characters
    string aiText = "";
    for (size_t i = 0; i < rawText.length(); i++) {
        if (rawText[i] == '\\' && i+1 < rawText.length()) {
            if (rawText[i+1] == 'n')        { aiText += '\n'; i++; }
            else if (rawText[i+1] == 't')   { aiText += '\t'; i++; }
            else if (rawText[i+1] == '"')   { aiText += '"';  i++; }
            else if (rawText[i+1] == '\\')  { aiText += '\\'; i++; }
            else aiText += rawText[i];
        } else {
            aiText += rawText[i];
        }
    }

    if (aiText.empty())
        return "AI unavailable: empty response.";

    return aiText;
}