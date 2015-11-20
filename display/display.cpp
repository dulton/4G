#include <stdio.h>
#include <unistd.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <map>

int Open_Port(char* comport)
{
    char port_tmp[20];
    sprintf(port_tmp, comport, comport);	

    int fd=0;
    // O_NOCTTY If path refers to a terminal device, do not allocate the device as the
    // controlling terminal for this process
    // can use O_NONBLOCK instead of O_NDELAY
    //fd=open(port_tmp,O_RDWR|O_NOCTTY|O_NDELAY);
    fd=open(port_tmp,O_RDWR);
    if(fd==-1){
        perror("Can not open serial port");
        return -1;
    }else{
        //printf("  -open %s ... done!\n", port_tmp);
    }

    // restore the serial port to block.
    if(fcntl(fd,F_SETFL,0)<0)
    {
        printf("fcntl failed!\n");
    }
    else
    {
        //printf("  -fcntl ... done!\n");
    }		
    if(isatty(STDIN_FILENO)==0)
    {
        printf("standard input is not a termined device\n");
    }
    else
    {
        //printf("  -istty ... done!\n");
    }		
    return fd;			
}


int Set_Opt(int fd, int nSpeed, int nBits, char nEvent, int nStop)
{
    struct termios newtio, oldtio;	
    if(tcgetattr(fd,&oldtio)!=0)
    {
        perror("Setup serial");
        return -1;
    }
    memset(&newtio, 0, sizeof(newtio));
    newtio.c_cflag|=CLOCAL|CREAD;
    newtio.c_cflag&=~CSIZE;	
    switch(nSpeed)
    {
        case 300:
            cfsetispeed(&newtio,B300);
            cfsetospeed(&newtio,B300);
            break;		
        case 600:
            cfsetispeed(&newtio,B600);
            cfsetospeed(&newtio,B600);
            break;			
        case 1200:
            cfsetispeed(&newtio,B1200);
            cfsetospeed(&newtio,B1200);
            break;			
        case 2400:
            cfsetispeed(&newtio,B2400);
            cfsetospeed(&newtio,B2400);
            break;		
        case 4800:
            cfsetispeed(&newtio,B4800);
            cfsetospeed(&newtio,B4800);
            break;			
        case 9600:
            cfsetispeed(&newtio,B9600);
            cfsetospeed(&newtio,B9600);
            break;			
        case 19200:
            cfsetispeed(&newtio,B19200);
            cfsetospeed(&newtio,B19200);
            break;			
        case 38400:
            cfsetispeed(&newtio,B38400);
            cfsetospeed(&newtio,B38400);
            break;			
        case 57600:
            cfsetispeed(&newtio,B57600);
            cfsetospeed(&newtio,B57600);
            break;		
        case 115200:
            cfsetispeed(&newtio,B115200);
            cfsetospeed(&newtio,B115200);
            break;		
        default:
            cfsetispeed(&newtio,B9600);
            cfsetospeed(&newtio,B9600);
            break;	
    }	
    switch(nBits)
    {
        case 7:
            newtio.c_cflag|=CS7;
            break;
        case 8:
            newtio.c_cflag|=CS8;
            break;			
        default:
            newtio.c_cflag|=CS8;
            break;			
    }  	
    switch(nEvent)
    {
        case 'O':
        case 'o':
            newtio.c_cflag|=PARENB;
            newtio.c_cflag|=PARODD;
            newtio.c_iflag|=(INPCK|ISTRIP);
            break;		
        case 'E':
        case 'e':
            newtio.c_iflag|=(INPCK|ISTRIP);
            newtio.c_cflag|=PARENB;
            newtio.c_cflag&=~PARODD;
            break;			
        case 'N':
        case 'n':
            newtio.c_cflag&=~PARENB;
            break;		
        default:
            newtio.c_cflag&=~PARENB;
            break;
    }  
    switch(nStop)
    {
        case 1:
            newtio.c_cflag&=~CSTOPB;
            break;			
        case 2:
            newtio.c_cflag|=CSTOPB;
            break;		
        default:
            newtio.c_cflag&=~CSTOPB;
            break; 			
    } 	
    newtio.c_cc[VTIME]=2;
    newtio.c_cc[VMIN]=0; // 0 non-block ; > 0 block
    tcflush(fd, TCIFLUSH);  
    // This function returns OK if it was able to perform any of the requested actions, even if it couldn’t
    // perform all the requested actions. If the function returns OK, it is our responsibility to
    // see whether all the requested actions were performed. This means that after we call
    // tcsetattr to set the desired attributes, we need to call tcgetattr and compare the
    // actual terminal’s attributes to the desired attributes to detect any differences.
    if((tcsetattr(fd,TCSANOW,&newtio))!=0)
    {
        perror("Com set error");
        return -1;
    }  
    //	printf("  -serial set (%d,%d,%c,%d)... done!\n", nSpeed,nBits,nEvent,nStop);
    return 0;
}

