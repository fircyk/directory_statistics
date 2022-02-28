
#include "file_and_directory_handling.h"


int main()
{

    std::thread thread1(initialisation);

    thread1.join();


    std::thread t2(go_through_directories);

    t2.join();

    std::thread t3(display_data);

    t3.join();

   // initialisation();
   // go_through_directories();
   // display_data();



    return 0;
}

