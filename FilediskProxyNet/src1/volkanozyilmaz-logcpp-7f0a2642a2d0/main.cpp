
#include "log.h"

int main(int argc, char* argv[])
{
    // Sample
#if 0
    try
    {
        FILELog::ReportingLevel() = FILELog::FromString(argv[1] ? argv[1] : "DEBUG1");
	    const int count = 3;
        L_(ldebug) << "A loop with " << count << " iterations";
        for (int i = 0; i != count; ++i)
        {
	        L_(ldebug1) << "the counter i = " << i;
        }
        return 0;
    }
    catch(const std::exception& e)
    {
        L_(lerror) << e.what();
    }
    return -1;
#endif
    
    // Another Sample
#if 0
    // initialize
    FILELog::ReportingLevel() = ldebug3;
    FILE* log_fd = fopen( "mylogfile.txt", "w" );
    Output2FILE::Stream() = log_fd;
    
    // use
    FILE_LOG(lwarning) << "Ops, variable x should be " << expectedX << "; is " << realX;
    // or
    L_(lwarning) << "Ops, variable x should be " << expectedX << "; is " << realX;
#endif
    
    // I use this sample
#if 1
    initLogger( "mylogfile.log", ldebug);
    
    L_(linfo) << "info";
    
    L_(lwarning) << "Ops, variable x should be " << expectedX << "; is " << realX;
    
    endLogger();
#endif
}
