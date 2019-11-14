#ifndef INJECTOR_H
#define INJECTOR_H

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <windows.h>

#include "empty.h"
#include "compressor.h"

class Injector
{
    public:
        Injector();
        virtual ~Injector();

        bool inject(const std::string& path);
        bool loadResource(int=0); //load main empty-container by default

        inline void setInjection(const std::string& data) { injectionData = data; }
        inline const std::string getLog() const { return log.str(); }

    private:
        std::stringstream log, resourceData;
        std::string injectionData;
};

#endif // INJECTOR_H
