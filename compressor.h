#ifndef COMPRESSOR_H
#define COMPRESSOR_H

#include <string>
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <sstream>

#include <zlib.h>

class Compressor
{
    public:
        Compressor(int compressionlevel = Z_BEST_COMPRESSION);
        virtual ~Compressor();

        bool compress(const std::string& str);
        bool decompress(const std::string& str);
        inline const std::string result() const { return _resultData; }
        inline const bool errorOccurred() const { return _errorOccurred; }
        inline const std::string getError() const { return _lastError; }
        inline void clearError() { _lastError = ""; _errorOccurred = false; }

    private:
        std::string _resultData, _lastError;
        bool _errorOccurred;
        int _compressionLevel;
        std::string compress_string(const std::string& str, int compressionlevel);
        std::string decompress_string(const std::string& str);
        void setError(const std::string& msg) { _lastError = msg; _errorOccurred = true; }

};

#endif // COMPRESSOR_H
