#include <iostream>

#include "Publisher.h"
#include "Subscriber.h"

int main() {

    Subscriber s1;
    s1.Subscribe("tag1",[](std::string data)
    {
        std::cout << "s1: " << data << std::endl;
    });

    Subscriber s2;
    s2.Subscribe("tag2",[](std::string data)
    {
        std::cout << "s2: " << data << std::endl;
    });

    Subscriber s3;
    s2.Subscribe("tag2",[](std::string data)
    {
        std::cout << "s3: " << data << std::endl;
    });


    Publisher p1;

    p1.Publish("tag3", "S3, you are adopted");
    p1.Publish("tag2", "Hello to S2 and S3");
    p1.Publish("tag1", "Don't worry S1, we still love you");

    system("pause");

    return 0;
}