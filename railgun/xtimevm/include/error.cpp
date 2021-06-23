#include <string>

class vm_error{
    std::string str;
    public:
    vm_error(std::string what){str = what;}
    std::string what(){return "[XtimeVMError] " + str;}
};