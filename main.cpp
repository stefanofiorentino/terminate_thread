#include <iostream>
#include <thread>
#include <chrono>
#include <future>
#include <sstream>
#include <string>
#include <map>

// ref. https://thispointer.com/c11-how-to-stop-or-terminate-a-thread/

namespace terminate
{
    namespace thread
    {
        static std::map<std::string, std::thread> threadMap;
        static std::promise<void> exitSignal;
        static std::future<void> futureObj = exitSignal.get_future();
    }
}

void safeRun(std::string const &taskName, int intraTaskLag)
{
    terminate::thread::threadMap[taskName] = std::thread([&, intraTaskLag, taskName]()
                                                         {
                                                             int counter = 0;
                                                             while (::terminate::thread::futureObj.wait_for(
                                                                     std::chrono::milliseconds(1)) ==
                                                                    std::future_status::timeout)
                                                             {
                                                                 std::this_thread::sleep_for(
                                                                         std::chrono::seconds(intraTaskLag));
                                                                 counter += intraTaskLag;
                                                                 std::cout << taskName << ": " << counter << std::endl;
                                                             }

                                                             std::cout << "Stop thread at counter: "
                                                                       << std::__cxx11::to_string(counter) << std::endl;
                                                         });
}

void safeJoin(std::string const &taskName)
{
    std::cout << "Asking `" << taskName << "' thread to stop" << std::endl;

    //Set the value in promise
    ::terminate::thread::exitSignal.set_value();
    ::terminate::thread::threadMap[taskName].join();

    ::terminate::thread::exitSignal = std::promise<void>();
    ::terminate::thread::futureObj = ::terminate::thread::exitSignal.get_future();

    std::cout << "Exiting `" << taskName << "' thread" << std::endl;
}

int main()
{
    safeRun("small", 1);
    safeRun("big", 3);

    std::this_thread::sleep_for(std::chrono::seconds(10));

    safeJoin("small");
    safeJoin("big");

    return 0;
}
