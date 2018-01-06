#include <gtk/gtk.h>
#include <unistd.h>
#include <pthread.h>
#include <shadowsocks.h>
#include <sys/signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include "main.h"

GtkBuilder *layout;
GtkStatusbar *statusBar = NULL;
profile_t ssConf;

static pthread_t id = 0;

int getMethodIdx(char *method)
{
	for(int i = 0; i < 14; i++) {
		if(strcmp(method, methodList[i]) == 0) {
			//printf("Method %s\n", method);
			return i;
		}
	}
	return -1;
}

void copyData(char *d1, char *d2)
{
	d1 = (char *)malloc(strlen(d2) + 1);
	memset(d1, 0, strlen(d2));
	for(int i = 0; i < strlen(d2); i++)
		d1[i] = d2[i];
}

void config2profile(struct configFile *conf, profile_t *prof)
{
	if(conf->acl[0] != '\0') {
		prof->acl = (char *)malloc(strlen(conf->acl) + 1);
		memset(prof->acl, 0, strlen(conf->acl) + 1);
		strcpy(prof->acl, conf->acl);
	} else
		prof->acl = NULL;
	prof->auth = conf->auth;
	prof->fast_open = conf->fastOpen;

	prof->local_addr = (char *)malloc(strlen(conf->localAddr) + 1);
	memset(prof->local_addr, 0, strlen(conf->localAddr) + 1);
	strcpy(prof->local_addr, conf->localAddr);

	prof->local_port = conf->localPort;
	if(conf->log[0] != '\0') {
		prof->log = (char *)malloc(strlen(conf->log) + 1);
		memset(prof->log, 0, strlen(conf->log) + 1);
		strcpy(prof->log, conf->log);
	} else
		prof->log = NULL;
	prof->method = (char *)malloc(strlen(conf->method) + 1);
	memset(prof->method, 0, strlen(conf->method) + 1);
	strcpy(prof->method, conf->method);

	prof->mode = conf->mode;
	prof->mptcp = conf->mptcp;
	prof->mtu = conf->mtu;

	prof->password = (char *)malloc(strlen(conf->password) + 1);
	memset(prof->password, 0, strlen(conf->password) + 1);
	strcpy(prof->password, conf->password);

	prof->remote_host = (char *)malloc(strlen(conf->serverAddr) + 1);
	memset(prof->remote_host, 0, strlen(conf->serverAddr) + 1);
	strcpy(prof->remote_host, conf->serverAddr);

	prof->remote_port = conf->serverPort;
	prof->timeout = conf->timeout;
	prof->verbose = conf->verbose;
}

void profile2config(profile_t *prof, struct configFile *conf)
{
	//printf("start conver");
	if(prof->acl != NULL)
		strcpy(conf->acl, prof->acl);
	//printf("Done 1");
	conf->auth = prof->auth;
	conf->fastOpen = prof->fast_open;
	strcpy(conf->localAddr, prof->local_addr);
	conf->localPort = prof->local_port;
	if(prof->log != NULL)
		strcpy(conf->log, prof->log);
	strcpy(conf->method, prof->method);
	conf->mode = prof->mode;
	conf->mptcp = prof->mptcp;
	conf->mtu = prof->mtu;
	strcpy(conf->password, prof->password);
	strcpy(conf->serverAddr, prof->remote_host);
	conf->serverPort = prof->remote_port;
	conf->timeout = prof->timeout;
	conf->verbose = prof->verbose;
}

void saveConfig()
{
	FILE *fp;
	int ret;
	struct configFile conFile;
	char configPath[1024];

	char *home = getenv("HOME");

	memset(configPath, 0, 1024);
	strcpy(configPath, home);
	strcat(configPath, CONFIG_PATH);
	if(mkdir(configPath, 0755) != 0) {
		perror("Cant create directory");
	}
	char *filePath = (char *)malloc(1024);
	memset(filePath, 0, 1024);
	memset(&conFile, 0, sizeof(conFile));
	strcpy(filePath, configPath);
	strcat(filePath, CONFIG_FILENAME);

	fp = fopen(filePath, "wb");
	if(fp == NULL) {
		//gtk_statusbar_push(statusBar, ret, "Can not create config file");
		perror("Cant create file");
		return;
	}
	profile2config(&ssConf, &conFile);
	strcpy(conFile.name, "Default");
	ret = fwrite(&conFile, sizeof(conFile), 1, fp);
	if(ret != 1) {
		printf("Can not save server config\n");
		//gtk_statusbar_push(statusBar, ret, "Can not save server configure");
	} else
		printf("Server config saved\n");
	fclose(fp);
	free(filePath);
}

profile_t *getConfig()
{
	profile_t *profile = malloc(sizeof(profile_t));
	struct configFile configFile;
	char *home = getenv("HOME");
	char configFilePath[1024];
	FILE *fp;

	strcpy(configFilePath, home);
	strcat(configFilePath, CONFIG_PATH);
	strcat(configFilePath, CONFIG_FILENAME);

	fp = fopen(configFilePath, "rb");
	if(fp == NULL)
		return NULL;
	fread(&configFile, sizeof(configFile), 1, fp);
	printf("Here\n");
	config2profile(&configFile, profile);
	printf("%s\n", profile->local_addr);
	return profile;
}

