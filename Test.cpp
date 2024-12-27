#include<iostream>
#include<thread>
#include<chrono>

using namespace std;

void call(){
    this_thread::sleep_for(chrono::seconds(5));
    cout<<"Hello from Call method\n";
}

int main(){
    cout<<"Hello";

    thread d(call);
    d.join();

//         user_name         | user_id | access_privilege |             password             | role_id
// --------------------------+---------+------------------+----------------------------------+---------
//  admin                    |       1 |                1 | 21232f297a57a5a743894a0e4a801fc3 |       1
//  win2019-10828-2412131731 |       2 |                4 |                                  |       4
//  win2019-10828-2412101220 |       3 |                4 |                                  |       4
//  win2019-10828            |       5 |                4 |                                  |       4
//  read@gmail.com           |       4 |              100 | 21d0e085ce9d2d985477e33598c586e1 |      10

}