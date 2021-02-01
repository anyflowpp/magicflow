#include<stdlib.h>
#include<functional>
#include<string.h>
#include<thread>
#include<spdlog_wrap.h>
#include<chrono>
#include<functional>
#include<Ice/Ice.h>
#include"Printer.h"
#include<gtest/gtest.h>

using namespace Demo;
using namespace std;

int argc = 1;
char* argv[]={"TestTool"};

int cmain()
{
    int status = 0;
    try {
        Ice::CommunicatorPtr ic;
        ic = Ice::initialize(argc, argv);
        Ice::ObjectPrx base = ic->stringToProxy("SimplePrinter:default -p 10000");
        PrinterPrx printer = PrinterPrx::checkedCast(base);
        if (!printer)
            throw "Invalid proxy";
 
        printer->printString("Hello World!");
        if (ic){
            ic->shutdown();
            ic->waitForShutdown();
            ic->destroy();
        }
    } catch (const Ice::Exception& ex) {
        cerr << ex << endl;
        status = 1;
    } catch (const char* msg) {
        cerr << msg << endl;
        status = 1;
    }
    return status;
}
class PrinterI:public Demo::Printer{
public:
    void printString(const std::string& s, const Ice::Current&) {
        loge("s:{}",s);
    }
};

int
smain()
{
    int status = 0;
    Ice::CommunicatorPtr ic;
    try {
        ic = Ice::initialize(argc, argv);
        Ice::ObjectAdapterPtr adapter =
            ic->createObjectAdapterWithEndpoints("aSimplePrinterAdapter", "default -p 10000");
        Ice::ObjectPtr object = new PrinterI;
        adapter->add(object, ic->stringToIdentity("SimplePrinter"));
        adapter->activate();
        ic->waitForShutdown();
    } catch (const Ice::Exception& e) {
        cerr << e << endl;
        status = 1;
    } catch (const char* msg) {
        cerr << msg << endl;
        status = 1;
    }
    if (ic) {
        try {
            ic->destroy();
        } catch (const Ice::Exception& e) {
            cerr << e << endl;
            status = 1;
        }
    }
    return status;
}
TEST(icetest,server){
    thread t1(smain);
    this_thread::sleep_for(chrono::milliseconds(100));
    while(true){
        cmain();
    }
};