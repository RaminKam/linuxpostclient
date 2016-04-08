#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <netdb.h>
#include <fstream>
#include <unistd.h>
#include "base64.h"

//
#include <QtCore/QCoreApplication>
#include <QRegExp>
#include <QStringList>
#include <stdio.h>
#include <QDebug>
#include <QTextStream>

#define POST_MAIL_PORT_SMTP 2525//465
#define POST_MAIL_PORT 110  //465
#define FILE_NAME "ram.png" //"2.png"                   //"sendFile.txt"
#define PACKET_SIZE 4
#define SIZE_OF_BUFER 50000
#define NAME_FILE_FROM_SERVER "receiveFile.png" //"rec2.png"    //"receiveFile.txt"

char buf[SIZE_OF_BUFER];

int sock;
int sock_smtp;
int numOper;

int strans(int sk_str,const char* msg);
void stransOne(int sk_str,const char* msg);
std::string opF(const char* filename);
int recPostMail(char* mail,int bytes);
void recInFile(const char* dat, int length,const char *filename);

using namespace std;
int main()
{

    //Connecting with 2 sockets:
    //SMTP:
    numOper=0;
    printf("\n\t\tPost-Client:");

    struct sockaddr_in addr_smtp;
    struct hostent *host_addr_smtp;
    sock_smtp = socket(AF_INET, SOCK_STREAM, 0);
    if(sock_smtp < 0)
    {
        perror("sock_smtpet");
        exit(1);
    }
    host_addr_smtp=gethostbyname("smtp.km.ru");
    if (host_addr_smtp==0)
    {
        printf("\nError in gethostbyname(...)");
        exit(4);
    };
    addr_smtp.sin_family = AF_INET;
    addr_smtp.sin_port = htons(POST_MAIL_PORT_SMTP); // или любой другой порт...
    addr_smtp.sin_addr.s_addr = *((unsigned long*)*host_addr_smtp->h_addr_list);
    if(connect(sock_smtp, (struct sockaddr *)&addr_smtp, sizeof(addr_smtp)) < 0)
    {
        perror("connect");
        exit(2);
    }
    else
        printf("\nGood!");
    recv(sock_smtp, buf, sizeof(buf), 0);
    printf("\n%s",buf);

    //POP3:
    int bytes_read;
    struct sockaddr_in addr;
    struct hostent *host_addr;
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0)
    {
        perror("socket");
        exit(1);
    }
    host_addr=gethostbyname("pop.km.ru");
    if (host_addr==0)
    {
        printf("\nError in gethostbyname(...)");
        exit(4);
    };
    addr.sin_family = AF_INET;
    addr.sin_port = htons(POST_MAIL_PORT); // или любой другой порт...
    addr.sin_addr.s_addr = *((unsigned long*)*host_addr->h_addr_list);
    if(connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("connect");
        exit(2);
    }
    else
        printf("\nGood!");
    recv(sock, buf, sizeof(buf), 0);
    printf("\nConnect: %s",buf);

    //Choice
    int cho=1;
    cout<<"\n1)Send email\n2)Receive, delete, information email\n0)Exit\n>>";
    cin>>cho;
    char subject_send[200];


    std::string messegeSend;//="Privet I'love int.\r\n";



