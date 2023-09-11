#include<iostream>
#include<fstream>
#include<string>
using namespace std;

int main(){
    ifstream file("..files/f4.txt", ios::binary);
    string word;
    int i = 0;
    while(file>>word){
        i++;
        cout<<word<<'\n';
    }
    cout<<i<<'\n';
}