void runShadowsocks()
{
	start_ss_local_server(ssConf);
}

void setUpConfig()
{
	GObject *obj;

	obj = gtk_builder_get_object(layout, "serverAddr");
	gtk_entry_set_text(GTK_ENTRY(obj), ssConf.remote_host);

	obj = gtk_builder_get_object(layout, "serverPortAdjustment");
	gtk_adjustment_set_value(GTK_ADJUSTMENT(obj), ssConf.remote_port);

	obj = gtk_builder_get_object(layout, "localPortAdjustment");
	gtk_adjustment_set_value(GTK_ADJUSTMENT(obj), ssConf.local_port);

	if(ssConf.password != NULL) {
		obj = gtk_builder_get_object(layout, "password");
		gtk_entry_set_text(GTK_ENTRY(obj), ssConf.password);
	}

	obj = gtk_builder_get_object(layout, "timeAdjustment");
	gtk_adjustment_set_value(GTK_ADJUSTMENT(obj), ssConf.timeout);

	obj = gtk_builder_get_object(layout, "fastOpen");
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(obj), ssConf.fast_open);

	obj = gtk_builder_get_object(layout, "method");
	gtk_combo_box_set_active(GTK_COMBO_BOX(obj), getMethodIdx(ssConf.method));

}

void setProfile()
{
	GObject *obj;

	obj = gtk_builder_get_object(layout, "serverAddr");
	ssConf.remote_host = gtk_entry_get_text(GTK_ENTRY(obj));

	obj = gtk_builder_get_object(layout, "serverPortAdjustment");
	ssConf.remote_port = (int)gtk_adjustment_get_value(GTK_ADJUSTMENT(obj));

	obj = gtk_builder_get_object(layout, "localPortAdjustment");
	ssConf.local_port = (int)gtk_adjustment_get_value(GTK_ADJUSTMENT(obj));

	obj = gtk_builder_get_object(layout, "password");
	ssConf.password = gtk_entry_get_text(GTK_ENTRY(obj));

	obj = gtk_builder_get_object(layout, "timeAdjustment");
	ssConf.timeout = gtk_adjustment_get_value(GTK_ADJUSTMENT(obj));

	obj = gtk_builder_get_object(layout, "fastOpen");
	ssConf.fast_open = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(obj));

	obj = gtk_builder_get_object(layout, "method");
	ssConf.method = methodList[gtk_combo_box_get_active(GTK_COMBO_BOX(obj))];
	printf("Set method %s\n", ssConf.method);
}

void quitProgram(GtkWidget *widget, profile_t *profile)
{
	free(profile->acl);
	free(profile->local_addr);
	free(profile->log);
	free(profile->method);
	free(profile->password);
	free(profile->remote_host);
	free(profile);
	gtk_main_quit();
}

void startClicked(GtkButton *button, gpointer data)
{
	guint msgId;
	if(id != 0) {
		if(pthread_kill(id, 0) == 0) {
			gtk_statusbar_push(statusBar, msgId, "Shadowsocks already running");
			return;
		}
	}
	setProfile();
	saveConfig();
	int ret = pthread_create(&id, NULL, (void *)runShadowsocks, NULL);
	if(ret == 0) {
		gtk_statusbar_push(statusBar, msgId, "Shadowsocks is running");
	} else {
		gtk_statusbar_push(statusBar, msgId, "Error");
	}
}

void stopClicked(GtkButton *button, gpointer data)
{
	guint msgId;
	if(id == 0) {
		gtk_statusbar_push(statusBar, msgId, "Shadowsocks not running");
		return;
	}
	if(pthread_kill(id, 0) == 0) {
		pthread_kill(id, SIGUSR1);
		gtk_statusbar_push(statusBar, msgId, "Stoped Shadowsocks");
	} else {
		gtk_statusbar_push(statusBar, msgId, "Shaodwsocks not Running");
	}
}

int main (int argc, char **argv)
{
	GObject *window;
	// Default config
	ssConf.local_addr = "127.0.0.1";
	ssConf.method = "aes-256-cfb";
	ssConf.password = NULL;
	ssConf.remote_host = "10.0.0.1";
	ssConf.remote_port = 8388;
	ssConf.local_port = 1080;
	ssConf.timeout = 300;
	ssConf.acl = NULL;
	ssConf.log = NULL;
	ssConf.fast_open = 0;
	ssConf.mode = 0;
	ssConf.verbose = 0;

	profile_t *profile = getConfig();
	if(profile != NULL) {
		ssConf = *profile;
	}

	gtk_init(&argc, &argv);
	layout = gtk_builder_new_from_file("/usr/share/shadowsocks-gtk3/ss-gtk3.glade");

	setUpConfig();

	window = gtk_builder_get_object(layout, "mainWindow");
	statusBar = GTK_STATUSBAR(gtk_builder_get_object(layout, "statusBar"));
	g_signal_connect(window, "destroy", G_CALLBACK(quitProgram), profile);
	gtk_builder_connect_signals(layout, NULL);
	gtk_widget_show_all(GTK_WIDGET(window));
	gtk_main();
}
