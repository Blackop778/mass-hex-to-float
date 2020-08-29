#include <string>
#include <fstream>
#include <streambuf>
#include <cstdio>
#include <cstring>
#include <cfloat>
#include <cmath>
#include <cstdint>

using namespace std;

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Expected usage: hex2float <file containing hex values>\n");
        return 1;
    }

    ifstream instream(argv[1]);
    if (!instream.good()) {
        printf("Something went wrong while opening the file for reading.\n");
        instream.close();
        return 2;
    }

    // Read in the file
    string contents((istreambuf_iterator<char>(instream)), istreambuf_iterator<char>());
    instream.close();

    // Find hex values
    size_t pos = 0;
    while ((pos = contents.find("0x", pos)) != string::npos) {
        // string of the hex value
        string value;
        value = contents.substr(pos, 10);
        // Ensure the value is an 8 digit hex number
        if (value.length() != 10)
            continue;
        
        bool failed = false;
        for (int i = 2; i < 10; i += 1) {
            if ((value[i] < '0' || value[i] > '9') && (value[i] < 'A' || value[i] > 'F') && (value[i] < 'a' || value[i] > 'f')) {
                failed = true;
                break;
            }
        }
        if (failed)
            continue;

        int32_t hexValue;
        float converter;
        // Perform the conversion, use strtol to ensure we get at least a 32 bit return type
        hexValue = (int32_t)strtol(value.c_str(), NULL, 16);
        *((int32_t*)&converter) = hexValue;
        
        // Find the minimum number of decimal places we have to include to keep the values equal
        int precision;
        bool matched = false;
        for (precision = 1; precision < 10; precision += 1) {
            float before = round(converter * pow(10, precision));
            float rounded = before / pow(10, precision);
            if (rounded == converter) {
                matched = true;
                break;
            }
        }

        char floatFormat[10];
        if (matched)
            // Ensure the output number always has a decimal place and at least one number after the decimal
            snprintf(floatFormat, 10, "%%#.%dff", precision);
        else
            // Resort to scientific notation if we couldn't match it when rounding decimal places
            snprintf(floatFormat, 10, "%%gf");

        // string of the float equivalent
        char floatString[32];
        snprintf(floatString, 32, floatFormat, converter);
        contents.replace(pos, 10, floatString);
        pos -= 10;
    };

    // Replace the file contents with the updated version
    ofstream outstream(argv[1], ios_base::out | ios_base::trunc);
    if (!outstream.good()) {
        printf("Something went wrong while opening the file for writing.\n");
        outstream.close();
        return 3;
    }

    outstream.write(contents.c_str(), contents.length());
    outstream.close();

    return 0;
}