int write_read_serial(char* msg, char* result)
{
    int fd = -1;
    char *comport = "/dev/ttyUSB2";
    int length = 0;
    if((fd = Open_Port(comport)) < 0)
    {
        printf("Open uart err \n");
        return -1;
    }

    if((Set_Opt(fd,115200,8,'N',1)) < 0)
    {
        perror("Set_Opt RS485 error");
        return -2;
    }

    tcflush(fd, TCIOFLUSH);  
    length = write(fd, msg, 128);
    while((length = read(fd, result, 512)) > 0) {
        tcflush(fd, TCIOFLUSH);  
        printf("%s\n", result);
    }

}
//#include <sys/types.h>
//#include <sys/stat.h>
//#include <fcntl.h>
//#include <unistd.h>
//
//int serial(int argc, char* argv[])
//{		
//    char *comport = "/dev/ttyUSB2";
//    int speed = 115200;
//    char snd_buf[128] = "";
//    char recv_buf[512] = "";
//    int fd = -1; 
//    int length = 0;
//
//    int type = 0; // 0 read 1 write 2 readwrite 3 writeread
//    int result = 0;
//    while( (result = getopt(argc, argv, "rw:a:v:t:s:")) != -1 )
//    {
//        switch(result)
//        {
//            case 'r': // read serail port
//                type = 0; 
//                break;
//            case 'w': // write to serial port
//                type = 1;
//                memcpy(snd_buf, optarg, strlen(optarg) + 1);
//                snd_buf[strlen(optarg)] = '\r';
//                break;
//            case 'a': // read serail port and write 
//                type = 2;
//                memcpy(snd_buf, optarg, strlen(optarg) + 1);
//                snd_buf[strlen(optarg)] = '\r';
//                break;
//            case 'v': // write serial port and read
//                type = 3;
//                memcpy(snd_buf, optarg, strlen(optarg) + 1);
//                snd_buf[strlen(optarg)] = '\r';
//                break;
//            case 't': // select device /dev/ttyUSB0
//                comport = optarg;
//                break;
//            case 's': // set speed 115200 
//                speed = atoi(optarg);
//                break;
//            default:
//                printf("Usage: %s [rwavts] r read w write a readwrite v writeread t device s speed\n", argv[0]);
//                break;
//        }
//    }
//
//    if((fd = Open_Port(comport)) < 0)
//    {
//        printf("Open uart err \n");
//        return -1;
//    }
//
//    if((Set_Opt(fd,speed,8,'N',1)) < 0)
//    {
//        perror("Set_Opt RS485 error");
//        return -2;
//    }
//
//    tcflush(fd, TCIOFLUSH);  
//    switch (type){
//        case 0:
//            while((length = read(fd, recv_buf, 512)) > 0) {
//                tcflush(fd, TCIOFLUSH);  
//                printf("%s\n", recv_buf);
//            }
//            break;
//        case 1:
//            length = write(fd, snd_buf, 128);
//            break;
//        case 2:
//            while((length = read(fd, recv_buf, 512)) > 0) {
//                tcflush(fd, TCIOFLUSH);  
//                printf("%s\n", recv_buf);
//            }
//            tcflush(fd, TCIOFLUSH);  
//            length = write(fd, snd_buf, 128);
//            break;
//        case 3:
//            length = write(fd, snd_buf, 128);
//            while((length = read(fd, recv_buf, 512)) > 0) {
//                tcflush(fd, TCIOFLUSH);  
//                printf("%s\n", recv_buf);
//            }
//            break;
//        default:
//            printf("Wrong type %d\n", type);
//            break;
//    }
//    close(fd);
//    return 0;
//}