//        char coty[]="Privet I'love int.\r\n";
//    for(int i=0;messegeSend[i]!='\0';i++)
//        cout<<"\n"<<messegeSend[i]<<"|"<<(int)messegeSend[i]<<"|"<<i;
//    cout<<"\n\n";
//    for(int i=0;coty[i]!='\0';i++)
//        cout<<"\n"<<coty[i]<<"|"<<(int)coty[i]<<"|"<<i;

    switch(cho)
    {
    case 1:
        {

            cout<<"\nEnter subject:";
            cin>>subject_send;
            if(strlen(subject_send)==0)
                strcpy(subject_send,"default");
            QString subject_send2=QString("Subject: ")+QString(subject_send)+QString("\r\n");

            int choice;
            printf("\n1:Send text\n2:Text+file\n3:Send File\n>>");
            cin>>choice;
            //choice=3;
            strans(sock_smtp,"HELO user\r\n");
            strans(sock_smtp,"AUTH LOGIN\r\n");

            const std::string usName = "pochBonch" ;
            const std::string passw = "fantasia11" ;
            std::string encUsName = base64_encode(reinterpret_cast<const unsigned char*>(usName.c_str()), usName.length());
            std::string encPassw = base64_encode(reinterpret_cast<const unsigned char*>(passw.c_str()), passw.length());
            encUsName=encUsName+"\r\n";
            encPassw=encPassw+"\r\n";
            strans(sock_smtp,encUsName.c_str());
            strans(sock_smtp,encPassw.c_str());
            std::string strFile =opF("ram.jpg");
            std::string encFile = base64_encode(reinterpret_cast<const unsigned char*>(strFile.c_str()), strFile.length());

            strans(sock_smtp,"MAIL FROM:pochBonch@km.ru\r\n");
            strans(sock_smtp,"RCPT TO:pochBonch@km.ru\r\n");
            strans(sock_smtp,"DATA\r\n");

            stransOne(sock_smtp,"MIME-Version: 1.0\r\n");
            stransOne(sock_smtp,"Return-Path: ponchBoncev Bonch<pochBonch@km.ru>\r\n");
            stransOne(sock_smtp,"Message-ID: <123456789>\r\n");
            stransOne(sock_smtp,"X-Mailer: KM-WebMail\r\n");

            stransOne(sock_smtp,subject_send2.toStdString().c_str());
            stransOne(sock_smtp,"FROM: pochBonch Bonchev <pochBonch@km.ru>\r\n");
            stransOne(sock_smtp,"TO: pochBonch Bonchev<pochBonch@km.ru>\r\n");


            switch(choice)
            {
            case 1://Only text
                {
                    cout<<"\nEnter text:";
                    cin.get();
                    std::getline (std::cin,messegeSend);
                    messegeSend=messegeSend+string("\r\n");

                    stransOne(sock_smtp,"Content-Type: text/html; charset=UTF-8\r\n");

                    stransOne(sock_smtp,messegeSend.c_str());
                    strans(sock_smtp,".\r\n");
                    break;
                }
            case 2://text+binary file
                {
                    cout<<"\nEnter text:";
                    cin.get();
                    std::getline (std::cin,messegeSend);
                    messegeSend=messegeSend+string("\r\n");

                    stransOne(sock_smtp,"Content-Type: multipart/mixed; boundary=AAA\r\n");
                    stransOne(sock_smtp,"\r\n--AAA\r\n");
                    stransOne(sock_smtp,"Content-Type: text/html; charset=UTF-8\r\n");

                    stransOne(sock_smtp,messegeSend.c_str());
                    stransOne(sock_smtp,"\r\n--AAA\r\n");
                    stransOne(sock_smtp,"Content-Type: application/octet-stream; name=ram12.jpg\r\n");
                    stransOne(sock_smtp,"Content-Disposition: attachment; filename=ram12.jpg\r\n");
                    stransOne(sock_smtp,"Content-Transfer-Encoding: base64\r\n");
                    stransOne(sock_smtp,"X-Attachment-Id: file0\r\n");
                    stransOne(sock_smtp,(encFile).c_str());
                    stransOne(sock_smtp,"\r\n--AAA--\r\n");
                    strans(sock_smtp,".\r\n");
                    break;
                }
            case 3://binary file
                {
                    stransOne(sock_smtp,"Content-Type: multipart/mixed; boundary=AAA\r\n");
                    stransOne(sock_smtp,"\r\n--AAA\r\n");
                    stransOne(sock_smtp,"Content-Type: text/html; charset=UTF-8\r\n");
                    stransOne(sock_smtp,"\r\n");
                    stransOne(sock_smtp,"\r\n--AAA\r\n");
                    stransOne(sock_smtp,"Content-Type: application/octet-stream; name=ram12.jpg\r\n");
                    stransOne(sock_smtp,"Content-Disposition: attachment; filename=ram12.jpg\r\n");
                    stransOne(sock_smtp,"Content-Transfer-Encoding: base64\r\n");
                    stransOne(sock_smtp,"X-Attachment-Id: file0\r\n");
                    stransOne(sock_smtp,(encFile).c_str());
                    stransOne(sock_smtp,"\r\n--AAA--\r\n");
                    strans(sock_smtp,".\r\n");
                    break;
                }
            }
            break;
        }
    case 2:
        {
            strans(sock,"USER pochBonch\r\n");
            strans(sock,"PASS fantasia11\r\n");
            cout<<"\n1)LIST\n2)STAT\n3)DELE\n4)RETR\n0)Not choice\n>>";
            int ch_pop;
            cin>>ch_pop;
            switch(ch_pop)
            {
            case 1:
                {
                    strans(sock,"LIST\r\n");
                    strans(sock,0);
                    break;
                }
            case 2:
                {
                    strans(sock,"STAT\r\n");
                    break;
                }
            case 3:
                {
                    strans(sock,"LIST\r\n");
                    strans(sock,0);
                    int postId=0;
                    cout<<"\nDelete postId=";
                    cin>>postId;
                    QString com_retr;
                    com_retr=QString("DELE ")+QString::number(postId)+QString("\r\n");
                    strans(sock,com_retr.toStdString().c_str());
                    break;
                }
            case 4:
                {
                    strans(sock,"LIST\r\n");
                    strans(sock,0);
                    cout<<"\npostId=";
                    int postId=0;
                    cin>>postId;
                    QString com_retr;
                    com_retr=QString("RETR ")+QString::number(postId)+QString("\r\n");
                    bytes_read=strans(sock,com_retr.toStdString().c_str());//RETR-6 multipart
                    int beg=0;
                    while(buf[beg]!=' ')
                        ++beg;
                    int en=beg+1;
                    while(buf[en]!=' ')
                        ++en;
                    ++beg;
                    buf[en]='\0';

                    int bytes=atoi(&buf[beg]);
                    printf("\nBytes=%d",bytes);
                    char* mail=new char[bytes];
                    recPostMail(mail,bytes);
                    recInFile(mail,bytes,"New");
                //Clear string of NUll-characters
                    std::vector<int> nData;
                    for(int i=0;i<bytes;i++)
                        if(mail[i]==0)
                            nData.push_back(i);
                    if(nData.size()==0)
                        printf("\nNull termintaors are not exist.");
                    else
                    {
                        printf("\nNulls = %d",(int)nData.size());
                        for(int i=0;i<nData.size();i++)
                            printf("\n[%d]=%d",i,nData[i]);
                    }
                    QStringList lst;//Разделенный на составляюшие
                        QByteArray qByteMail(mail,bytes);
                        QString sMail(qByteMail);
                        if(sMail.size()!=bytes)
                            printf("\nConversation from QByteArray to QString is errored.");
                        else
                            printf("\nConversation is good.");
                        //Regular Expressions
                        int pos;
                        QString m_subj;
                        QString m_cont;
                        QString m_bound;
                        QString m_text_cont,text;
                        QString onlyText;//Наше сообщение
                        QString fname;//имя бинарного файла
                        QRegExp rx_subj("\\bSubject(:)");//\\bSubject:\\s*(\\w+)\r\n"
                        pos=rx_subj.indexIn(sMail);
                        m_subj=rx_subj.cap(1);
                        pos=rx_subj.pos(1)+1;
                        int lolpos=pos;
                        while(mail[lolpos]!='\r')
                            lolpos++;
                        m_subj=QString((std::string(&mail[pos],&mail[lolpos])).c_str());

                        cout<<"\nSubject="<<m_subj.toStdString();

                        QRegExp rx_cont("\\bContent-Type:\\s*(\\w+)/");
                        pos=rx_cont.indexIn(sMail);
                        m_cont=rx_cont.cap(1);
                        cout<<"\nContent="<<m_cont.toStdString();
                        if(m_cont==QString("multipart"))
                        {
                            //cout<<"\nYes-Cont";
                            QRegExp rx_bound("\\bboundary\\s*=\\s*(\"){0,1}(\\w+)(\"){0,1}\\b");
                            pos=rx_bound.indexIn(sMail);
                            m_bound=rx_bound.cap(2);
                            cout<<"\nBoundary="<<m_bound.toStdString();

                            //Первая текстовая часть - получаем доступ к ней, если она есть
                            QRegExp rx_cont_text("\\bContent-Type:\\s*(\\w+)/");
                            pos=rx_cont_text.indexIn(sMail,pos);
                            m_text_cont=rx_cont_text.cap(1);
                            pos=rx_cont_text.pos(1);
                            cout<<"\nTextcontent="<<m_text_cont.toStdString();
                            if(m_text_cont==QString("text"))
                            {
                //                QRegExp rx_text("");
                //                rx_text.indexIn(sMail);
                //                text=rx_text.cap(1,pos);
                //                text_beg=rx_text.pos(1);
                //                cout<<"\nFound="<<text.toStdString();
                                //cout<<"\ncout="<<mail[text_beg];
                                lst=sMail.split(QRegExp(QString("\r\n--")+m_bound+QString("\r\n")));
                                cout<<"\nlst.size="<<lst.size();
                                QString qstr1=lst.takeAt(1);
                                QRegExp rx_text(".+\r\n\r\n(.+)");
                                rx_text.indexIn(qstr1);
                                onlyText=rx_text.cap(1);
                                //cout<<"\nonlyText="<<onlyText.toStdString();
                            }
                            //Return post before view with null-terms
                            for(int i=0;i<nData.size();i++)
                                mail[nData[i]]=0;

                            int bbeg,bend;
                            int pos1,pos2;
                            QString qstr2=lst.takeAt(1);
                            QRegExp rx_bin("\\bname=\\s*(\"){0,1}(\\w+.\\w+)(\"){0,1}\\b");
                            rx_bin.indexIn(qstr2);
                            fname=rx_bin.cap(2);
                            fname=QString("received_")+fname;
                            cout<<"\nfilename="<<fname.toStdString();

                            QRegExp rx_bn(QString("--")+m_bound);
                            rx_bn.indexIn(sMail);
                            rx_bn.cap(0);
                            pos1=rx_bn.pos();
                            rx_bn.indexIn(sMail,pos1+1);
                            rx_bn.cap(0);
                            pos1=rx_bn.pos();
                            rx_bn.indexIn(sMail,pos1+1);
                            rx_bn.cap(0);
                            pos2=rx_bn.pos();
                            //cout<<"\nposs"<<mail[pos1-10]<<":"<<pos1<<" "<<mail[pos2+5]<<":"<<pos2;
                            int sum_rn=0;
                            for(int k=pos1;k<bytes-1;k++)
                            {
                               // cout<<"\nm="<<mail[k]<<":"<<(int)mail[k];
                                if(mail[k]=='\r' && mail[k+1]=='\n')
                                    ++sum_rn;
                                if (sum_rn==5)
                                {
                                    bbeg=k+4;
                                    break;
                                }
                            }
                //            for(int ki=bbeg;ki<bbeg+20;ki++)
                //                cout<<"\nm="<<mail[ki]<<"  :   "<<(int)mail[ki];
                            bend=pos2-3;
                            //cout<<"\nbeg="<<mail[bbeg]<<" bend="<<mail[bend];
                           char* rec_post_file=new char[bend-bbeg+1];
                           for(int ki=bbeg,y=0;ki<(bend-bbeg+1);ki++,y++)
                               rec_post_file[y]=mail[ki];
                           std::string encoded=std::string(rec_post_file,bend-bbeg+1);
                           std::string decoded = base64_decode(encoded);
                           recInFile(decoded.c_str(), decoded.size(),fname.toStdString().c_str());
                           delete[] rec_post_file;
                           cout<<"\n\n\t\tText in Post:\n"<<onlyText.toStdString().c_str();



                        }
                        else
                        {
                            cout<<"\nNot-Multipart";
                            if(m_cont==QString("text"))
                            {
                                QRegExp rx_cont("\\b(Content-Type)");
                                pos=rx_cont.indexIn(sMail);
                                rx_cont.cap(1);
                                pos=rx_cont.pos(1);
                //                while(mail[pos]!='\n')
                //                    pos++;
                //                while(mail[pos]=='\n' || mail[pos]=='\r')
                //                    pos++;
                                for(;pos<bytes;pos++)
                                    if(mail[pos]=='\r' && mail[pos+1]=='\n'&& mail[pos+2]=='\r'&& mail[pos+3]=='\n')
                                        break;
                                pos=pos+4;


                                onlyText=QString(&mail[pos]);
                                cout<<"\n\n\t\tText in Post:\n"<<onlyText.toStdString().c_str();
                            }
                        }


                //Content-Type: multipart/mixed;


                        //Return post before view with null-terms
                    for(int i=0;i<nData.size();i++)
                        mail[nData[i]]=0;
                    delete[] mail;
                    break;
                }
            }
            break;
        }
    case 0:
        {

            break;
        }
    default:

        {

            cout<<"\nYour choice not found.";
            //...
        }
    }
    //Close connectings
    //strans(sock_smtp,".\r\n");
    strans(sock_smtp,"QUIT\r\n");
    close(sock_smtp);

    strans(sock,"QUIT\r\n");

    close(sock);

    return 0;
}


