//
//  main.cpp
//  HuaJiao
//
//  Created by LowBoy on 2017/8/11.
//  Copyright © 2017年 @LowBoyTeam. All rights reserved.
//

#include <iostream>
#include <assert.h>
#include <memory>

#include "global.hpp"
#include "parse_pack.hpp"
#include "make_pack.hpp"

#include "websocket/easywsclient.hpp"
#include "utils/utils.hpp"

#ifdef _WIN32
	#ifdef _DEBUG
		#pragma comment(lib,"libprotobuf_d.lib")
	#else
		#pragma comment(lib,"libprotobuf.lib")
	#endif // _DEBUG
#endif // _WIN32

using easywsclient::WebSocket;

int main(int argc, const char * argv[]) {
	
	std::cout << RED << "========================" << GREEN << "[Hello,HuaJiao]" << RED << "=======================\n";

	std::cout << GREEN << "[please enter roomid] >>>>>> " << WHITE;

	std::string roomid;

	while (std::cin >> roomid)
	{
		if (!IsNum(roomid))
		{
			std::cerr << RED << "[ERROR] must be a number !" << std::endl;
			continue;
		}

		if (roomid.empty()) {
			std::cerr << RED << "[ERROR] roomid is empty !" << std::endl;
			continue;
		}

		if (roomid.length() != 9)
		{
			std::cerr << RED << "[ERROR] roomid length error !" << std::endl;
			continue;
		}

		if (!roomid.empty() && roomid.length() == 9)
			break;
	}

    g_UserInfo.userid		= "688961111731502278921563";
    g_UserInfo.sender		= "688961111731502278921563";
    g_UserInfo.password		= "688961111731502278921563";
    g_UserInfo.client_ram	= randomString(10);
    g_UserInfo.sn			= randomNumber(10);
    
#ifdef _WIN32
	INT rc;
	WSADATA wsaData;
	rc = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (rc) {
		printf("WSAStartup Failed.\n");
		return 1;
	}
#endif
    
    std::unique_ptr<WebSocket> ws(WebSocket::from_url(g_Config.wsServer));
    
	if (ws->getReadyState() == WebSocket::CLOSED)
	{
		std::cout << GREEN << "[WebSocket] = " << BLUE << "connection to server success ....." << std::endl;
		system("pause");
		return 0;
	}
    
    std::cout << GREEN << "[WebSocket] = " << BLUE << "connection to server success ....." << std::endl;
    
    std::string handsharkpacket = new_hand_shark_pack();
    ws->sendBinary(handsharkpacket);
    
    while (ws->getReadyState() != WebSocket::CLOSED) {
        WebSocket::pointer wsp = &*ws;
        ws->poll();
        ws->dispatch([wsp,roomid](const std::string & message) {
            
            std::cout << YELLOW << "\n[Recv] " << WHITE << "packet size = " << RED << message.length() << std::endl;
            
            HandleBinaryMessage(message.c_str(), (int)message.length());
            
            if(g_UserInfo.handshake && !g_UserInfo.bLogin)
            {
                std::string loginpacket = new_login_pack();
                wsp->sendBinary(loginpacket);
            }
            
            if(g_UserInfo.handshake && g_UserInfo.bLogin && g_UserInfo.roomId.empty())
            {
                g_UserInfo.roomId = roomid;
                std::string joinchatroom = new_join_chat_room_pack();
                wsp->sendBinary(joinchatroom);
            }
        });
    }
    
#ifdef _WIN32
	WSACleanup();
#endif

	system("pasue");

    return 0;
}