// ============================================================================
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
using namespace std;

map<char, string>DLSX;

void initializeDLSX(){
    DLSX['A'] = "00"; DLSX['B'] = "01"; DLSX['C'] = "02"; DLSX['D'] = "03";
    DLSX['E'] = "04"; DLSX['F'] = "05"; DLSX['G'] = "06"; DLSX['H'] = "07";
    DLSX['I'] = "08"; DLSX['J'] = "09"; DLSX['K'] = "0a"; DLSX['L'] = "0b";
    DLSX['M'] = "0c"; DLSX['N'] = "0d"; DLSX['O'] = "0e"; DLSX['P'] = "0f";
    DLSX['Q'] = "10"; DLSX['R'] = "11"; DLSX['S'] = "12"; DLSX['T'] = "13";
    DLSX['U'] = "14"; DLSX['V'] = "15"; DLSX['W'] = "16"; DLSX['X'] = "17";
    DLSX['Y'] = "18"; DLSX['Z'] = "19"; DLSX['&'] = "1a"; DLSX[' '] = "1b";
    DLSX['?'] = "1c"; DLSX['!'] = "1d"; DLSX['1'] = "1e"; DLSX['2'] = "1f";
    DLSX['3'] = "20"; DLSX['4'] = "21"; DLSX['5'] = "22"; DLSX['6'] = "23";
    DLSX['7'] = "24"; DLSX['8'] = "25"; DLSX['9'] = "26"; DLSX['0'] = "27";
    DLSX['"'] = "49"; DLSX['*'] = "50"; DLSX['<'] = "51"; DLSX['>'] = "52";
    DLSX['.'] = "b1"; DLSX['-'] = "c9"; DLSX['x'] = "d4"; DLSX['%'] = "d5";
    DLSX[','] = "1b"; // dlsx don't support ',' just work as space;
    DLSX['+'] = ""; // dlsx don't support ',' just work as space;
    DLSX[':'] = ""; // dlsx don't support ',' just work as space;
}
enum DisplayType
{
    DCSQ = 0,
    DPSART,
    DCOPS,
    DNETRATE,
    DGPS
};

enum DisplayDirection
{
    ToVideo = 0,
    ToLogFile,
    ToWebPage,
    ToStdout,
    None
};  
class Item
{
    public:
        Item(DisplayType type, DisplayDirection disp):m_type(type), m_disp(disp){}
        virtual int Display()=0;
        virtual void DisplayOnVideo(string &value, string &position)
        {
            string cmd = "~/bit_dlsx 81 01 04 73 " + position;
            size_t value_size = value.size();
            int actual_count = 0;
            // only display 10 chars peer item.
            for(int i = 0, actual_count = 0; actual_count < 10; i++) {
                if(i < value_size){
                    if(DLSX[value[i]] != "") {
                        cmd += " " + DLSX[value[i]];
                        actual_count++; 
                    }
                } else {
                    cmd += " 1b";
                    actual_count++;
                }
            }
            cmd += " ff";
            system(cmd.c_str());
        }
//    private:
        DisplayDirection m_disp;
        DisplayType m_type; 
};

class CSQ : public Item
{
public:
    CSQ(DisplayType type, DisplayDirection disp) : Item(type, disp){}
    virtual int Display()
    { 
        char buf[512] = {0};
        write_read_serial("ATE\r", buf);
        memset(buf, 0, sizeof(buf));
        write_read_serial("AT+CSQ\r", buf);

        string tmp(buf);
        int pos = 0, start_pos = 0;
        while((pos = tmp.find("\r\n", start_pos)) != string::npos)
        {
            string sentence = tmp.substr(start_pos, pos);
            start_pos = pos + 2;
            if(sentence.find("+CSQ:") != string::npos)
            {
                string value = sentence.substr(0, sentence.find("\r\n"));
                switch(m_disp)
                {
                    case ToVideo: {
                        string position("20");
                        DisplayOnVideo(value, position);
                        break;
                    }
                    case ToStdout:
                        break;
                    case ToLogFile:
                        break;
                    case ToWebPage:
                        break;
                    default:
                        break;
                }
                break;
            }
        }
    }
};

