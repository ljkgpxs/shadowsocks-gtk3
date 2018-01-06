#pragma once

#define CONFIG_PATH	"/.config/shadowsocks-gtk3/"
#define CONFIG_FILENAME	"config.bin"

struct configFile {
	char serverAddr[15];
	int serverPort;
	char localAddr[15];
	int localPort;
	char password[100];
	char method[20];
	int timeout;
	int fastOpen;
	char acl[1024];
	char log[1024];
	int mode;
	int verbose;
	int mtu;
	int mptcp;
	char name[20];
};

char *methodList[] = {
	"rc4-md5",
	"aes-128-cfb",
	"aes-192-cfb",
	"aes-256-cfb",
	"aes-128-ctr",
	"aes-192-ctr",
	"aes-256-ctr",
	"bf-cfb",
	"cmaellia-128-cfb",
	"cmaellia-192-cfb",
	"cmaellia-256-cfb",
	"salsa20",
	"chacha20",
	"chacha20-lete"
};


