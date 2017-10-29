#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <sys/types.h>
#include <iostream>
#include <stdlib.h>
#include <math.h>

using namespace std;

//The following function converts the DNS name into IP address in dotted notation

int getIPAddress(char *host , char* ipAddress){

    struct hostent *hostStruct;

    struct in_addr **list;

    hostStruct = gethostbyname(host);
         
    if (hostStruct == NULL){

        return 0;

    }
 
    list = (struct in_addr **) hostStruct->h_addr_list;
     
    for(int i = 0; list[i] != NULL; i++){

        strcpy(ipAddress , inet_ntoa(*list[i]));

        return 0;

    }
     
}

//The following is the main function that sends and receives all the requests and processes the data
  
int main(int argc, char **argv){

    //Check whether proper arguments are provided or not

    if(argc > 1){

        int mainSocket = 0;

        int check = 1;

        struct sockaddr_in serverInfo;

        string common = "cs653fall2017 ";

        //Initial message to start the data exchange with the server

        char *start = "cs653fall2017 HELLO shreyasmishr@umass.edu\n";

        uint16_t port = 0;

        char *temp = argv[1];

        char *hostname = argv[2];

        char *email = argv[3];

        char *ipAddress = new char[129];

        getIPAddress(hostname, ipAddress);

        //Port number obtained from character array that is received as an actual parameter

        for(int i = 0; i<strlen(temp); i++){

            if(isdigit(temp[i])){

                port = 10*port + (temp[i]-'0');

            }

        }

        //Check to see if the socket was created properly

        if ((mainSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        {
            cout<<"Socket could not be created"<<endl;

            return -1;

        }
      
        memset(&serverInfo, '0', sizeof(serverInfo));

        //Add the address and port parameters to the socket
      
        serverInfo.sin_family = AF_INET;

        serverInfo.sin_addr.s_addr= inet_addr(ipAddress);

        serverInfo.sin_port = htons(port);

        //Connect to the server using the created socket
      
        if (connect(mainSocket, (struct sockaddr *)&serverInfo, sizeof(serverInfo)) < 0){

            cout<<"Connection could not be made"<<endl;

            return -1;

        }

        //Send the message to the server

        send(mainSocket , start , strlen(start) , 0 );

        //Loop till BYE message is obtained

        while(check){

            char *arr = new char[256];

            //Receive the response from the server

            recv(mainSocket, arr, 256 ,0);

            double num1 = 0;

            double num2 = 0;

            char op = ' ';

            int index = 0;

            char *sendArr = new char[256];

            string s = "";

            string sEnd = "";

            string secret = "";

            int endStatus = 0;

            //parse the string to determine which kind of message the server has sent

            for(int i = 14; i<20; i++){

                s.push_back(arr[i]);

                endStatus = i;

            }

            for(int i = 14; i<78; i++){

                secret.push_back(arr[i]);

            }

            for(int i = 79; i<82; i++){

                sEnd.push_back(arr[i]);

            }

            //Mathematical operations to be done are only determined if the message is a STATUS message

            if(s=="STATUS"){

                int index = endStatus+2;

                while(index<strlen(arr) && arr[index]!='\n'){

                    if(isspace(arr[index])){

                        index++;

                        continue;

                    }

                    else if(isdigit(arr[index]) && op == ' '){

                        num1 = (double)10*num1 + (double)(arr[index]-'0');

                        index++;

                    }

                    else if(isdigit(arr[index]) && op != ' '){

                        num2 = (double)10*num2 + (double)(arr[index]-'0');

                        index++;
                        
                    }

                    else if(arr[index]=='+'||arr[index]=='-'||arr[index]=='*'||arr[index]=='/'){

                        op = arr[index];

                        index++;

                    }

                }

                if(op=='+'){

                    num1 += num2;

                }

                else if(op=='-'){

                    num1 -= num2;

                }

                else if(op=='*'){

                    num1 *= num2;
                    
                }

                else if(op=='/'){

                    num1 /= num2;

                }

                long int res = floor(num1);

                long int tempNum = res;

                string s2;

                tempNum = abs(tempNum);

                if(tempNum == 0){

                    s2.push_back('0');

                }

                while(tempNum){

                    s2.insert(s2.begin(), tempNum%10 + '0');

                    tempNum /= 10;

                }

                if(res<0){

                    s2.insert(s2.begin(), '-');

                }

                int tempIndex = 0;

                for(int i = 0; i<14; i++){

                    sendArr[i] = common[i];

                    tempIndex = i;

                }

                for(int i = 0; i<s2.length(); i++){

                    sendArr[i+14] = s2[i];

                    tempIndex = i+14;

                }

                sendArr[tempIndex+1] = '\n';

                //Send the message to the server

                send(mainSocket , sendArr , tempIndex+2 , 0 );

            }

            //Secret flag is displayed and loop is ended on receipt of the BYE message

            else if(sEnd=="BYE"){

                cout<<secret<<endl;

                check = 0;

                break;

            }

            //If server sends bogus responses, connection is ended

            else{

                cout<<"Server ended connection"<<endl;

                break;

            }

        }

        return 0;

    }

    else{

        cout<<"Specify correct number of parameters"<<endl;

    }

    return 0;

}