class PSART : public Item
{
    public:
        PSART(DisplayType type, DisplayDirection disp) : Item(type, disp){}
        virtual int Display()
        {
            char buf[512] = {0};
            write_read_serial("ATE\r", buf);
            memset(buf, 0, sizeof(buf));
            write_read_serial("AT+PSRAT\r", buf);

            string tmp(buf);
            int pos = 0, start_pos = 0;
            while((pos = tmp.find("\r\n", start_pos)) != string::npos)
            {
                string sentence = tmp.substr(start_pos, pos);
                start_pos = pos + 2;
                if(sentence.find("+PSRAT:") != string::npos)
                {
                    string value = sentence.substr(0, sentence.find("\r\n"));
                    switch (m_disp) 
                    {
                        case ToVideo: {
                            string position("21");
                            DisplayOnVideo(value, position);
                            break;
                        }
                        case ToStdout:
                            break;
                        case ToLogFile:
                            break;
                        case ToWebPage:
                            break;
                        default:
                            break;
                    }
                    break;
                }
            }
        }
};

class COPS : public Item
{
    public:
        COPS(DisplayType type, DisplayDirection disp) : Item(type, disp){}
        virtual int Display()
        {
            char buf[512] = {0};
            write_read_serial("ATE\r", buf);
            memset(buf, 0, sizeof(buf));
            write_read_serial("AT+COPS?\r", buf);

            string tmp(buf);
            int pos = 0, start_pos = 0;
            while((pos = tmp.find("\r\n", start_pos)) != string::npos)
            {
                string sentence = tmp.substr(start_pos, pos);
                start_pos = pos + 2;
                if(sentence.find("+COPS:") != string::npos)
                {
                    string value = sentence.substr(0, sentence.find("\r\n"));
                    switch (m_disp) 
                    {
                        case ToVideo: {
                            string position("22");
                            DisplayOnVideo(value, position);
                            break;
                        }
                        case ToStdout:
                            break;
                        case ToLogFile:
                            break;
                        case ToWebPage:
                            break;
                        default:
                            break;
                    }
                    break;
                }
            }

        }
};

class NETRATE : public Item
{
    public:
        NETRATE(DisplayType type, DisplayDirection disp) : Item(type, disp){}
        virtual int Display()
        {
            static int recv_bytes = 0;
            static int tras_bytes = 0;
            int current_recv = 0, current_tras = 0;

            char tmp[64] = {0};
            FILE* rp = popen("ifconfig ppp0 | grep \"RX bytes\" | cut -d : -f 2 | cut - -d \" \" -f 1", "r");
            while(fgets(tmp, sizeof(tmp), rp) != NULL)
            {
                if (tmp[strlen(tmp) - 1] == '\n') {
                    tmp[strlen(tmp) - 1] = '\0'; //去除换行符
                }
                current_recv = atoi(tmp);
            }
            pclose(rp);

            // check peer 5 seconds.
            double recv_rate = (current_recv - recv_bytes) / 5;
            recv_bytes = current_recv;

            FILE* tp = popen("ifconfig ppp0 | grep \"RX bytes\" | cut -d : -f 3 | cut - -d \" \" -f 1", "r");
            while(fgets(tmp, sizeof(tmp), tp) != NULL)
            {
                if (tmp[strlen(tmp) - 1] == '\n') {
                    tmp[strlen(tmp) - 1] = '\0'; //去除换行符
                }
                current_tras = atoi(tmp);
            }
            pclose(tp);
            double tras_rate = (current_tras - tras_bytes) / 5;
            tras_bytes = current_tras;

            char str[11] = {0};
            sprintf(str, "RX %.1lf", recv_rate);
            string value(str);

            char str1[11] = {0}; //TODO
            sprintf(str1, "TX %.1lf", tras_rate);
            string value1(str1);
            cout << value1 << endl;
            switch (m_disp)
            {
                case ToVideo: {
                    string position("23");
                    DisplayOnVideo(value, position);
                    string position1("24");
                    DisplayOnVideo(value1, position1);
                    break;
                }
                case ToStdout:
                    break;
                case ToLogFile:
                    break;
                case ToWebPage:
                    break;
                default:
                    break;
            }
        }
};

