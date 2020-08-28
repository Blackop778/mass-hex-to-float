#include <string>
#include <fstream>
#include <streambuf>
#include <cstdio>
#include <cstring>
#include <cfloat>
#include <cmath>

using namespace std;

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Expected usage: hex2float <file containing hex values>\n");
        return 1;
    }

    ifstream instream(argv[1]);
    string value;
    float converter;
    char floatString[32];
    unsigned long hexValue;

    string contents((istreambuf_iterator<char>(instream)), istreambuf_iterator<char>());
    instream.close();
    size_t pos = 0;
    while ((pos = contents.find("0x", pos)) != string::npos) {
        value = contents.substr(pos, 10);
        if (value.length() != 10)
            continue;
        
        bool failed = false;
        for (int i = 2; i < 10; i += 1) {
            if ((value[i] < '0' || value[i] > '9') && (value[i] < 'A' || value[i] > 'F')) {
                failed = true;
                break;
            }
        }
        if (failed)
            continue;

        hexValue = strtoul(value.c_str(), NULL, 16);
        *((unsigned int*)&converter) = (unsigned int)hexValue;
        
        int digits;
        for (digits = 1; digits < 10; digits += 1) {
            float before = round(converter * pow(10, digits));
            float rounded = before / pow(10, digits);
            if (rounded == converter)
                break;
        }

        char floatFormat[10];
        snprintf(floatFormat, 10, "%%#.%dff", digits);

        snprintf(floatString, 32, floatFormat, converter);
        // skip decrease by 1 to get a valid index, then decrease by another 1 to skip 'f'
        // Also don't let i reach 0 since we check the char before the one at i
        for (int i = strlen(floatString) - 2; i > 0; i -= 1) {
            if (floatString[i] == '0' && floatString[i - 1] != '.') {
                floatString[i] = 'f';
                // Even if the whole float string was somehow used,
                // this would only overwrite the null terminator with another null terminator
                floatString[i + 1] = '\0';
            } else {
                break;
            }
        }
        contents.replace(pos, 10, floatString);
        pos -= 10;
    };

    ofstream outstream(argv[1], ios_base::out | ios_base::trunc);
    outstream.write(contents.c_str(), contents.length());
    outstream.close();

    return 0;
}