void recInFile(const char* dat, int length,const char* filename)
{
    FILE* fl;
    fl=fopen(filename,"w+t");
    for(int i=0;i<length;i++)
    {
        fputc(dat[i],fl);
    }
    fclose(fl);

}
int strans(int sk_str,const char* msg)//simple transaction with showing
{
    printf("\n\t----\t-----\t[%d]\t----\t-----",numOper);
    int bytes_send,bytes_read;
    if(msg!=0)
    {
        bytes_send=send(sk_str, msg, strlen(msg), 0);
        if(bytes_send<=0)
        {
            printf("\nError send comand.");
            return 0;
        }
        printf("\nSended: <%s>",msg);
    }
    bytes_read=recv(sk_str, buf, sizeof(buf), 0);
    if(bytes_read<=0)
    {
        printf("\nError receive answer.");
        return 0;
    }
    //printf("\nAnswered: <%s>",buf);
    printf("\nAnswered: <");
    cout.write (buf,bytes_read);
    printf(">");
    numOper++;
    return bytes_read;
}
void stransOne(int sk_str,const char* msg)
{
    printf("\n\t----\t-----\t[%d]\t----\t-----",numOper);
    int bytes_send;
    bytes_send=send(sk_str, msg, strlen(msg), 0);
    if(bytes_send<=0)
    {
        printf("\nError send comand.");
        return;
    }
    printf("\nSended: <%s>",msg);
    numOper++;
}
std::string opF(const char* filename)
{
  int length=0;
  char * buffer;

  ifstream is;
  is.open (filename, ios::binary );

  // get length of file:
  is.seekg (0, ios::end);
  length = is.tellg();
  is.seekg (0, ios::beg);

  // allocate memory:
  buffer = new char [length];

  // read data as a block:
  is.read (buffer,length);
  is.close();

  //cout.write (buffer,length);
  std::string ret(buffer,length);
  //cout.write (ret.c_str(),length);
  delete[] buffer;
  return ret;
}
//mail!=buf
int recPostMail(char* mail,int bytes)
{
    if (mail==0)
        return 0;
    int bytes_read,summBytes=0;
    while(summBytes<bytes)
    {
        bytes_read=recv(sock, mail+summBytes, bytes-summBytes, 0);
        if(bytes_read<=0)
        {
            printf("\nError receive data!");
            return summBytes;
        }
        //printf("\nBlock:");
        //cout.write (mail+summBytes,bytes_read);
        summBytes+=bytes_read;
        //printf("\nEndBlock");
    }

    int summ=0;
    while(summ<5)
    {
        bytes_read=recv(sock, buf+summ, 5-summ, 0);
        summ+=bytes_read;

    }
    int beg=0;
    while(buf[beg]!='\r')
        beg++;
    if(buf[beg+0]=='\r' && buf[beg+1]=='\n' && buf[beg+2]=='.' && buf[beg+3]=='\r' && buf[beg+4]=='\n')
        printf("\nPost succesfully received.");
    else
    {
        printf("\nError received post.");
//        cout.write (buf,bytes_read);
//        printf(">\n\n");
//        for(int i=0;i<bytes_read;++i)
//            printf("\n[%d]=<%c>=%d",i,buf[i],(int)buf[i]);
    }
    return summBytes;
}