class GPS : public Item
{
    public:
        GPS(DisplayType type, DisplayDirection disp) : Item(type, disp){}
        virtual int Display()
        {

        }
};

class DisplayFactory
{
    public:
        static Item* createDisplay(DisplayType type, DisplayDirection direction)
        {
            Item* p = NULL;
            switch(type){
                case DCSQ:
                    p = new CSQ(type, direction);
                    break;
                case DPSART:
                    p = new PSART(type, direction);
                    break;
                case DCOPS:
                    p = new COPS(type, direction);
                    break;
                case DNETRATE:
                    p = new NETRATE(type, direction);
                    break;
                case DGPS:
                    p = new GPS(type, direction);
                    break;
                default:
                    break;
            }
            return p;
        }
};
// ============================================================================
#include <map>
bool parse_config(const string & filename, map<string, string> & m);

// ----------------------------------------------------------------------------
#define COMMENT_CHAR '#'

bool IsSpace(char c)
{
    if (' ' == c || '\t' == c)
        return true;
    return false;
}

bool IsCommentChar(char c)
{
    switch(c) {
        case COMMENT_CHAR:
            return true;
        default:
            return false;
    }
}

void Trim(string & str)
{
    if (str.empty()) {
        return;
    }
    int i, start_pos, end_pos;
    for (i = 0; i < str.size(); ++i) {
        if (!IsSpace(str[i])) {
            break;
        }
    }
    if (i == str.size()) { // 全部是空白字符串
        str = "";
        return;
    }

    start_pos = i;

    for (i = str.size() - 1; i >= 0; --i) {
        if (!IsSpace(str[i])) {
            break;
        }
    }
    end_pos = i;

    str = str.substr(start_pos, end_pos - start_pos + 1);
}

bool AnalyseLine(const string & line, string & key, string & value)
{
    if (line.empty())
        return false;
    int start_pos = 0, end_pos = line.size() - 1, pos;
    if ((pos = line.find(COMMENT_CHAR)) != -1) {
        if (0 == pos) {  // 行的第一个字符就是注释字符
            return false;
        }
        end_pos = pos - 1;
    }
    // 预处理，删除注释部分
    string new_line = line.substr(start_pos, start_pos + 1 - end_pos);  
    if ((pos = new_line.find('=')) == -1)
        return false;  // 没有=号

    key = new_line.substr(0, pos);
    value = new_line.substr(pos + 1, end_pos + 1- (pos + 1));

    Trim(key);
    if (key.empty()) {
        return false;
    }
    Trim(value);
    return true;
}

bool parse_config(const string & filename, map<string, string> & m)
{
    m.clear();
    ifstream infile(filename.c_str());
    if (!infile) {
        cout << "file open error" << endl;
        return false;
    }
    string line, key, value;
    while (getline(infile, line)) {
        if (AnalyseLine(line, key, value)) {
            m[key] = value;
        }
    }
}

// ============================================================================
int main(int argc, char* argv[])
{
    map<string, string> cfg;

    map<string, DisplayType> map_type;
    map_type["CSQ"] = DCSQ;
    map_type["PSART"] = DPSART;
    map_type["COPS"] = DCOPS;
    map_type["NETRATE"] = DNETRATE;
    map_type["GPS"] = DGPS;

    map<string, DisplayDirection> map_direction;
    map_direction["ToLogFile"] = ToLogFile;
    map_direction["ToStdout"] = ToStdout;
    map_direction["ToVideo"] = ToVideo;
    map_direction["ToWebPage"] = ToWebPage;

    initializeDLSX();

    while(true) 
    {
        parse_config("/mnt/nand/lte.conf", cfg);
        vector<Item*> display_list; 

        map<string, string>::iterator itc = cfg.begin();
        for(; itc != cfg.end(); ++itc)
        {
            Item* p = DisplayFactory::createDisplay(map_type[itc->first], 
                    map_direction[itc->second]);
            display_list.push_back(p);
        }

        vector<Item*>::iterator it;
        for(it = display_list.begin(); it != display_list.end(); ++it)
        {
            (*it)->Display();
        }
        sleep(5);

        for(it = display_list.begin(); it != display_list.end(); ++it)
        {
            delete (*it);
        }
    }
    return 0;
}